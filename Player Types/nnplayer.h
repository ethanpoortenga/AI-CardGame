//
//  nnplayer.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef nnplayer_h
#define nnplayer_h

#include "player.h"
#include "neuralnetwork.h"
#include <stdio.h>

class nnplayer : public player
{
    public:
        nnplayer(neuralnetwork* _ann);
        void playcard(status &status, bool first, const bool &forgenerating);
        int findmaxposition(vector<double> outputlayer);
        void printhand();
    private:
        neuralnetwork* ann;
};

#endif /* nnplayer_h */
