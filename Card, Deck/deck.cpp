//
//  deck.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include "card.h"
#include "deck.h"

#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

void deck::makedeck()
{
    //Don't include 6,7,8 in the deck
    for(int i = 2; i < 6; ++i) carddeck.push_back(card('s',i)); //spades
    for(int i = 9; i < 15; ++i) carddeck.push_back(card('s',i)); //spades
    for(int i = 2; i < 6; ++i) carddeck.push_back(card('c',i)); //clubs
    for(int i = 9; i < 15; ++i) carddeck.push_back(card('c',i)); //clubs
    for(int i = 2; i < 6; ++i) carddeck.push_back(card('h',i)); //hearts
    for(int i = 9; i < 15; ++i) carddeck.push_back(card('h',i)); //hearts
    for(int i = 2; i < 6; ++i) carddeck.push_back(card('d',i)); //diamonds
    for(int i = 9; i < 15; ++i) carddeck.push_back(card('d',i)); //diamonds
    seedrand();
    shuffle();
}

void deck::shuffle()
{
    int max = 39, min = 0;
    for(int i = 0; i < 7; ++i)
    {
        for(int i = 0; i < 38; ++i)
        {
            min = i + 1;
            int random_number = min + (rand() % static_cast<int>(max - min + 1));
            if(i != random_number) swap(carddeck[i], carddeck[random_number]);
        }
        swap(carddeck[0], carddeck[20]);
    }
}

void deck::seedrand()
{
    time_t currenttime;
    struct tm *localtime_;
    time(&currenttime);
    localtime_ = localtime(&currenttime);
    int randseed = localtime_->tm_sec + 61 * localtime_->tm_min + 241 * localtime_->tm_hour;
    srand(randseed);
    seed = randseed;
    cout << "Random Seed: " << randseed << endl << endl;
}
