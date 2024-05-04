#include "File.hpp"
#include <fstream>
#include <iostream>

std::vector<std::string> File::split(std::string str, char c) {
	std::vector<std::string> array;
	std::string element = "";

	for (int i = 0; i < str.length(); ++i) {
		if (str[i] != c)
			element += str[i];
		else if (str[i] == c && element != "") {
			array.push_back(element);
			element = "";
		}
	} if (element != "")
		array.push_back(element);

	return array;
}

File::File(const std::string& filePath, DataType datatype) {
	MAX_ITERATIONS = 0;
	std::ifstream file(filePath);

	if (file.is_open()) {
		std::string line;
		std::vector<std::string> part;
		switch (datatype){
		case MNIST:
			while (std::getline(file, line)){
				++MAX_ITERATIONS;
				part = split(line, ',');
				int t = atoi(part[0].c_str());
				std::vector<double> target;
				for(int i = 0; i < 10; ++i){
					target.push_back((i==t)?1:0);
				}
				targets.push_back(target);

				std::vector<double> i;
				for (int p = 1; p < part.size(); ++p)
						i.push_back(atof(part[p].c_str()));
				inputs.push_back(i);
				data_size = i.size();
			}
			if(file.bad()) throw std::runtime_error("File error\n");
			if(!file.eof()) throw std::runtime_error("File format error\n");
			layout = {(int)data_size, INNERLEVER, INNERLEVER, 10};
		    break;
		case WEIGHTS:
			std::vector<double> weights;
			std::vector<double> deltas;
			while(std::getline(file, line)){
				part = split(line, ' ');
				weights.push_back(atof(part[0].c_str()));
				deltas.push_back(atof(part[1].c_str()));
			}
			if(file.bad()) throw std::runtime_error("File error\n");
			if(!file.eof()) throw std::runtime_error("File format error\n");
			inputs.push_back(weights);
			targets.push_back(deltas);
			layout = {DATA_SIZE, INNERLEVER, INNERLEVER, 10};
			break;
		}
	}else{
		throw std::runtime_error("Cannot open file \n");
	}
	file.close();
}