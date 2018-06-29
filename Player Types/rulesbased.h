//
//  rulesbased.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef rulesbased_h
#define rulesbased_h

#include <stdio.h>

#include "player.h"

class rulesbased : public player
{
public:
    void playcard(status &status, bool first, const bool &forgenerating);
    void playcardhelper(pair<bool,card> result, status &status);
    void leadout(status &status);
    pair<bool,card> proirity1(status &status);
    pair<bool,card> proirity2(status &status);
    pair<bool,card> proirity3(status &status);
    pair<bool,card> playworstcard(const char &ledsuit, const char &trumpsuit);
    pair<bool,card> playmostgame(const char &ledsuit, const char &trumpsuit, card winningcard);
    card findlowestcardthatcanbeat(const char &trumpsuit, card &currenthighest);
    void printhand();
    vector<int> getchoice();
private:
    vector<card> playablecards;
    vector<int> lastchoice;
};


#endif /* rulesbased_h */
