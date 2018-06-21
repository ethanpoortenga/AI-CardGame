//
//  card.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/3/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include "card.h"

using namespace std;


card::card(int _suit, int _value) : suit(_suit), value(_value) {}

int card::getsuit() { return suit; }

int card::getvalue() { return value; }

int card::getgame()
{
    if(value < 10) return 0;
    if(value > 10) return value - 10;
    return 10;
}

int card::gethierarchy(const char &trumpsuit)
{
    if(suit == trumpsuit) return (value + 14);
    return value;
}

bool card::operator==(const card &other)
{
    return (other.suit == suit && other.value == value);
}
