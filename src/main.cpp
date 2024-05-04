#include <iostream>
#include "File.hpp"
#include "Network.hpp"
#include <fstream>
#include "GUI.hpp"

int main(int argc, char *argv[]) {
	std::cout << "HELLO\n";
	srand(time(NULL));
	Network net({DATA_SIZE, INNERLEVER, INNERLEVER, 10});
	std::cout << "Start Training or Load Existing? (1 or 2)" << std::endl;
	char choice;
	std::cin >> choice;
	if(choice == '1'){
		
		try{
			File file(PATH_MNIST_TRAIN, MNIST);
			int iteration = 0;
			int total_iteration = 0;
			std::cout << file.getMaxIterations() << std::endl;
			while (iteration < file.getMaxIterations()) {

				std::vector<double> inputs = file.getInputs(iteration % file.getDataSize());
				net.feedForward(inputs);

				std::vector<double> targets = file.getTargets(iteration % file.getDataSize());
				net.backProp(targets);

				std::cout << std::endl << "Targets: " << std::flush;
				for (int i = 0; i < targets.size(); i++)
					std::cout << targets[i] << " " << std::flush;
				
				std::cout << std::endl << "Results: " << std::flush;
				std::vector<double> results;
				net.getResults(results);
				for (int i = 0; i < results.size(); i++)
					std::cout << results[i] << " " << std::flush;
				

				iteration++;

				double error = net.getRecentAverageError();

				std::cout << std::endl << "ITERATION " << iteration << ". Average error: " << error  << std::endl;
				
				
			}

		}catch (std::runtime_error &e){
			std::cout << e.what() << std::endl;
			return 1;
		}

		std::cout << "TRAINING OVER\n";
		std::cout << "Update model? (y/n)\n";
		std::cin >> choice;
		if(choice == 'y'){
			std::ofstream file_weights;
			file_weights.open(PATH_WEIGHTS_TXT);

			if(file_weights.is_open())
				net.outputWeights(file_weights);
			else
				std::cout << "Error with openning file weights.txt\n";
			file_weights.close();
			std::cout << "MODEL UPDATED\n";
		}
	}
	else if(choice == '2'){

		try{
			File file_weights(PATH_WEIGHTS_TXT, WEIGHTS);
			net.setWeights(file_weights.getInputs(0), file_weights.getTargets(0));
		}catch(std::runtime_error& e){
			std::cout << e.what() << std::endl;
			return 1;
		};
        std::cout << "MODEL LOADED\n";
	}

	Fl::add_handler([](int event)->int {return event == FL_SHORTCUT && Fl::event_key() == FL_Escape;});
	
	while(true)
		net.test(new MainWindow());
	return 0;
}