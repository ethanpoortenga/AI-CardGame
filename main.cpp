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
    int getgame()
    {
        if(value < 10) return 0;
        if(value > 10) return value - 10;
        return 10;
    }
    int whoplayed;
    int whotookin;
private:
    int suit; //SPADES='s',CLUBS='c',HEARTS='h',DIAMONDS='d'
    int value; //2,3,4,5,6,7,8,9,10,11=j,12=q,13=k,14=a
};

struct deck
{
    void makedeck()
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
        shuffle();
    }
    void shuffle()
    {
        int random = 0, numshuffles = 7;
        for(int i = 0; i < numshuffles; ++i)
        {
            for(int i = 0; i < decksize; ++i)
            {
                srand((unsigned)time(NULL));
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
    int team1score = 0, team2score = 0, whodealt = -1;
    //FOR THE HAND
    int bet = 0, whobet = 0;
    char trumpsuit = ' ';
    vector<card> cardsplayed;
    bool jackcollected = false;
    int points[2] = {0,0};
    bool jackqueenkingace[4] = {false,false,false,false};
    //FOR THE TRICK
    vector<card> trick;
    int wholeads, nummovestaken = 0;
    char ledsuit = ' ';
    //FOR RESETTING
    void resettrick()
    {
        int whowon = findtrickwinner();
        if(whowon%2 == 0)
        {
            for(auto card:trick) card.whotookin = wholeads;
        }
        else
        {
            for(auto card:trick) card.whotookin = (wholeads+1)%2;
        }
        for(auto card:trick) cardsplayed.push_back(card);
        for(auto card:trick)
        {
            if(card.getvalue() > 10) points[card.whotookin] += card.getvalue() - 10;
            else if(card.getvalue() == 10) points[card.whotookin] += 10;
            if(card.getsuit() == trumpsuit && card.getvalue() >10) jackqueenkingace[card.getvalue()-11]=true;
        }
        cout << "player " << whowon << " won the trick" << endl;
        wholeads = whowon;
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
        if(whobet%2 == 0)
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
        for(int i = 0; i < 4; ++i) jackqueenkingace[i] = false;
    }
    void resetgame()
    {
        team1score = 0;
        team2score = 0;
    }
    int findtrickwinner() //return true if the team who led won
    {
        card currenthighest(0,0);
        for(int i = 0; i < 4; ++i) if(trick[i].getsuit() == trumpsuit && trick[i].getvalue() > currenthighest.getvalue())currenthighest = trick[i];
        if(currenthighest.getvalue() == 0) for(int i = 0; i < 4; ++i) if(trick[i].getsuit() == ledsuit && trick[i].getvalue() > currenthighest.getvalue())currenthighest = trick[i]; //notrump
        return currenthighest.whoplayed;
    }
    bool findhighestcard() //return true if it's in team1
    {
        card currenthighest(0,0);
        for(auto card : cardsplayed) if(card.getsuit() == trumpsuit && card.getvalue() > currenthighest.getvalue()) currenthighest = card;
        return (currenthighest.whoplayed%2 == 1);
    }
    bool findlowestcard() //return true if it's in team1
    {
        card currentlowest(0,15);
        for(auto card : cardsplayed) if(card.getsuit() == trumpsuit && card.getvalue() < currentlowest.getvalue()) currentlowest = card;
        return (currentlowest.whoplayed%2 == 1);
    }
    int findjack() //return 0 = team1, 1 = team2, -1 = no jack
    {
        for(auto card:cardsplayed) if(card.getvalue() == 11 && card.getsuit() == trumpsuit) return (card.whotookin);
        return -1;
    }
    int findgame() //return 0 = team1, 1 = team2, -1 = tie
    {
        if(points[0] > points[1]) return 0;
        if(points[1] > points[0]) return 1;
        return -1;
    }
};

class player
{
public:
    virtual void playcard(gamecontroller &gamecontroller, bool first) = 0;
    virtual bool bet(gamecontroller &gamecontroller, const int &index) = 0;
    virtual void forcebet(gamecontroller &gamecontroller, const int &index) = 0;
    virtual void printhand() = 0;
    vector<card> hand;
    int whichplayer = 0;
};

class trainer : public player
{
    //maybe incorporate knowing whether the other players have trump
    void playcard(gamecontroller &gamecontroller, bool first)
    {
        printhand();
        cout << whichplayer << " trainer" << endl;
        card cardtoplay(0,0);
        pair<bool,card> result(false,card(0,0));
        result = proirity1(gamecontroller);
        if(result.first == true)
        {
            playcardhelper(result, gamecontroller);
            cout << char(result.second.getsuit()) << " " << result.second.getvalue() << endl;
            if(first) gamecontroller.ledsuit = result.second.getsuit();
            return;
        }
        result = proirity2(gamecontroller);
        if(result.first == true)
        {
            playcardhelper(result, gamecontroller);
            cout << char(result.second.getsuit()) << " " << result.second.getvalue() << endl;
            if(first) gamecontroller.ledsuit = result.second.getsuit();
            return;
        }
        result = proirity3(gamecontroller);
        playcardhelper(result, gamecontroller);
        cout << char(result.second.getsuit()) << " " << result.second.getvalue() << endl;
        if(first) gamecontroller.ledsuit = result.second.getsuit();
        return;
    }
    void playcardhelper(pair<bool,card> &result, gamecontroller &gamecontroller)
    {
        card cardtoplay(0,0);
        cardtoplay = result.second;
        cardtoplay.whoplayed = whichplayer;
        gamecontroller.trick.push_back(cardtoplay);
        for(int i = 0; i < hand.size(); ++i)
        {
            if(hand[i].getsuit() == cardtoplay.getsuit() && hand[i].getvalue() == cardtoplay.getvalue())
            {
                if(i!=hand.size() - 1) swap(*(hand.begin()+i),hand.back());
                hand.pop_back();
                break;
            }
        }
    }
    bool bet(gamecontroller &gamecontroller, const int &index)
    {
        printhand();
        cout << whichplayer << endl;
        int bet = 0;
        cout << "bet? put -1 for no" << endl;
        cin >> bet;
        if(bet == -1) return false;
        gamecontroller.bet = bet;
        gamecontroller.whobet = index;
        cout << "which suit" << endl;
        char suit;
        cin >> suit;
        gamecontroller.trumpsuit = suit;
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
        bool havejack = false;
        card cardtotakeinjack(0,0);
        int value = 0;
        for(auto card:hand)
        {
            if(card.getsuit() == gamecontroller.trumpsuit && card.getvalue() > 11)
            {
                cantakeinjack = true;
                value = card.getvalue();
                cardtotakeinjack = card;
            }
            else if(card.getsuit() == gamecontroller.trumpsuit && card.getvalue() == 11)
            {
                cantakeinjack = true;
                value = 11;
                cardtotakeinjack = card;
                havejack = true;
            }
        }
        if(!cantakeinjack) return pair<bool,card>(false,card(0,0));
        //check whether the jack is out in the trick
        bool jackisout = false;
        int teamplayedjack = -1;
        //DONT DO THIS IF YOU ALREADY HAVE THE JACK
        for(auto card:gamecontroller.trick)
        {
            if (card.getsuit() == gamecontroller.trumpsuit && card.getvalue() == 11)
            {
                jackisout = true;
                teamplayedjack = card.whoplayed;
            }
        }
        bool otherscouldtakein = false;
        //check to see whether others could possibly take in the jack
        if(gamecontroller.nummovestaken != 3) for(int i = value + 1; i < 15; ++i) if(!gamecontroller.jackqueenkingace[i-11])otherscouldtakein=true;
        if(!otherscouldtakein)
        {
            //check whether my team is already taking it
            if(havejack) return pair<bool,card>(true,cardtotakeinjack);
            else if(jackisout && teamplayedjack == whichplayer%2) return playmostgame(gamecontroller.ledsuit, gamecontroller.trumpsuit);
            else if(jackisout) return pair<bool,card>(true,cardtotakeinjack);
        }
        return pair<bool,card>(false,card(0,0));
    }
    pair<bool,card> proirity2(gamecontroller &gamecontroller) //game
    {
        //check whether game has already been won
        if(gamecontroller.points[0] > 40 || gamecontroller.points[1] > 40) return pair<bool,card>(false,card(0,0));
        //check to see whether you can take in a ten
        if(gamecontroller.nummovestaken == 3)
        {
            bool tenledsuit = false;
            bool tentrumpsuit = false;
            for(auto card: hand)
            {
                if(card.getvalue() == 10 && card.getsuit() == gamecontroller.ledsuit) tenledsuit = true;
                if(card.getvalue() == 10 && card.getsuit() == gamecontroller.trumpsuit) tentrumpsuit = true;
            }
            //check whether the other person would take in the card
            bool otherwouldtakeinledsuit = false;
            bool otherwouldtakeintrumpsuit = false;
            if(tenledsuit)
            {
                for(auto card:gamecontroller.trick) if(card.getvalue() > 10 && card.getsuit() == gamecontroller.ledsuit && card.whoplayed == (whichplayer + 1) % 2) otherwouldtakeinledsuit = true;
            }
            for(auto card : gamecontroller.trick) if(card.getvalue() > 10 && card.getsuit() == gamecontroller.trumpsuit) otherwouldtakeintrumpsuit = true;
            if(!otherwouldtakeinledsuit && !otherwouldtakeintrumpsuit)
            {
                card cardtoplay(0,0);
                for(auto card:hand) if(card.getvalue() == 10 && card.getsuit() == gamecontroller.ledsuit) cardtoplay = card;
                return pair<bool,card>(true,cardtoplay);
            }
            else if(!otherwouldtakeintrumpsuit)
            {
                card cardtoplay(0,0);
                for(auto card:hand) if(card.getvalue() == 10 && card.getsuit() == gamecontroller.trumpsuit) cardtoplay = card;
                return pair<bool,card>(true,cardtoplay);
            }
        }
        //check whether there's substantial game out
        int gameintrick = 0;
        for(auto card : gamecontroller.trick)
        {
            if(card.getvalue() > 10) gameintrick += card.getvalue() - 10;
            else if(card.getvalue() == 10) gameintrick += 10;
        }
        //yes substantial game (Greater than 7 or will win game point)
        if(gameintrick > 7 || (40 - gamecontroller.points[whichplayer%2]) <= gameintrick)
        {
            //work on taking in the game
            //check to see whether partner is already winning
            card currenthighest(0,0);
            bool myteamwinning = false;
            for(auto card:gamecontroller.trick)
            {
                if(card.getvalue() > currenthighest.getvalue())
                {
                    if (card.getsuit() == gamecontroller.trumpsuit)
                    {
                        currenthighest = card;
                        if(card.whoplayed%2 == whichplayer%2) myteamwinning = true;
                        else myteamwinning = false;
                    }
                    else if (card.getsuit() == gamecontroller.ledsuit)
                    {
                        if(currenthighest.getsuit() != gamecontroller.trumpsuit)
                        {
                            currenthighest = card;
                            if(card.whoplayed%2 == whichplayer%2) myteamwinning = true;
                            else myteamwinning = false;
                        }
                    }
                }
            }
            //my team winning
            if(myteamwinning)
            {
                if(gamecontroller.ledsuit != gamecontroller.trumpsuit) return playmostgame(gamecontroller.ledsuit, gamecontroller.trumpsuit);
                return playworstcard(gamecontroller.ledsuit, gamecontroller.trumpsuit);
            }
            //other team winning
            {
                if(gamecontroller.nummovestaken != 3)
                {
                    card cardtoplay = highestcardsamesuit(gamecontroller, currenthighest);
                    if(cardtoplay.getsuit() != 0) return pair<bool,card>(true,cardtoplay);
                    else return playworstcard(gamecontroller.ledsuit, gamecontroller.trumpsuit);
                }
                //find the lowest card I have that can beat them
                card lowestcardthatcanbeat = findlowestcardthatcanbeat(gamecontroller, currenthighest);
                //if the current highest is trumpsuit
                if(lowestcardthatcanbeat.getsuit() != 0) return pair<bool,card>(true,lowestcardthatcanbeat);
                else return playworstcard(gamecontroller.ledsuit, gamecontroller.trumpsuit);
            }
        }
        //no substantial game
        else return pair<bool,card>(false,card(0,0));
    }
    pair<bool,card> proirity3(gamecontroller &gamecontroller) //taking in the trick
    {
        //check to see who's winning
        card currenthighest(0,0);
        bool myteamwinning = false;
        for(auto card:gamecontroller.trick)
        {
            if(card.getvalue() > currenthighest.getvalue())
            {
                if (card.getsuit() == gamecontroller.trumpsuit)
                {
                    currenthighest = card;
                    if(card.whoplayed%2 == whichplayer%2) myteamwinning = true;
                    else myteamwinning = false;
                }
                else if (card.getsuit() == gamecontroller.ledsuit)
                {
                    if(currenthighest.getsuit() != gamecontroller.trumpsuit)
                    {
                        currenthighest = card;
                        if(card.whoplayed%2 == whichplayer%2) myteamwinning = true;
                        else myteamwinning = false;
                    }
                }
            }
        }
        if(myteamwinning)
        {
            if(gamecontroller.ledsuit != gamecontroller.trumpsuit) return playmostgame(gamecontroller.ledsuit, gamecontroller.trumpsuit);
            return playworstcard(gamecontroller.ledsuit, gamecontroller.trumpsuit);
        }
        //see whether I can win
        else
        {
            if(gamecontroller.nummovestaken != 3)
            {
                card cardtoplay = highestcardsamesuit(gamecontroller, currenthighest);
                if(cardtoplay.getsuit() != 0) return pair<bool,card>(true,cardtoplay);
                else return playworstcard(gamecontroller.ledsuit, gamecontroller.trumpsuit);
            }
            card lowestcardthatcanbeat = findlowestcardthatcanbeat(gamecontroller, currenthighest);
            if(lowestcardthatcanbeat.getsuit() != 0) return pair<bool,card>(true,lowestcardthatcanbeat);
            else return playworstcard(gamecontroller.ledsuit, gamecontroller.trumpsuit);
        }
    }
    pair<bool,card> playworstcard(const char &ledsuit, const char &trumpsuit)
    {
        pair<bool,card> cardtoplay(true,card(0,0));
        card currentworstcard(0,15);
        for(auto card:hand)
        {
            if(card.getsuit() == ledsuit && currentworstcard.getsuit() != ledsuit) currentworstcard = card;
            else if (card.getsuit() == ledsuit && currentworstcard.getsuit() == ledsuit && card.getgame() < currentworstcard.getgame()) currentworstcard = card;
            else if (card.getgame() < currentworstcard.getgame() && currentworstcard.getsuit() != ledsuit) currentworstcard = card;
        }
        if(currentworstcard.getsuit() == 0) cardtoplay.second = hand[0];
        else cardtoplay.second = currentworstcard;
        return cardtoplay;
    }
    pair<bool,card> playmostgame(const char &ledsuit, const char &trumpsuit)
    {
        pair<bool,card> cardtoplay(true,card(0,0));
        card currentbestcard(0,0);
        for(auto card:hand)
        {
            if(card.getsuit() == ledsuit && currentbestcard.getsuit() != ledsuit) currentbestcard = card;
            else if (card.getsuit() == ledsuit && currentbestcard.getsuit() == ledsuit && card.getgame() > currentbestcard.getgame()) currentbestcard = card;
            else if (card.getgame() > currentbestcard.getgame() && currentbestcard.getsuit() != ledsuit) currentbestcard = card;
        }
        cardtoplay.second = currentbestcard;
        return cardtoplay;
    }
    card highestcardsamesuit(gamecontroller &gamecontroller, card &currenthighest)
    {
        card cardtoplay(0,0);
        for(auto card:hand)
        {
            if(card.getsuit() == gamecontroller.ledsuit && card.getvalue() > currenthighest.getvalue() && card.getvalue() > cardtoplay.getvalue()) cardtoplay = card;
        }
        return cardtoplay;
    }
    card findlowestcardthatcanbeat(gamecontroller &gamecontroller, card &currenthighest)
    {
        card lowestcardthatcanbeat(0,15);
        if(currenthighest.getsuit() == gamecontroller.trumpsuit)
        {
            for(auto card: hand) if(card.getsuit() == gamecontroller.trumpsuit && card.getvalue() > currenthighest.getvalue() && card.getvalue() < lowestcardthatcanbeat.getvalue()) lowestcardthatcanbeat = card;
        }
        //if the current highest is not trump suit
        else
        {
            //first pass looking for ledsuit
            for(auto card:hand)
            {
                if(card.getsuit() == gamecontroller.ledsuit && card.getvalue() > currenthighest.getvalue() && card.getvalue() < lowestcardthatcanbeat.getvalue()) lowestcardthatcanbeat = card;
            }
            if(lowestcardthatcanbeat.getvalue() != 15) return lowestcardthatcanbeat;
            //second pass looking for trumpsuit
            for(auto card:hand)
            {
                if(card.getsuit() == gamecontroller.trumpsuit && card.getvalue() < lowestcardthatcanbeat.getvalue()) lowestcardthatcanbeat = card;
            }
        }
        if(!gamecontroller.jackcollected && lowestcardthatcanbeat.getsuit() == gamecontroller.trumpsuit && lowestcardthatcanbeat.getvalue() > 11) return card(0,0);
        return lowestcardthatcanbeat;
    }
    void printhand()
    {
        for(auto card:hand) cout << char(card.getsuit()) << " " << card.getvalue() << "   ";
        cout << endl;
    }
};

class learner : public player
{
    void playcard(gamecontroller &gamecontroller, bool first)
    {
        hand.back().whoplayed = whichplayer;
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
    void printhand()
    {
        for(auto card:hand) cout << char(card.getsuit()) << " " << card.getvalue() << "   ";
        cout << endl;
    }
};

class human : public player
{
    void playcard(gamecontroller &gamecontroller, bool first)
    {
        printhand();
        cout << whichplayer << endl;
        int index = -1;
        cout << "which card do you want to play?" << endl;
        cin >> index;
        hand[index].whoplayed = whichplayer;
        if(first) gamecontroller.ledsuit = hand[index].getsuit();
        gamecontroller.trick.push_back(hand[index]);
        if(index!=hand.size() - 1) swap(*(hand.begin() + index),hand.back());
        hand.pop_back();
    }
    bool bet(gamecontroller &gamecontroller, const int &index)
    {
        printhand();
        cout << whichplayer << endl;
        cout << "How much do you want to bet? put -1 for no" << endl;
        int x;
        cin >> x;
        if(x == -1) return false;
        gamecontroller.whobet = whichplayer;
        gamecontroller.bet = x;
        cout << "which suit" << endl;
        char suit;
        cin >> suit;
        gamecontroller.trumpsuit = suit;
        return true;
    }
    void forcebet(gamecontroller &gamecontroller, const int &index)
    {
        gamecontroller.bet = 2;
        gamecontroller.whobet = index;
    }
    void printhand()
    {
        for(auto card:hand) cout << char(card.getsuit()) << " " << card.getvalue() << "   ";
        cout << endl;
    }
};

void setup(vector<player*> &players, deck &carddeck, gamecontroller &gamecontroller);
void dealcards(vector<player*> &players, const deck &carddeck, gamecontroller &gamecontroller);
void makebets(const vector<player*> &players, gamecontroller &gamecontroller);
void playtrick(const vector<player*> &players, gamecontroller &gamecontroller);

int main(int argc, const char * argv[])
{
    deck carddeck;
    vector<player*> players;
    gamecontroller gamecontroller;
    setup(players, carddeck, gamecontroller);
    while(gamecontroller.team1score < 11 && gamecontroller.team2score < 11)
    {
        dealcards(players, carddeck, gamecontroller);
        makebets(players, gamecontroller);
        for(int round = 0; round < 6; ++round) playtrick(players, gamecontroller);
        gamecontroller.resethand();
    }
    if(gamecontroller.team1score < 11) cout << "You Lose: Team 2 Wins" << endl;
    else cout << "You win!! Congrats" << endl;
    return 0;
}

void setup(vector<player*> &players, deck &carddeck, gamecontroller &gamecontroller)
{
    carddeck.makedeck();
    players.push_back(new human());
    players[0]->whichplayer = 0;
    for(int i = 1; i < 4; ++i)
    {
        players.push_back(new trainer());
        players[i]->whichplayer = i;
    }
}

void dealcards(vector<player*> &players, const deck &carddeck, gamecontroller &gamecontroller)
{
    int numplayers = int(players.size()), index = 0, i = 0;
    while(index != numplayers*6) players[(i++)%numplayers]->hand.push_back(carddeck.carddeck[index++]);
    gamecontroller.whodealt = (gamecontroller.whodealt+1) % 4;
}

void makebets(const vector<player*> &players, gamecontroller &gamecontroller)
{
    int currentbetter = gamecontroller.whodealt;
    for(int i = 0; i < 3; ++i)
    {
        players[currentbetter]->bet(gamecontroller, currentbetter%2);
        currentbetter = (currentbetter + 1) % 4;
    }
    //if nobody has bet yet, the dealer is screwed
    if(gamecontroller.bet == 0) players[currentbetter]->forcebet(gamecontroller,currentbetter);
    else players[currentbetter]->bet(gamecontroller,currentbetter);
    cout << endl;
}

void playtrick(const vector<player*> &players, gamecontroller &gamecontroller)
{
    int currplayer = gamecontroller.wholeads;
    players[currplayer]->playcard(gamecontroller,true);
    cout << endl;
    players[(currplayer+1)%4]->playcard(gamecontroller,false);
    cout << endl;
    players[(currplayer+2)%4]->playcard(gamecontroller,false);
    cout << endl;
    players[(currplayer+3)%4]->playcard(gamecontroller,false);
    cout << endl;
    gamecontroller.resettrick();
    cout << "continue?" << endl;
    cin >> currplayer;
    cout << endl << endl;
    cout << flush;
}
