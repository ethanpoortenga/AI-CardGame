//
//  neuralnetwork.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef neuralnetwork_h
#define neuralnetwork_h

#include <stdio.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

class neuralnetwork
{
public:
    neuralnetwork();
    void learn();
    vector<double> compute();
    void backpropagate();
    void setinputneurons(vector<int> _inputneurons);
    ~neuralnetwork();
private:
    vector<vector<double>> layer1weights; //120 rows, 80 columns
    vector<vector<double>> layer2weights; //80 rows, 6 columns
    vector<int> inputneurons;//first 40 = hand, second 40 = trick, third 40 = collected;
    vector<double> hiddenlayer1; //80 neurons
    vector<double> hiddenlayer2; //80 neurons
    vector<double> outputlayer; //6 neurons
    vector<int> actualoutput;
    double learningrate = 0.05;
    ifstream datainput;
    ifstream weightinput;
    ofstream weightoutput;
};

#endif /* neuralnetwork_h */
