//
//  human.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include <iostream>

#include "human.h"

void human::playcard(status &status, bool first, const bool &forgenerating)
{
    cout << "Player " << whichplayer << ": You" << endl;
    printhand();
    int index = -1;
    cout << "which card do you want to play?" << endl;
    cin >> index;
    hand[index].whoplayed = whichplayer;
    if(first) status.ledsuit = hand[index].getsuit();
    status.trick.push_back(hand[index]);
    if(index!=hand.size() - 1) swap(*(hand.begin() + index),hand.back());
    hand.pop_back();
}
bool human::bet(status &status, const int &index, const bool &forgenerating)
{
    printhand();
    cout << "Player " << whichplayer << ": Human" << endl;
    cout << "How much do you want to bet? put -1 for no" << endl;
    int x;
    cin >> x;
    if(x == -1) return false;
    status.whobet = whichplayer;
    status.bet = x;
    cout << "which suit" << endl;
    char suit;
    cin >> suit;
    status.trumpsuit = suit;
    return true;
}
void human::forcebet(status &status)
{
    status.bet = 2;
    status.whobet = whichplayer;
}
void human::printhand()
{
    cout << "Hand: ";
    for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
    cout << endl;
}
