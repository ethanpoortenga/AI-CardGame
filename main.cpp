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

struct gamecontroller
{
    //FOR THE GAME OVERALL
    int team1score, team2score, whodealt, numplayers;
    //FOR THE HAND
    int bet, whobet;
    string trumpsuit;
    vector<card> team1cardsplayed, team2cardsplayed;
    vector<card> team1cardswon, team2cardswon;
    //FOR THE TRICK
    vector<card> trick;
    int wholeads;
    //FOR RESETTING
    void resettrick()
    {
        if(findtrickwinner()) team1cardswon.insert(team1cardswon.end(),trick.begin(),trick.end());
        else team2cardswon.insert(team2cardswon.end(),trick.begin(), trick.end());
        trick.clear();
    }
    void resethand()
    {
        int team1pointswon = 0, team2pointswon = 0;
        if(findhighestcard()) ++team1pointswon;
        else ++team2pointswon;
        if(findlowestcard()) ++team1pointswon;
        else ++team2pointswon;
        if(findjack()) ++team1pointswon;
        else ++team2pointswon;
        if(findgame()) ++team1pointswon;
        else ++team2pointswon;
        team1cardswon.clear();
        team1cardsplayed.clear();
        team2cardswon.clear();
        team2cardsplayed.clear();
    }
    void resetgame()
    {
        team1score = 0;
        team2score = 0;
    }
    bool findtrickwinner() //return true if team1won
    {
        return true;
    }
    bool findhighestcard() //return true if it's in team1
    {
        return true;
    }
    bool findlowestcard() //return true if it's in team1
    {
        return true;
    }
    bool findjack() //return true if it's in team1
    {
        return true;
    }
    bool findgame() //return true if it's in team1
    {
        return true;
    }
};

class player
{
public:
    virtual void playcard(gamecontroller &gamecontroller) = 0;
    virtual bool bet(gamecontroller &gamecontroller, const int &index) = 0;
    virtual void forcebet(gamecontroller &gamecontroller, const int &index) = 0;
    vector<card> hand;
};

class trainer : public player
{
    virtual void playcard(gamecontroller &gamecontroller)
    {
        gamecontroller.trick.push_back(hand.back());
        hand.pop_back();
    }
    bool bet(gamecontroller &gamecontroller, const int &index)
    {
        gamecontroller.bet = 2;
        gamecontroller.whobet = index;
        return true;
    }
    void forcebet(gamecontroller &gamecontroller, const int &index)
    {
        gamecontroller.bet = 2;
        gamecontroller.whobet = index;
    }
};

class learner : public player
{
    virtual void playcard(gamecontroller &gamecontroller)
    {
        gamecontroller.trick.push_back(hand.back());
        hand.pop_back();
    }
    bool bet(gamecontroller &gamecontroller, const int &index)
    {
        gamecontroller.bet = 2;
        gamecontroller.whobet = index;
        return true;
    }
    void forcebet(gamecontroller &gamecontroller, const int &index)
    {
        gamecontroller.bet = 2;
        gamecontroller.whobet = index;
    }
};

class human : public player
{
    virtual void playcard(gamecontroller &gamecontroller)
    {
        cout << "";
    }
    bool bet(gamecontroller gamecontroller, const int &index)
    {
        cout << "How much do you want to bet?" << endl;
        int x;
        cin >> x;
        return true;
    }
    void forcebet(gamecontroller gamecontroller, const int &index)
    {
        gamecontroller.bet = 2;
        gamecontroller.whobet = index;
    }
};

void setup(vector<player*> &players, deck &carddeck, gamecontroller &gamecontroller);
void dealcards(vector<player*> &players, const deck &carddeck);
void makebets(const vector<player*> &players, gamecontroller &gamecontroller);

int main(int argc, const char * argv[])
{
    deck carddeck;
    vector<player*> players;
    gamecontroller gamecontroller;
    setup(players, carddeck, gamecontroller);
    while(gamecontroller.team1score < 11 && gamecontroller.team2score < 11)
    {
        dealcards(players, carddeck);
        makebets(players, gamecontroller);
        for(int round = 0; round < 6; ++round)
        {
            for(int currplayer = gamecontroller.wholeads, i = 0; i < 4; ++i, currplayer = (currplayer + 1)%gamecontroller.numplayers) players[currplayer]->playcard(gamecontroller);
            gamecontroller.resettrick();
        }
        gamecontroller.resethand();
    }
    if(gamecontroller.team1score < 11) cout << "You Lose: Team 2 Wins" << endl;
    else cout << "You win!! Congrats" << endl;
    return 0;
}

void setup(vector<player*> &players, deck &carddeck, gamecontroller &gamecontroller)
{
    //cout << "How many players do you want (4/6/8)?" << endl;
    //int numplayers;
    //cin >> numplayers;
    gamecontroller.numplayers = 4;
    carddeck.makedeck(gamecontroller.numplayers);
    players.push_back(new learner());
    for(int i = 0; i < gamecontroller.numplayers - 1; ++i) players.push_back(new trainer());
}

void dealcards(vector<player*> &players, const deck &carddeck)
{
    int numplayers = int(players.size()), index = 0, i = 0;
    while(index != numplayers*6) players[(i++)%numplayers]->hand.push_back(carddeck.carddeck[index++]);
}

void makebets(const vector<player*> &players, gamecontroller &gamecontroller)
{//FIX THIS
    int currentbetter = (gamecontroller.whodealt+2) % gamecontroller.numplayers;
    gamecontroller.whodealt = (gamecontroller.whodealt+1) % gamecontroller.numplayers;
    for(int dealer = currentbetter - 1; currentbetter != dealer && !players[currentbetter]->bet(gamecontroller, currentbetter); currentbetter = (currentbetter + 1) % gamecontroller.numplayers);
    if(currentbetter == gamecontroller.whodealt) players[currentbetter]->forcebet(gamecontroller, currentbetter);
}
