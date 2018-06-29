//
//  main.cpp
//  smear
//
//  Created by Ethan Poortenga on 5/26/17.
//  Copyright © 2017 Ethan Poortenga. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <cmath>

#include "deck.h"
#include "status.h"
#include "neuralnetwork.h"
#include "training.h"
#include "player.h"
#include "rulesbased.h"
#include "nnplayer.h"
#include "human.h"

using namespace std;

void setup(vector<player*> &players, deck &carddeck, status &status);
void dealcards(vector<player*> &players, const deck &carddeck, status &status);
void makebets(const vector<player*> &players, status &status);
void playtrick(const vector<player*> &players, status &status,const bool &forgenerating);

int main(int argc, const char * argv[])
{
    cout << "Do you want to generate data(1), train the nn on data(2), or play against the neural network(3)?" << endl;
    int answer;
    cin >> answer;
    if(answer == 1)
    {
        generatetrainingdata generate;
        generate.startgenerating();
    }
    else if(answer == 2)
    {
        neuralnetwork nn;
        nn.learn();
    }
    else
    {
        for(int i = 0; i < 3; ++i)
        {
            deck carddeck;
            vector<player*> players;
            status status;
            setup(players, carddeck, status);
            while(status.overallscore[0] < 11 && status.overallscore[1] < 11)
            {
                carddeck.shuffle();
                dealcards(players, carddeck, status);
                makebets(players, status);
                for(int round = 0; round < 6; ++round) playtrick(players, status, false);
                status.resethand();
            }
            if(status.overallscore[0] < 11) cout << "You Lose: Team 2 Wins" << endl;
            else cout << "You win!! Congrats" << endl;
        }
    }
    return 0;
}

void setup(vector<player*> &players, deck &carddeck, status &status)
{
    carddeck.makedeck();
    for(int i = 0; i < 4; ++i)
    {
        cout << endl << "Do you want the player to be rules based(1), neural network(2), or human(3)?" << endl;
        int answer;
        cin >> answer;
        if(answer == 1) players.push_back(new rulesbased());
        else if (answer == 2)
        {
            neuralnetwork nn;
            players.push_back(new nnplayer(&nn));
        }
        else players.push_back(new human());
        players[i]->whichplayer = i;
    }
}

void dealcards(vector<player*> &players, const deck &carddeck, status &status)
{
    int numplayers = int(players.size()), index = 0, i = 0;
    while(index != numplayers*6) players[(i++)%numplayers]->hand.push_back(carddeck.carddeck[index++]);
    status.whodealt = (status.whodealt+1) % 4;
}

void makebets(const vector<player*> &players, status &status)
{
    int currentbetter = status.whodealt;
    for(int i = 0; i < 3; ++i)
    {
        players[currentbetter]->bet(status, currentbetter%2, true);
        currentbetter = (currentbetter + 1) % 4;
    }
    //if nobody has bet yet, the dealer is screwed
    if(status.bet == 0) players[currentbetter]->forcebet(status);
    else players[currentbetter]->bet(status,currentbetter, true);
    status.wholeads = status.whobet;
    cout << endl;
}

void playtrick(const vector<player*> &players, status &status,const bool &forgenerating)
{
    int currplayer = status.wholeads;
    //COME BACK, currently have all generating
    players[currplayer]->playcard(status,true, true);
    cout << endl;
    players[(currplayer+1)%4]->playcard(status,false, true);
    cout << endl;
    players[(currplayer+2)%4]->playcard(status,false, true);
    cout << endl;
    players[(currplayer+3)%4]->playcard(status,false, true);
    cout << endl;
    status.resettrick();
    cout << endl << endl;
    cout << flush;
}
