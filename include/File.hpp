#ifndef FILE_HPP
#define FILE_HPP
#include <vector>
#include <string>
#include "Constants.hpp"

enum DataType { MNIST, WEIGHTS };

class File {
	
	int MAX_ITERATIONS; // first line
	std::vector<int> layout;//second line

	std::vector< std::vector<double> > inputs; //even lines
	std::vector< std::vector<double> > targets; //odd lines	
	int data_size; //size of inputs

public:
	File(const std::string& filePath, DataType dataType);
    std::vector<std::string> split(std::string str, char c);
	std::vector<double> getInputs(const int index) const { return inputs[index]; };
	std::vector<double> getTargets(const int index) const { return targets[index]; };
	std::vector<int> getLayout() const { return layout; };
	int getMaxIterations() const { return MAX_ITERATIONS; };
	int getDataSize() const { return data_size; };
};

#endif