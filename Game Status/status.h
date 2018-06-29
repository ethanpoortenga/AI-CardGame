//
//  status.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef status_h
#define status_h

#include "deck.h"

#include <stdio.h>
#include <vector>

struct status
{
    //FOR THE LEARNER
    vector<int> gamestatus;
    //FOR THE GAME OVERALL
    int overallscore[2] = {0,0};
    int whodealt = -1;
    //FOR THE HAND
    int bet = 0, whobet = 0;
    char trumpsuit = ' ';
    vector<card> cardsplayed;
    bool jackcollected = false;
    int points[2] = {0,0};
    vector<int> jackqueenkingace = {0,0,0,0};
    //FOR THE TRICK
    vector<card> trick;
    int wholeads = -1, nummovestaken = 0;
    char ledsuit = ' ';
    //FOR RESETTING
    void resettrick();
    void resethand();
    void resetgame();
    card findtrickwinner();
    int findhighestcard();
    int findlowestcard();
    int findjack();
    vector<int> getstatus(vector<card> hand);
};

#endif /* status_h */
