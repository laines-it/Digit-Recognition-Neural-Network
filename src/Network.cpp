#include "Network.hpp"
#include <iostream>
#include <fstream>

Network::Network(const std::vector<int> &layout) {
	zeroed = false;
	layers.reserve(layout.size());
	error = 0;
	averageError = 0;
	for (int l = 0; l < layout.size(); l++) {
		layers.push_back(Layer());
		int activationAmt = l == layout.size() - 1 ? 0 : layout[l + 1];

		layers.back().reserve(layout[l]);
		for (int n = 0; n <= layout[l]; n++)
			layers.back().push_back(Neuron(activationAmt, n));

		layers.back().back().setActivation(1);
	}
}

void Network::getResults(std::vector<double> &results) const {
	results.clear();
	results.reserve(layers.back().size());

	for (int n = 0; n < layers.back().size() - 1; n++) {
		results.push_back(layers.back()[n].getActivation());
	}
}

void Network::backProp(const std::vector<double> &targets) {
	
	Layer &outputLayer = layers.back();
	error = 0.0;

	for (int n = 0; n < outputLayer.size() - 1; n++) {
		double delta = targets[n] - outputLayer[n].getActivation();
		error += delta * delta;
	}
	
	error /= outputLayer.size() - 1;
	error = sqrt(error);

	averageError = (averageError * SMOOTHING + error) / (SMOOTHING + 1.0);
	// ++smoothingFactor;
	//averageError = error;

	for (int n = 0; n < outputLayer.size() - 1; n++) {
		outputLayer[n].calculateOutputGradients(targets[n]);
	}

	for (int layerNum = layers.size() - 2; layerNum > 0; --layerNum) {
		for (int n = 0; n < layers[layerNum].size(); n++) {
			layers[layerNum][n].calculateHiddenGradients(layers[layerNum + 1]);
		}
	}

	for (int layerNum = layers.size() - 1; layerNum > 0; layerNum--) {
		for (int n = 0; n < layers[layerNum].size() - 1; n++) {
			layers[layerNum][n].updateWeights(layers[layerNum - 1]);
		}
	}
}

void Network::feedForward(const std::vector<double> &inputVals, bool init) {
	for (int i = 0; i < inputVals.size(); ++i) {
		//Fill the first (input) layer
		layers[0][i].setActivation(inputVals[i]);
	}
	for (int layerNum = 1; layerNum < layers.size(); ++layerNum) {
		for (int n = 0; n < layers[layerNum].size() - 1; ++n)
			layers[layerNum][n].feedForward(layers[layerNum - 1]);
	}
}

void Network::setWeights(const std::vector<double>& weights, const std::vector<double>& deltas){
	unsigned i = 0;
	for(Layer layer : layers){
		for (Neuron neuron : layer){
			for (Connection connection : neuron.getForwardLinks()){
				connection.weight = weights[i++];
				connection.deltaWeight = deltas[i++];
			}
		}
	}
}

void Network::outputWeights(std::ofstream& out) const {
	for(Layer layer : layers){
		for (Neuron neuron : layer){
			for (Connection connection : neuron.getForwardLinks()){
				out << connection.weight << " " << connection.deltaWeight << std::endl;
			}
		}
	}
}
