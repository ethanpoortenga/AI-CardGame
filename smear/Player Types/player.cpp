//
//  player.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include <iostream>

#include "player.h"

using namespace std;

bool player::bet(status &status, const int &index, const bool &forgenerating)
{
    if(!forgenerating) printhand();
    if(!forgenerating) cout << "Player " << whichplayer << ": Trainer" << endl;
    vector<int> suits = {0,0,0,0};
    for(auto card: hand)
    {
        if(card.getvalue() == 14) bethelper(card.getsuit(), 10, suits);
        if(card.getvalue() == 13) bethelper(card.getsuit(), 9, suits);
        if(card.getvalue() == 12) bethelper(card.getsuit(), 6, suits);
        if(card.getvalue() == 11) bethelper(card.getsuit(), 9, suits);
        if(card.getvalue() == 10) bethelper(card.getsuit(), 4, suits);
        if(card.getvalue() == 3) bethelper(card.getsuit(), 4, suits);
        if(card.getvalue() == 2) bethelper(card.getsuit(), 7, suits);
        else bethelper(card.getsuit(), 1, suits);
    }
    int maxstrength = 0, maxindex = 0;
    for(int i = 0; i < 3; ++i) if(suits[i] > maxstrength)
    {
        maxstrength = suits[i];
        maxindex = i;
    }
    if(maxstrength > 42 && status.bet < 4)
    {
        status.bet = 4;
        status.whobet = whichplayer;
        settrumpsuit(maxindex,status);
        if(!forgenerating) cout << "Bet 4 with strength " << maxstrength << endl << endl;
        return true;
    }
    if(maxstrength > 32 && status.bet < 3)
    {
        status.bet = 3;
        status.whobet = whichplayer;
        settrumpsuit(maxindex,status);
        if(!forgenerating) cout << "Bet 3 with strength " << maxstrength << endl << endl;
        return true;
    }
    if(maxstrength > 22 && status.bet < 2)
    {
        status.bet = 2;
        status.whobet = whichplayer;
        settrumpsuit(maxindex,status);
        if(!forgenerating) cout << "Bet 2 with strength " << maxstrength << endl << endl;
        return true;
    }
    if(!forgenerating) cout << "Bet 0 with strength " << maxstrength << endl << endl;
    return false;
}

void player::bethelper(int suit, int worth, vector<int> &suits)
{
    int suitindex;
    if(suit == 's') suitindex = 3;
    else if(suit == 'c') suitindex = 2;
    else suitindex = (suit == 'h');
    if(worth == 1) suits[suitindex] += 1;
    else
    {
        for(int i = 0; i < 3; ++i)
        {
            if(i == suitindex) suits[i] += worth;
            else suits[i] += worth*0.5;
        }
    }
}

void player::settrumpsuit(int suit, status &status)
{
    if(suit == 3) status.trumpsuit = 's';
    if(suit == 2) status.trumpsuit = 'c';
    if(suit == 1) status.trumpsuit = 'h';
    if(suit == 0) status.trumpsuit = 'd';
}

void player::forcebet(status &status)
{
    if(!forgenerating) printhand();
    if(!forgenerating) cout << whichplayer << endl;
    if(!forgenerating) cout << "Forced to bet" << endl << endl;
    status.bet = 2;
    status.whobet = whichplayer;
    int whichsuit[4] = {0,0,0,0};
    for(auto card:hand)
    {
        if(card.getvalue() < 4) whichsuit[forcebethelper(card)] += 6;
        else if(card.getvalue() > 13) whichsuit[forcebethelper(card)] += 12;
        else if(card.getvalue() > 11) whichsuit[forcebethelper(card)] += 8;
        else if(card.getvalue() == 11) whichsuit[forcebethelper(card)] += 6;
        else if(card.getvalue() == 10) whichsuit[forcebethelper(card)] += 4;
    }
    int max = 0, index = 0;
    for(int i = 0; i < 4; ++i)
    {
        if(whichsuit[i] > max)
        {
            index = i;
            max = whichsuit[i];
        }
    }
    if(index == 0) status.trumpsuit = 's';
    if(index == 1) status.trumpsuit = 'c';
    if(index == 2) status.trumpsuit = 'h';
    if(index == 3) status.trumpsuit = 'd';
}

int player::forcebethelper(card helper)
{
    if(helper.getsuit() == 's') return 0;
    if(helper.getsuit() == 'c') return 1;
    if(helper.getsuit() == 'h') return 2;
    return 3;
}

void player::sorthand()
{
    sort(hand.begin(), hand.end(), cardcomparator());
}

bool cardcomparator::operator()(card a, card b)
{
    if(a.getsuit() != b.getsuit()) return a.getsuit() < b.getsuit();
    return a.getvalue() < b.getvalue();
}
