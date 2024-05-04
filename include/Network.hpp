#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <vector>
#include "Neuron.hpp"
#include "GUI.hpp"

class Network {
	std::vector<Layer> layers;
	double error;
	double averageError;
	static double smoothingFactor;
	bool zeroed;
public:
	Network(const std::vector<int> &layout);
	void zero(){ averageError = 0; }
	bool get_zeroed(){ return zeroed; }
	void set_zeroed(bool b){ zeroed = b; }
	void feedForward(const std::vector<double> &inputs, bool init = true);
	void backProp(const std::vector<double> &targets);
	void getResults(std::vector<double> &results) const; //get activations of output layer
	double getRecentAverageError(void) const { return averageError; }
	std::vector<Layer> getLayers() const { return layers; }

	void setWeights(const std::vector<double>& weights, const std::vector<double>& deltas);

	void outputWeights(std::ofstream& out) const;

	int test(MainWindow * window) {

			window->show();
			Canvas* canvas = window->getCanvas();
			std::cout << "GO FL!\n";
			Fl::run();
			std::cout << "FL ENDED! with size " << canvas->inputdraw.size() << "\n";

			for (int i = 0; i < INPUTSIZE; ++i){
				std::cout << "input>";
				for (int j = 0; j < INPUTSIZE; ++j){
					std::cout << " " << canvas->inputdraw[INPUTSIZE*i + j];	
				}
				std::cout << "\n";
			}
			
			feedForward(canvas->inputdraw);
			canvas->inputdraw.clear();

			std::vector<double> results;
			getResults(results);
			
			std::cout << std::endl << "Results: " << std::flush;
			for (int i = 0; i < results.size(); i++)
				std::cout << results[i] << " " << std::flush;


			int guess = 0;
			double confidence = 0;
			for (int i = 0; i < results.size(); i++) {
				
				if (results[i] > results[guess]) {
					guess = i;
					confidence = results[i];
				}
			}
			std::cout << std::endl << guess << ". Confidence: " << confidence*100 << "%" << std::endl;
			return guess;
	}
};
#endif