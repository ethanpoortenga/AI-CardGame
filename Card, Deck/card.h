//
//  card.h
//  smear
//
//  Created by Ethan Poortenga on 3/3/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef card_h
#define card_h

#include <stdio.h>

struct card
{
    card(int _suit, int _value);
    int getsuit();
    int getvalue();
    int getgame();
    int gethierarchy(const char &trumpsuit);
    bool operator==(const card &other);
    int whoplayed;
    int whotookin;
private:
    int suit; //SPADES='s',CLUBS='c',HEARTS='h',DIAMONDS='d'
    int value; //2,3,4,5,6,7,8,9,10,11=j,12=q,13=k,14=a
};


#endif /* card_h */
