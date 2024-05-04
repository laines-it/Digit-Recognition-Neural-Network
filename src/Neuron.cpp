#include "Neuron.hpp"
#include "Constants.hpp"
Neuron::Neuron(int activationAmt, int index) {
	this->index = index;
	forwardLinks.reserve(activationAmt);

	for (int i = 0; i < activationAmt; i++) {
		forwardLinks.push_back(Connection());
		forwardLinks.back().weight = random();
	}
}

void Neuron::setWeights(const std::vector<double>& weights){
	for(int i = 0; i < weights.size(); ++i){
    	forwardLinks[i].weight = weights[i];
    }
}

void Neuron::calculateOutputGradients(double target) {
	double delta = target - activation;
	gradient = delta * Neuron::activateDerivative(activation);
}

double Neuron::sumDOW(const Layer &nextLayer) const {

	//maybe, DOW is Derivative Of Weights

	double sum = 0.0;

	for (int n = 0; n < nextLayer.size() - 1; n++)
		sum += forwardLinks[n].weight * nextLayer[n].gradient;

	//sum = Xw;

	return sum;
}

void Neuron::calculateHiddenGradients(const Layer &nextLayer) {
	gradient = sumDOW(nextLayer) * Neuron::activateDerivative(activation);
}

void Neuron::feedForward(const Layer &prevLayer) {
	double sum = 0.0;

	for (int n = 0; n < prevLayer.size(); n++)
		sum += prevLayer[n].getActivation() * prevLayer[n].forwardLinks[index].weight;

	activation = Neuron::activate(sum);
}


void Neuron::updateWeights(Layer &prevLayer) {
	for (int n = 0; n < prevLayer.size(); n++) {
		double oldDeltaWeight = prevLayer[n].forwardLinks[index].deltaWeight;

		double newDeltaWeight = LEARNINGRATE * prevLayer[n].getActivation() * gradient + ALPHA * oldDeltaWeight;

		prevLayer[n].forwardLinks[index].deltaWeight = newDeltaWeight;
		prevLayer[n].forwardLinks[index].weight += newDeltaWeight;
	}
}