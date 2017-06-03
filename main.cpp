//
//  main.cpp
//  smear
//
//  Created by Ethan Poortenga on 5/26/17.
//  Copyright © 2017 Ethan Poortenga. All rights reserved.
//

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

struct card
{
    card(int _suit, int _value) : suit(_suit), value(_value) {}
    int getsuit() { return suit; }
    int getvalue() { return value; }
private:
    int suit; //SPADES=3,CLUBS=2,HEARTS=1,DIAMONDS=0
    int value; //2,3,4,5,6,7,8,9,10,11=j,12=q,13=k,14=a
};

struct deck
{
    void makedeck(int numplayers)
    {
        if(numplayers > 4)
        {
            for(int i = 0; i < 4; ++i)
            {
                for(int j = 2; j < 15; ++j)
                {
                    card newcard(i,j);
                    carddeck.push_back(newcard);
                }
            }
            decksize = 52;
        }
        else
        {
            for(int i = 0; i < 4; ++i)
            {
                for(int j = 2; j < 6; ++j)
                {
                    card newcard(i,j);
                    carddeck.push_back(newcard);
                }
                for(int j = 9; j < 15; ++j)
                {
                    card newcard(i,j);
                    carddeck.push_back(newcard);
                }
            }
            decksize = 40;
        }
        shuffle();
    }
    void shuffle()
    {
        int random = 0, numshuffles = 7;
        for(int i = 0; i < numshuffles; ++i)
        {
            for(int i = 0; i < decksize; ++i)
            {
                random = rand();
                random %= decksize;
                if(i != random) swap(carddeck[i],carddeck[random]);
            }
        }
    }
    vector<card> carddeck;
    int decksize;
};

struct gamestatus
{
    //FOR THE GAME OVERALL
    int team1score, team2score, whodealt, numplayers;
    //FOR THE HAND
    int bet, whobet;
    string trumpsuit;
    vector<card> team1cards, team2cards;
    //FOR THE TRICK
    vector<card> trick;
    //FOR RESETTING
    void resettrick()
    {
        
    }
    void resethand()
    {
        
    }
    void resetgame()
    {
        
    }
};

class player
{
public:
    virtual void playcard(gamestatus &gamestatus) = 0;
    virtual bool bet(gamestatus &gamestatus, const int &index) = 0;
    virtual void forcebet(gamestatus &gamestatus, const int &index) = 0;
    vector<card> hand;
};

class trainer : public player
{
    virtual void playcard(gamestatus &gamestatus)
    {
        gamestatus.trick.push_back(hand.back());
        hand.pop_back();
    }
    bool bet(gamestatus &gamestatus, const int &index)
    {
        gamestatus.bet = 2;
        gamestatus.whobet = index;
        return true;
    }
    void forcebet(gamestatus &gamestatus, const int &index)
    {
        gamestatus.bet = 2;
        gamestatus.whobet = index;
    }
};

class learner : public player
{
    virtual void playcard(gamestatus &gamestatus)
    {
        gamestatus.trick.push_back(hand.back());
        hand.pop_back();
    }
    bool bet(gamestatus &gamestatus, const int &index)
    {
        gamestatus.bet = 2;
        gamestatus.whobet = index;
        return true;
    }
    void forcebet(gamestatus &gamestatus, const int &index)
    {
        gamestatus.bet = 2;
        gamestatus.whobet = index;
    }
};

class human : public player
{
    virtual void playcard(gamestatus &gamestatus)
    {
        cout << "";
    }
    bool bet(gamestatus gamestatus, const int &index)
    {
        cout << "How much do you want to bet?" << endl;
        int x;
        cin >> x;
        return true;
    }
    void forcebet(gamestatus gamestatus, const int &index)
    {
        gamestatus.bet = 2;
        gamestatus.whobet = index;
    }
};

void setup(vector<player*> &players, deck &carddeck, gamestatus &gamestatus);
void dealcards(vector<player*> &players, const deck &carddeck);
void makebets(const vector<player*> &players, gamestatus &gamestatus);

int main(int argc, const char * argv[])
{
    deck carddeck;
    vector<player*> players;
    gamestatus gamestatus;
    setup(players, carddeck, gamestatus);
    while(gamestatus.team1score < 11 && gamestatus.team2score < 11)
    {
        dealcards(players, carddeck);
        makebets(players, gamestatus);
        for(int round = 0; round < 6; ++round)
        {
            for(int currplayer = gamestatus.whobet, i = 0; i < 4; ++i, currplayer = (currplayer + 1)%gamestatus.numplayers) players[currplayer]->playcard(gamestatus);
            gamestatus.resettrick();
        }
        gamestatus.resethand();
    }
    if(gamestatus.team1score < 11) cout << "You Lose: Team 2 Wins" << endl;
    else cout << "You win!! Congrats" << endl;
    return 0;
}

void setup(vector<player*> &players, deck &carddeck, gamestatus &gamestatus)
{
    //cout << "How many players do you want (4/6/8)?" << endl;
    //int numplayers;
    //cin >> numplayers;
    gamestatus.numplayers = 4;
    carddeck.makedeck(gamestatus.numplayers);
    players.push_back(new learner());
    for(int i = 0; i < gamestatus.numplayers - 1; ++i) players.push_back(new trainer());
}

void dealcards(vector<player*> &players, const deck &carddeck)
{
    int numplayers = int(players.size()), index = 0, i = 0;
    while(index != numplayers*6) players[(i++)%numplayers]->hand.push_back(carddeck.carddeck[index++]);
}

void makebets(const vector<player*> &players, gamestatus &gamestatus)
{//FIX THIS
    int currentbetter = (gamestatus.whodealt+2) % gamestatus.numplayers;
    gamestatus.whodealt = (gamestatus.whodealt+1) % gamestatus.numplayers;
    for(int dealer = currentbetter - 1; currentbetter != dealer && !players[currentbetter]->bet(gamestatus, currentbetter); currentbetter = (currentbetter + 1) % gamestatus.numplayers);
    if(currentbetter == gamestatus.whodealt) players[currentbetter]->forcebet(gamestatus, currentbetter);
}
