//
//  deck.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef deck_h
#define deck_h

#include <stdio.h>

#include "card.h"

#include <vector>

using namespace std;

struct deck
{
    void makedeck();
    void shuffle();
    void seedrand();
    vector<card> carddeck;
    int seed;
};

#endif /* deck_h */
