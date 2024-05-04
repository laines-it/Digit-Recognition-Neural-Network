#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string>
#include <cmath>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Box.H>
#include "Fl_Trace_Window.h"
#include <sstream>
#include <iomanip>
#include <unistd.h>

#define SCALE 20
#define INPUTSIZE 28

class Neuron;
typedef std::vector<Neuron> Layer;

bool debug = false;

class File {
	
	int MAX_ITERATIONS; // first line
	std::vector<int> layout;//second line

	std::vector< std::vector<double> > inputs; //even lines
	std::vector< std::vector<double> > targets; //odd lines
	int DATA_SIZE; //size of inputs

public:
	File(const char* filePath, bool mnist);
	std::vector<double> getInputs(const int index) const { return inputs[index]; };
	std::vector<double> getTargets(const int index) const { return targets[index]; };
	std::vector<int> getLayout() const { return layout; };
	int getMaxIterations() const { return MAX_ITERATIONS; };
	int getDataSize() const { return DATA_SIZE; };
};

std::vector<std::string> split(std::string str, char c) {
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

File::File(const char* filePath, bool mnist = false) {

	std::string line;
	std::vector<std::string> part;
	std::ifstream file(filePath);

	if (file.is_open()) {
		if(mnist){
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
				DATA_SIZE = i.size();
			}
			layout = {(int)DATA_SIZE, 16, 16, 10};
		}else{
			if(debug) std::cout << "File " << filePath << " opened\n";
			int index = 0;
			while (std::getline(file, line)){
				if (index == 0){
					MAX_ITERATIONS = atoi(line.c_str());
					if(debug) std::cout << "max_iterations = " << MAX_ITERATIONS << std::endl;
				}
				else if (index == 1) {
					part = split(line, ' ');
					for (int p = 0; p < part.size(); ++p)
						layout.push_back(atoi(part[p].c_str()));
				}
				else if (index % 2 == 0) {
					std::vector<double> i;
					part = split(line, ' ');
					for (int p = 0; p < part.size(); ++p)
						i.push_back(atof(part[p].c_str()));
					inputs.push_back(i);
				}
				else {
					std::vector<double> t;
					part = split(line, ' ');
					for (int p = 0; p < part.size(); ++p)
						t.push_back(atof(part[p].c_str()));
					targets.push_back(t);
				}
				index++;
			}
		}
	}else{
		std::cout << "Cannot open file " << filePath << std::endl;
	}
	if(!mnist) DATA_SIZE = inputs.size();
	if(debug) std::cout << "DATA_SIZE = " << DATA_SIZE << std::endl;
	sleep(1);
	file.close();
}


std::vector<double> inputdraw;

class Canvas : public Fl_Box{
	
	// (x,y) = INPUTSIZE*y + x
	bool pixelState[INPUTSIZE * INPUTSIZE];
	
	int location(int x, int y){
		return INPUTSIZE*(y/SCALE) + x/SCALE;
	}
public:

	static void submit_callback(Fl_Widget * w, void* pixels){
		Fl_Widget *p = w->parent();//get canvas
		
		Fl_Widget *q;
		do{
			q = p->parent();
			if(q) p = q;
		}while(q);
		w->hide();
	}

	Canvas() : Fl_Box(0,0, INPUTSIZE * SCALE, INPUTSIZE * SCALE){
		Fl_Button *submit = new Fl_Button(INPUTSIZE * SCALE + 100, 0, 40, 20, "SUBMIT");
		
		for (int i = 0; i < INPUTSIZE * INPUTSIZE; i++)
			pixelState[i] = 0;
		submit->callback(submit_callback, (void*)"Hi");
	}


	int handle(int event) override {
      if (Fl::event_inside(0, 0, w(), h())) {
		switch (event){
		case FL_PUSH:
			std::cout << "pushed\n";
			return 1;
		case FL_DRAG:
			fl_color(FL_DARK_BLUE);
			fl_begin_polygon();
			fl_circle(Fl::event_x(), Fl::event_y(), (SCALE-1)/2);
			pixelState[location(Fl::event_x(), Fl::event_y())] = true;
			fl_end_polygon();
			break;
        case FL_RELEASE: 
			std::cout << "released\n";
			for (int i = 0; i < INPUTSIZE; ++i){	
				for (int j = 0; j < INPUTSIZE; ++j){
					inputdraw.push_back(pixelState[INPUTSIZE*i + j] ? 255 : 0);
				}
			}
			break;
        case FL_MOUSEWHEEL:
			std::cout << "mwheelood\n";
			break;
		default:
			break;
		}
      }
      return Fl_Box::handle(event);
    }
	 void hide() override {
      Canvas::hide();
      exit(0);
    }
    
};

class MainWindow : public Fl_Window{
	Canvas * canvas = new Canvas();
	public:
	MainWindow() : Fl_Window(1000,600){
		canvas->label("Draw here");
		canvas->color(FL_YELLOW);
		canvas->box(FL_DOWN_BOX);
	}
};

struct Connection {
	double weight;
	double deltaWeight;
};

class Neuron;

typedef std::vector<Neuron> Layer;

class Neuron {

	static double learningRate;
	static double alpha;
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

	//connections FROM this to others
	std::vector<Connection> getForwardLinks() const { return forwardLinks; }
	
	//weights of connections TO this neuron from behind
	void updateWeights(Layer &prevLayer); 
};

double Neuron::learningRate = 0.2;
double Neuron::alpha = 0.5;

Neuron::Neuron(int activationAmt, int index) {
	this->index = index;
	forwardLinks.reserve(activationAmt);

	for (int i = 0; i < activationAmt; i++) {
		forwardLinks.push_back(Connection());
		forwardLinks.back().weight = random();
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

		double newDeltaWeight = learningRate * prevLayer[n].getActivation() * gradient + alpha * oldDeltaWeight;

		prevLayer[n].forwardLinks[index].deltaWeight = newDeltaWeight;
		prevLayer[n].forwardLinks[index].weight += newDeltaWeight;
	}
}



class MainWindow;

class Network {

private:
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
	void feedForward(const std::vector<double> &inputs);
	void backProp(const std::vector<double> &targets);
	void getResults(std::vector<double> &results) const; //get activations of output layer
	double getRecentAverageError(void) const { return averageError; }
	std::vector<Layer> getLayers() const { return layers; }
	int test(MainWindow * window) {

			window->show();
			
			std::cout << "GO FL!\n";
			Fl::run();
			std::cout << "FL ENDED! with size " << inputdraw.size() << "\n";

			for (int i = 0; i < INPUTSIZE; ++i){
				std::cout << "input>";
				for (int j = 0; j < INPUTSIZE; ++j){
					std::cout << " " << inputdraw[INPUTSIZE*i + j];	
				}
				std::cout << "\n";
			}
			
			feedForward(inputdraw);
			inputdraw.clear();

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


double Network::smoothingFactor = 100;


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

	averageError = (averageError * smoothingFactor + error) / (smoothingFactor + 1.0);
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

void Network::feedForward(const std::vector<double> &inputVals) {
	for (int i = 0; i < inputVals.size(); ++i) {
		//Fill the first (input) layer
		layers[0][i].setActivation(inputVals[i]);
	}
	for (int layerNum = 1; layerNum < layers.size(); ++layerNum) {
		for (int n = 0; n < layers[layerNum].size() - 1; ++n)
			layers[layerNum][n].feedForward(layers[layerNum - 1]);
	}
}

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

int main(int argc, char *argv[]) {
	std::cout << "HELLO\n";
	File file("MNIST_test.txt", true);
	srand(time(NULL));
	Network net(file.getLayout());

	int iteration = 0;
	int total_iteration = 0;
	
	while (iteration < file.getMaxIterations()) {

		if(debug)
		std::cout << "Iteration: " << iteration << std::endl;

		std::vector<double> inputs = file.getInputs(iteration % file.getDataSize());
		net.feedForward(inputs);

		if(debug)
		std::cout << "Inputs: " << std::flush;

		if(debug)
		for (int i = 0; i < inputs.size(); i++)
			std::cout << inputs[i] << " " << std::flush;

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
	std::cout << "TRAINING OVER\n";
	
	std::ofstream file_weights("weights.txt", std::ofstream::out);

	std::cout << "OPENED\n";

	if(file_weights.is_open())
		for(int i = 0; i < net.getLayers().size() - 1; ++i){
			std::cout << "LAYER\n";
			for(Neuron neuro : net.getLayers()[i]){
				std::cout << "NEURON\n";
				for(Connection con : neuro.getForwardLinks()){
					std::cout << "CONNECTION\n";
					file_weights << con.weight << "," << con.deltaWeight << '\n';
				}
			}
		}
	else
		std::cout << "Error with openning file weights.txt\n";

	std::cout << "ALL WRITTEN\n";

	file_weights.close();

	Fl::add_handler([](int event)->int {return event == FL_SHORTCUT && Fl::event_key() == FL_Escape;});
			
	while(true)
		net.test(new MainWindow());
	return 0;
	iteration = 0;
	net.zero();
	File file2("MNIST_test.txt", true);

	while (iteration < file2.getMaxIterations()) {

		std::vector<double> inputs = file2.getInputs(iteration % file2.getDataSize());
		net.feedForward(inputs);

		std::vector<double> targets = file2.getTargets(iteration % file2.getDataSize());
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
		
		

		if(iteration == file2.getMaxIterations()){

			iteration = 0;
			
			std::string choice;
			if(debug){
				// std::cout << "Neural network has reached expected amount of iterations." << std::endl;
				// std::cout << "Enter Y to test the neural network, enter N to keep training it" << std::endl;
				// std::cin >> choice;
				// std::cin.ignore(256, '\n');
			}
			
			if(error <= 1){

			// if (choice == "Y" || choice == "y") {
				Fl::add_handler([](int event)->int {return event == FL_SHORTCUT && Fl::event_key() == FL_Escape;});
			
				while(true)
					net.test(new MainWindow());
				return 0;
			}
		
		}
		
	}
	return 0;
}