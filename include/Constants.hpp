#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
const int SCALE = 20;
const int INPUTSIZE = 28;
const int SMOOTHING = 100;
const int WINDOWWIDTH = 1000;
const int WINDOWHEIGHT = 600;
const double LEARNINGRATE = 0.2;
const double ALPHA = 0.5;
const int DATA_SIZE = INPUTSIZE * INPUTSIZE;
const int INNERLEVER = 16;
const int INNER_NUMBER = 2;
const char* const PATH_MNIST_TRAIN = "include/MNIST_train.txt";
const char* const PATH_MNIST_TEST = "include/MNIST_test.txt";
const char* const PATH_WEIGHTS_TXT = "include/weights.txt";
#endif