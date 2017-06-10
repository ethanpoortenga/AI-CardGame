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
    int whoplayed;
    int whotookin;
private:
    int suit; //SPADES='s',CLUBS='c',HEARTS='h',DIAMONDS='d'
    int value; //2,3,4,5,6,7,8,9,10,11=j,12=q,13=k,14=a
};

struct deck
{
    void makedeck(int numplayers)
    {
        if(numplayers > 4)
        {
            for(int i = 2; i < 15; ++i) carddeck.push_back(card('s',i)); //spades
            for(int i = 2; i < 15; ++i) carddeck.push_back(card('c',i)); //clubs
            for(int i = 2; i < 15; ++i) carddeck.push_back(card('h',i)); //hearts
            for(int i = 2; i < 15; ++i) carddeck.push_back(card('d',i)); //diamonds
            decksize = 52;
        }
        else
        {
            for(int i = 2; i < 6; ++i) carddeck.push_back(card('s',i)); //spades
            for(int i = 9; i < 15; ++i) carddeck.push_back(card('s',i)); //spades
            for(int i = 2; i < 6; ++i) carddeck.push_back(card('c',i)); //clubs
            for(int i = 9; i < 15; ++i) carddeck.push_back(card('c',i)); //clubs
            for(int i = 2; i < 6; ++i) carddeck.push_back(card('h',i)); //hearts
            for(int i = 9; i < 15; ++i) carddeck.push_back(card('h',i)); //hearts
            for(int i = 2; i < 6; ++i) carddeck.push_back(card('d',i)); //diamonds
            for(int i = 9; i < 15; ++i) carddeck.push_back(card('d',i)); //diamonds
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
    int team1score = 0, team2score = 0, whodealt = 0, numplayers = 0;
    //FOR THE HAND
    int bet = 0, whobet = 0;
    char trumpsuit = ' ';
    vector<card> cardsplayed;
    bool jackcollected = false;
    //FOR THE TRICK
    vector<card> trick;
    int wholeads, nummovestaken = 0;
    char ledsuit = ' ';
    //FOR RESETTING
    void resettrick()
    {
        if(findtrickwinner()) for(auto card:trick) card.whotookin = 1;
        else for(auto card:trick) card.whotookin = 1;
        for(auto card:trick) cardsplayed.push_back(card);
        trick.clear();
    }
    void resethand()
    {
        int team1pointswon = 0, team2pointswon = 0;
        if(findhighestcard()) ++team1pointswon;
        else ++team2pointswon;
        if(findlowestcard()) ++team1pointswon;
        else ++team2pointswon;
        if(findjack() == 1) ++team1pointswon;
        else if(findjack() == 2) ++team2pointswon;
        if(findgame() == 0) ++team1pointswon;
        else if(findgame() == 1) ++team2pointswon;
        if(whobet%numplayers == 0)
        {
            if(team1pointswon >= bet)
            {
                team1score += team1pointswon;
                team2score += team2pointswon;
            }
            else team1score -= bet;
        }
        else
        {
            if(team2pointswon >= bet)
            {
                team1score += team1pointswon;
                team2score += team2pointswon;
            }
            else team2score -= bet;
        }
        //A LOT TO UPDATE HERE
        cardsplayed.clear();
        jackcollected = false;
    }
    void resetgame()
    {
        team1score = 0;
        team2score = 0;
    }
    int findtrickwinner() //return true if team1won
    {
        card currenthighest(0,0);
        for(int i = 0; i < numplayers; ++i) if(trick[i].getsuit() == trumpsuit && trick[i].getvalue() > currenthighest.getvalue())currenthighest = trick[i];
        if(currenthighest.getvalue() == 0) for(int i = 0; i < numplayers; ++i) if(trick[i].getsuit() == ledsuit && trick[i].getvalue() > currenthighest.getvalue())currenthighest = trick[i]; //notrump
        return (currenthighest.whoplayed == 1);
    }
    bool findhighestcard() //return true if it's in team1
    {
        card currenthighest(0,0);
        for(auto card : cardsplayed) if(card.getsuit() == trumpsuit && card.getvalue() > currenthighest.getvalue()) currenthighest = card;
        return (currenthighest.whoplayed == 1);
    }
    bool findlowestcard() //return true if it's in team1
    {
        card currentlowest(0,15);
        for(auto card : cardsplayed) if(card.getsuit() == trumpsuit && card.getvalue() < currentlowest.getvalue()) currentlowest = card;
        return (currentlowest.whoplayed == 1);
    }
    int findjack() //return 0 = team1, 1 = team2, -1 = no jack
    {
        for(auto card:cardsplayed) if(card.getvalue() == 11 && card.getsuit() == trumpsuit) return (card.whotookin);
        return -1;
    }
    int findgame() //return 0 = team1, 1 = team2, -1 = tie
    {
        int points[2] = {0,0};
        for(auto card : cardsplayed)
        {
            if(card.getvalue() > 10) points[card.whotookin] += card.getvalue() - 10;
            else if(card.getvalue() == 10) points[card.whotookin] += 10;
        }
        if(points[0] > points[1]) return 0;
        if(points[1] > points[0]) return 1;
        return -1;
    }
};

class player
{
public:
    virtual void playcard(gamecontroller &gamecontroller) = 0;
    virtual bool bet(gamecontroller &gamecontroller, const int &index) = 0;
    virtual void forcebet(gamecontroller &gamecontroller, const int &index) = 0;
    vector<card> hand;
    int whichteam = 0;
};

class trainer : public player
{
    virtual void playcard(gamecontroller &gamecontroller)
    {
        hand.back().whoplayed = whichteam;
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
    pair<bool,card> proirity1(gamecontroller &gamecontroller) //jack
    {
        //check to see whether the jack has already been taken in
        if(gamecontroller.jackcollected) return pair<bool,card>(false,card(0,0));
        //check to see whether able to take in the jack
        bool cantakeinjack = false;
        for(auto card:hand) if(card.getsuit() == gamecontroller.trumpsuit && card.getvalue() > 11) cantakeinjack = true;
        if(!cantakeinjack) return pair<bool,card>(false,card(0,0));
        //check whether the jack is out
        bool jackisout = false;
        for(auto card:gamecontroller.trick) if (card.getsuit() == gamecontroller.trumpsuit && card.getvalue() == 11) jackisout = true;
        if(jackisout)
        //check to see whether others could possibly take in the jack
        int acekingqueen = 0;
        for(auto card:gamecontroller.cardsplayed) if(card.getvalue() > 11 && card.getsuit() == gamecontroller.trumpsuit) ++acekingqueen;
        if(acekingqueen == 3)
        {
            //check whether my team already has it
        }
    }
    pair<bool,card> proirity2(gamecontroller &gamecontroller) //game
    {
        
    }
    pair<bool,card> proirity3(gamecontroller &gamecontroller) //taking in the trick
    {
        
    }
};

class learner : public player
{
    virtual void playcard(gamecontroller &gamecontroller)
    {
        hand.back().whoplayed = whichteam;
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
    //some way of determining the strength of a given card in the context of the trick/hand
        //score as a function of adding up weights determined by the ML multiplied by certain occurances/situations in the hand/trick
    //some way of measuring the accuracy of the strength estimate
        //whether the jack was taken in and whether it could have been
        //how much game was taken in relative to what could have been
        //whether the game was won (if it could have been won)
    //some way of adjusting strength estimate based on accuracy results
        //keep track of the choices per trick and hand and raise/lower the weights based on success/failure taking margainality into account in situations like game
    //methodology
        //have different weights for different types of cards
            //trump:ace,king,queen,jack,ten,9-2 and non-trump A-j,10,9-2
            //check back bc may be able to use the same function for every card
        //compute a card effectiveness score and choose the card with the highest score
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
        for(int round = 0; round < 6; ++round) playtrick(players,gamecontroller);
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
    players[0]->whichteam = 1;
    for(int i = 1; i < gamecontroller.numplayers; ++i)
    {
        players.push_back(new trainer());
        players[i]->whichteam = i % 2 + 1;
    }
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

void playtrick(const vector<player*> &players, gamecontroller &gamecontroller)
{
    int endplayer = (gamecontroller.wholeads + gamecontroller.numplayers - 1) % gamecontroller.numplayers;
    for(int currplayer = gamecontroller.wholeads; currplayer != endplayer; currplayer = (currplayer + 1) % gamecontroller.numplayers) players[currplayer]->playcard(gamecontroller);
    gamecontroller.resettrick();
}
