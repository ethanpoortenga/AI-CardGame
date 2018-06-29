//
//  player.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef player_h
#define player_h

#include <stdio.h>

#include "deck.h"
#include "status.h"

class player
{
public:
    virtual bool bet(status &status, const int &index, const bool &forgenerating);
    virtual void bethelper(int suit, int worth, vector<int> &suits);
    virtual void settrumpsuit(int suit, status &status);
    virtual void forcebet(status &status);
    virtual int forcebethelper(card helper);
    virtual void sorthand();
    virtual void printhand() = 0;
    virtual void playcard(status &status, bool first, const bool &forgenerating) = 0;
    vector<int> getchoice();
    vector<card> hand;
    bool forgenerating = false;
    int whichplayer = 0;
};

struct cardcomparator
{
    bool operator()(card a, card b);
};

#endif /* player_h */
