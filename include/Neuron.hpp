#ifndef NEURON_HPP
#define NEURON_HPP
#include <vector>
#include <cmath>

struct Connection {
	double weight;
	double deltaWeight;
};

class Neuron;

typedef std::vector<Neuron> Layer;

class Neuron {
	//static double activate(double value) { return tanh(value); }
	static double activate(double value) { return 1 / (1 + exp(-value)); }
	//static double activateDerivative(double value) { return 1 - (value) * (value); }
	static double activateDerivative(double value) { return activate(value) * (1 - activate(value)); }
	static double random(void) { return rand() / double(RAND_MAX); }
	double sumDOW(const Layer &nextLayer) const;
	double activation;
	std::vector<Connection> forwardLinks; 
	int index;
	double gradient;

public:
	Neuron(int activationAmt, int index);
	void setActivation(double value) { activation = value; }
	double getActivation(void) const { return activation; }	
	void feedForward(const Layer &prevLayer);
	void calculateOutputGradients(double target);
	void calculateHiddenGradients(const Layer &nextLayer);

	void setWeights(const std::vector<double> &weights);

	//connections FROM this to others
	std::vector<Connection> getForwardLinks() const { return forwardLinks; }
	
	//weights of connections TO this neuron from behind
	void updateWeights(Layer &prevLayer); 
};

#endif