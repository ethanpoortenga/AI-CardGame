//
//  neuralnetwork.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include "neuralnetwork.h"

neuralnetwork::neuralnetwork() : weightinput("random_weights.txt"), weightoutput("output.txt"), datainput("training.txt")
{
    double weight;
    for(int i = 0; i < 80; ++i)
    {
        layer1weights.push_back(vector<double>(120));
        for(int j = 0; j < 120; ++j)
        {
            weightinput >> weight;
            layer1weights[i].push_back(weight);
        }
    }
    for(int i = 0; i < 6; ++i)
    {
        layer2weights.push_back(vector<double>(120));
        for(int j = 0; j < 80; ++j)
        {
            weightinput >> weight;
            layer2weights[i].push_back(weight);
        }
    }
}
neuralnetwork::~neuralnetwork()
{
    for(int i = 0; i < 80; ++i) for(int j = 0; j < 120; ++j) weightoutput << layer1weights[i][j] << " ";
    for(int i = 0; i < 6; ++i) for(int j = 0; j < 80; ++j) weightoutput << layer2weights[i][j] << " ";
    remove("weights.txt");
    rename("output.txt","weights.txt");
}
void neuralnetwork::learn()
{
    int trainingline = 0;
    inputneurons.resize(120);
    actualoutput.resize(6);
    while(trainingline < 10000)
    {
        for(int i = 0; i < 120; ++i) datainput >> inputneurons[i];
        for(int i = 0; i < 6; ++i) datainput >> actualoutput[i];
        compute();
        backpropagate();
        if(trainingline % 100 == 0) cout << trainingline/100 << endl;
        trainingline++;
    }
}
vector<double> neuralnetwork::compute()
{
    outputlayer.resize(6);
    hiddenlayer.resize(80);
    for(int i = 0; i < 80; ++i)
    {
        hiddenlayer[i] = 0;
        for(int j = 0; j < 120; ++j) hiddenlayer[i] += inputneurons[j] * layer1weights[i][j];
    }
    //activation function for hidden layer neurons
    for(int i = 0; i < 80; ++i) hiddenlayer[i] = 1 / (1 + pow(2.71828182846, -1 * hiddenlayer[i]));
    for(int i = 0; i < 6; ++i)
    {
        outputlayer[i] = 0;
        for(int j = 0; j < 80; ++j) outputlayer[i] += hiddenlayer[j] * layer2weights[i][j];
    }
    //output is weights in doubles
    for(int i = 0; i < 6; ++i) outputlayer[i] = 1 / (1 + pow(2.71828182846, -1 * outputlayer[i]));
    return outputlayer;
}
void neuralnetwork::backpropagate()
{
    //compute layer 2
    for(int i = 0; i < 6; i++)
    {
        double constant = 0.05*(actualoutput[i]-outputlayer[i])*outputlayer[i]*(1-outputlayer[i]);
        for(int j = 0; j < 80; ++j) layer2weights[i][j] = constant*hiddenlayer[j];
    }
    //compute layer 1
    for(int i = 0; i < 80; ++i)
    {
        double constant = 0.05;
        for(int j = 0; j < 6; ++j)
        {
            constant *= (actualoutput[j]-outputlayer[j])*(1-outputlayer[j])*(outputlayer[j]);
            layer1weights[i][j] += constant;
        }
    }
}
void neuralnetwork::setinputneurons(vector<int> _inputneurons) { inputneurons = _inputneurons; }
