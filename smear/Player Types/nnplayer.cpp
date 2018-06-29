//
//  nnplayer.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include "nnplayer.h"

nnplayer::nnplayer(neuralnetwork* _ann) : ann(_ann) {};
void nnplayer::playcard(status &status, bool first, const bool &forgenerating)
{
    vector<int> inputneurons = status.getstatus(hand);
    ann->setinputneurons(inputneurons);
    vector<double> nnoutput = ann->compute();
    int choice = findmaxposition(nnoutput);
    //housekeeping
    hand[choice].whoplayed = whichplayer;
    ++status.nummovestaken;
    if(!forgenerating) cout << "Card Played: " << char(hand[choice].getsuit() - 32) << " " << hand[choice].getvalue() << endl;
    status.trick.push_back(hand[choice]);
    hand.erase(hand.begin() + choice);
}
int nnplayer::findmaxposition(vector<double> outputlayer)
{
    int max = 0;
    for(int i = 0; i < 6; ++i) if(outputlayer[i] >= outputlayer[max]) max = i;
    return max;
}
void nnplayer::printhand()
{
    cout << "Hand: ";
    for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
    cout << endl;
}
