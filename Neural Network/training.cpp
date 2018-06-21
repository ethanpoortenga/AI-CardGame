//
//  training.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include <vector>
#include <iostream>

#include "training.h"
#include "status.h"
#include "rulesbased.h"

using namespace std;

generatetrainingdata::generatetrainingdata() : dataoutput("training.txt") {};
void generatetrainingdata::startgenerating()
{
    for(int i = 0; i < 500; ++i)
    {
        deck carddeck;
        carddeck.makedeck();
        status status;
        vector<rulesbased*> players;
        for(int i = 0; i < 4; ++i)
        {
            players.push_back(new rulesbased());
            players[i]->forgenerating = true;
            players[i]->whichplayer = i;
        }
        while(status.overallscore[0] < 11 && status.overallscore[1] < 11)
        {
            //DEAL THE CARDS
            carddeck.shuffle();
            int index = 0, i = 0;
            while(index != 4*6) players[(i++)%4]->hand.push_back(carddeck.carddeck[index++]);
            status.whodealt = (status.whodealt+1) % 4;
            //MAKE THE BETS
            int currentbetter = status.whodealt;
            for(int i = 0; i < 3; ++i)
            {
                players[currentbetter]->bet(status, currentbetter%2, true);
                currentbetter = (currentbetter + 1) % 4;
            }
            //if nobody has bet yet, the dealer is screwed
            if(status.bet == 0) players[currentbetter]->forcebet(status);
            else players[currentbetter]->bet(status,currentbetter, true);
            for(int i = 0; i < 4; ++i) players[i]->sorthand();
            status.wholeads = status.whobet;
            cout << endl;
            //PLAY THE TRICKS
            for(int round = 0; round < 6; ++round)
            {
                int currplayer = status.wholeads;
                logbinarystatus(status.getstatus(players[currplayer]->hand));
                players[currplayer]->playcard(status,true, true);
                logcardchoice(players[currplayer]->getchoice());
                currplayer = (currplayer + 1) % 4;
                logbinarystatus(status.getstatus(players[currplayer]->hand));
                players[(currplayer)%4]->playcard(status,false, true);
                logcardchoice(players[currplayer]->getchoice());
                currplayer = (currplayer + 1) % 4;
                logbinarystatus(status.getstatus(players[currplayer]->hand));
                players[(currplayer)%4]->playcard(status,false, true);
                logcardchoice(players[currplayer]->getchoice());
                currplayer = (currplayer + 1) % 4;
                logbinarystatus(status.getstatus(players[currplayer]->hand));
                players[(currplayer)%4]->playcard(status,false, true);
                logcardchoice(players[currplayer]->getchoice());
                currplayer = (currplayer + 1) % 4;
                status.resettrick();
            }
            status.resethand();
        }
    }
}
void generatetrainingdata::logbinarystatus(vector<int> binarystatus)
{
    for(auto i: binarystatus) dataoutput << i << " ";
}
void generatetrainingdata::logcardchoice(vector<int> cardchoice)
{
    for(auto i: cardchoice) dataoutput << i << " ";
}
