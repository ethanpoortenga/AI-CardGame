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
    int gethierarchy(const char &trumpsuit)
    {
        if(suit == trumpsuit) return (value + 14);
        return value;
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
        //Don't include 6,7,8 in the deck
        for(int i = 2; i < 6; ++i) carddeck.push_back(card('s',i)); //spades
        for(int i = 9; i < 15; ++i) carddeck.push_back(card('s',i)); //spades
        for(int i = 2; i < 6; ++i) carddeck.push_back(card('c',i)); //clubs
        for(int i = 9; i < 15; ++i) carddeck.push_back(card('c',i)); //clubs
        for(int i = 2; i < 6; ++i) carddeck.push_back(card('h',i)); //hearts
        for(int i = 9; i < 15; ++i) carddeck.push_back(card('h',i)); //hearts
        for(int i = 2; i < 6; ++i) carddeck.push_back(card('d',i)); //diamonds
        for(int i = 9; i < 15; ++i) carddeck.push_back(card('d',i)); //diamonds
        shuffle();
    }
    void shuffle()
    {
        for(int i = 0; i < 4; ++i) random_shuffle(carddeck.begin(), carddeck.end());
    }
    vector<card> carddeck;
};

struct status
{
    //FOR THE GAME OVERALL
    int overallscore[2] = {0,0}, whodealt = -1;
    //FOR THE HAND
    int bet = 0, whobet = 0;
    char trumpsuit = ' ';
    vector<card> cardsplayed;
    bool jackcollected = false;
    int points[2] = {0,0};
    vector<int> jackqueenkingace = {0,0,0,0};
    //FOR THE TRICK
    vector<card> trick;
    int wholeads = -1, nummovestaken = 0;
    char ledsuit = ' ';
    //FOR RESETTING
    void resettrick()
    {
        int whowon = findtrickwinner();
        //if team 1 won
        if(whowon % 2 == 0) for(auto card:trick) card.whotookin = wholeads;
        //if team 2 won
        else for(auto card:trick) card.whotookin = (wholeads+1)%2;
        //clean up
        for(auto card:trick)
        {
            cardsplayed.push_back(card);
            points[whowon % 2] += card.getgame();
        }
        cout << "Player " << whowon << " won the trick!" << endl;
        wholeads = whowon;
        trick.clear();
    }
    void resethand()
    {
        int handpoints[2] = {0,0};
        //add all of the points
        ++handpoints[findhighestcard()];
        ++handpoints[findhighestcard()];
        if(jackcollected) ++handpoints[findjack()];
        if(points[0] != points[1]) ++handpoints[findgame()];
        //adjust the overall team score accordingly
        if(handpoints[whobet % 2] >= bet) for(int i = 0; i < 2; ++i) overallscore[i] += handpoints[i];
        else overallscore[whobet % 2] -= bet;
        //update statistics
        cardsplayed.clear();
        jackcollected = false;
        for(int i = 0; i < 4; ++i) jackqueenkingace[i] = false;
        points[0] = 0, points[1] = 0;
    }
    void resetgame()
    {
        overallscore[0] = 0, overallscore[1] = 0;
    }
    int findtrickwinner() //return 0 = team1, 1 = team2
    {
        card currenthighest(0,0);
        for(int i = 0; i < 4; ++i) if(trick[i].gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit))currenthighest = trick[i];
        return currenthighest.whoplayed;
    }
    int findhighestcard() //return 0 = team1, 1 = team2
    {
        card currenthighest(0,0);
        for(auto card : cardsplayed) if(card.gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit)) currenthighest = card;
        //return the team that won
        return (currenthighest.whoplayed % 2);
    }
    int findlowestcard()  //return 0 = team1, 1 = team2
    {
        card currentlowest(0,15);
        for(auto card : cardsplayed) if(card.getsuit() == trumpsuit && card.getvalue() < currentlowest.getvalue()) currentlowest = card;
        return (currentlowest.whoplayed % 2);
    }
    int findjack()        //return 0 = team1, 1 = team2
    {
        for(auto card:cardsplayed) if(card.getvalue() == 11 && card.getsuit() == trumpsuit) return (card.whotookin);
        return -1; //will never get here but for compile reasons
    }
    int findgame()        //return 0 = team1, 1 = team2
    {
        return(points[0] < points[1]);
    }
};

class player
{
public:
    virtual bool bet(status &status, const int &index) = 0;
    virtual void forcebet(status &status, const int &index) = 0;
    virtual void playcard(status &status, bool first) = 0;
    virtual void printhand() = 0;
    vector<card> hand;
    int whichplayer = 0;
};

class trainer : public player
{
    //STILL NEED TO IMPLEMENT THIS LOGIC
    bool bet(status &status, const int &index)
    {
        printhand();
        cout << whichplayer << endl;
        int bet = 0;
        cout << "bet? put -1 for no" << endl;
        cin >> bet;
        if(bet == -1) return false;
        status.bet = bet;
        status.whobet = index;
        cout << "which suit" << endl;
        char suit;
        cin >> suit;
        status.trumpsuit = suit;
        return true;
    }
    void forcebet(status &status)
    {
        status.bet = 2;
        status.whobet = whichplayer;
    }
    void playcard(status &status, bool first)
    {
        cout << "Player " << whichplayer << ": Trainer" << endl;
        printhand();
        pair<bool,int> result(false,0);
        //if first player lead out (different logic)
        if(first) return leadout(status);
        //check to see whether the jack can be taken in
        if(!status.jackcollected) result = proirity1(status);
        if(result.first == true) return playcardhelper(result, status);
        //check to see whether a significant amount of game can be taken in/contributed
        result = proirity2(status);
        if(result.first == true) return playcardhelper(result, status);
        // attempt to take in the trick, if not possible return worst card
        result = proirity3(status);
        return playcardhelper(result, status);
    }
    void playcardhelper(pair<bool,int> &result, status &status)
    {
        hand[result.second].whoplayed = whichplayer;
        status.trick.push_back(hand[result.second]);
        hand.erase(hand.begin() + result.second);
        cout << "Card Played: " << char(hand.back().getsuit() - 32) << " " << hand.back().getvalue() << endl;
    }
    void leadout(status &status)
    {
        //remember to set the ledsuit
    }
    pair<bool,int> proirity1(status &status) //jack
    {
        //assess situation around taking in the jack
        vector<int> cardsjackandabove = status.jackqueenkingace; //0=not seen,1=taken in,2=in my hand
        int highestvalueinhand = 0;
        card highestvalueintrick(0,0);
        for(auto card:hand)
        {
            if(card.getsuit() == status.trumpsuit && card.getvalue() > 10) cardsjackandabove[card.getvalue() - 11] = 2;
            if(card.getsuit() == status.trumpsuit && card.getvalue() > highestvalueinhand) highestvalueinhand = card.getvalue();
        }
        for(auto card:status.trick) if(card.getsuit() == status.trumpsuit && card.getvalue() > highestvalueintrick.getvalue()) highestvalueintrick = card;
        //if i can't take in the jack, return
        if(cardsjackandabove[0] != 2 && cardsjackandabove[1] != 2 && cardsjackandabove[2] != 2 && cardsjackandabove[3] != 2) return pair<bool,int>(false,0);
        if(highestvalueintrick.getvalue() > highestvalueinhand)
        {
            if(highestvalueintrick.whoplayed % 2 == whichplayer % 2) return playmostgame(status.ledsuit, status.trumpsuit);
            return pair<bool,int>(false,0);
        }
        //check whether the jack is out in the trick
        card jack(0,0);
        if(cardsjackandabove[0] != 2) for(auto card:status.trick) if(card.getvalue() == 11 && card.getsuit() == status.trumpsuit) jack = card;
        //scenarios to check for
            //jack out, partner winning, last move
            //jack out, partner winning, others cant take in
            //jack out, partner winning, others can take in
            //jack out, other winning, last move
            //jack out, other winning, others cant take in
            //jack out, other winning, others can take in
            //i have jack, others cant take in
            //i have jack, others can take in
        if(jack.getvalue() != 0)
        {
            if(status.findtrickwinner() % 2 == whichplayer % 2)
            {
                if(status.nummovestaken == 3) return playmostgame(status.ledsuit, status.trumpsuit);
                for(int index = 3; index >= 0; --index)
                {
                    if(cardsjackandabove[index] == 2) for(int i = 0; i < (int)hand.size(); ++i) if(hand[i].getvalue() == (index + 11) && hand[i].getsuit() == status.trumpsuit) return pair<bool,int>(true,i);
                    if(cardsjackandabove[index] == 0) return playworstcard(status.ledsuit, status.trumpsuit);
                }
            }
            else
            {
                if(status.nummovestaken == 3) return pair<bool,int>(true,findlowestcardthatcanbeat(status,highestvalueintrick));
                for(int index = 3; index >= 0; --index)
                {
                    if(cardsjackandabove[index] == 2) for(int i = 0; i < (int)hand.size(); ++i) if(hand[i].getvalue() == (index + 11) && hand[i].getsuit() == status.trumpsuit) return pair<bool,int>(true,i);
                    if(cardsjackandabove[index] == 0) return playworstcard(status.ledsuit, status.trumpsuit);
                }
            }
        }
        
        
        
        
        
        
        
        
        //check to see whether others could take in the jack
        if(cardsjackandabove[0] != 0 && cardsjackandabove[1] != 0 && cardsjackandabove[2] != 0 && cardsjackandabove[3] != 0)
        {
            //check whether my team is already taking it
            if(havejack) return pair<bool,card>(true,cardtotakeinjack);
            else if(jackisout && teamplayedjack == whichplayer%2) return playmostgame(status.ledsuit, status.trumpsuit);
            else if(jackisout) return pair<bool,card>(true,cardtotakeinjack);
        }
        return pair<bool,card>(false,card(0,0));
    }
    pair<bool,int> proirity2(status &status) //game
    {
        //check whether game has already been won
        if(status.points[0] > 40 || status.points[1] > 40) return pair<bool,card>(false,card(0,0));
        //check to see whether you can take in a ten
        if(status.nummovestaken == 3)
        {
            bool tenledsuit = false;
            bool tentrumpsuit = false;
            for(auto card: hand)
            {
                if(card.getvalue() == 10 && card.getsuit() == status.ledsuit) tenledsuit = true;
                if(card.getvalue() == 10 && card.getsuit() == status.trumpsuit) tentrumpsuit = true;
            }
            //check whether the other person would take in the card
            bool otherwouldtakeinledsuit = false;
            bool otherwouldtakeintrumpsuit = false;
            if(tenledsuit)
            {
                for(auto card:status.trick) if(card.getvalue() > 10 && card.getsuit() == status.ledsuit && card.whoplayed == (whichplayer + 1) % 2) otherwouldtakeinledsuit = true;
            }
            for(auto card : status.trick) if(card.getvalue() > 10 && card.getsuit() == status.trumpsuit) otherwouldtakeintrumpsuit = true;
            if(!otherwouldtakeinledsuit && !otherwouldtakeintrumpsuit)
            {
                card cardtoplay(0,0);
                for(auto card:hand) if(card.getvalue() == 10 && card.getsuit() == status.ledsuit) cardtoplay = card;
                return pair<bool,card>(true,cardtoplay);
            }
            else if(!otherwouldtakeintrumpsuit)
            {
                card cardtoplay(0,0);
                for(auto card:hand) if(card.getvalue() == 10 && card.getsuit() == status.trumpsuit) cardtoplay = card;
                return pair<bool,card>(true,cardtoplay);
            }
        }
        //check whether there's substantial game out
        int gameintrick = 0;
        for(auto card : status.trick)
        {
            if(card.getvalue() > 10) gameintrick += card.getvalue() - 10;
            else if(card.getvalue() == 10) gameintrick += 10;
        }
        //yes substantial game (Greater than 7 or will win game point)
        if(gameintrick > 7 || (40 - status.points[whichplayer%2]) <= gameintrick)
        {
            //work on taking in the game
            //check to see whether partner is already winning
            card currenthighest(0,0);
            bool myteamwinning = false;
            for(auto card:status.trick)
            {
                if(card.getvalue() > currenthighest.getvalue())
                {
                    if (card.getsuit() == status.trumpsuit)
                    {
                        currenthighest = card;
                        if(card.whoplayed%2 == whichplayer%2) myteamwinning = true;
                        else myteamwinning = false;
                    }
                    else if (card.getsuit() == status.ledsuit)
                    {
                        if(currenthighest.getsuit() != status.trumpsuit)
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
                if(status.ledsuit != status.trumpsuit) return playmostgame(status.ledsuit, status.trumpsuit);
                return playworstcard(status.ledsuit, status.trumpsuit);
            }
            //other team winning
            {
                if(status.nummovestaken != 3)
                {
                    card cardtoplay = highestcardsamesuit(status, currenthighest);
                    if(cardtoplay.getsuit() != 0) return pair<bool,card>(true,cardtoplay);
                    else return playworstcard(status.ledsuit, status.trumpsuit);
                }
                //find the lowest card I have that can beat them
                card lowestcardthatcanbeat = findlowestcardthatcanbeat(status, currenthighest);
                //if the current highest is trumpsuit
                if(lowestcardthatcanbeat.getsuit() != 0) return pair<bool,card>(true,lowestcardthatcanbeat);
                else return playworstcard(status.ledsuit, status.trumpsuit);
            }
        }
        //no substantial game
        else return pair<bool,card>(false,card(0,0));
    }
    pair<bool,int> proirity3(status &status) //taking in the trick
    {
        //check to see who's winning
        card currenthighest(0,0);
        bool myteamwinning = false;
        for(auto card:status.trick)
        {
            if(card.getvalue() > currenthighest.getvalue())
            {
                if (card.getsuit() == status.trumpsuit)
                {
                    currenthighest = card;
                    if(card.whoplayed%2 == whichplayer%2) myteamwinning = true;
                    else myteamwinning = false;
                }
                else if (card.getsuit() == status.ledsuit)
                {
                    if(currenthighest.getsuit() != status.trumpsuit)
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
            if(status.ledsuit != status.trumpsuit) return playmostgame(status.ledsuit, status.trumpsuit);
            return playworstcard(status.ledsuit, status.trumpsuit);
        }
        //see whether I can win
        else
        {
            if(status.nummovestaken != 3)
            {
                card cardtoplay = highestcardsamesuit(status, currenthighest);
                if(cardtoplay.getsuit() != 0) return pair<bool,card>(true,cardtoplay);
                else return playworstcard(status.ledsuit, status.trumpsuit);
            }
            card lowestcardthatcanbeat = findlowestcardthatcanbeat(status, currenthighest);
            if(lowestcardthatcanbeat.getsuit() != 0) return pair<bool,card>(true,lowestcardthatcanbeat);
            else return playworstcard(status.ledsuit, status.trumpsuit);
        }
    }
    pair<bool,int> playworstcard(const char &ledsuit, const char &trumpsuit)
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
    pair<bool,int> playmostgame(const char &ledsuit, const char &trumpsuit)
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
    int highestcardsamesuit(status &status, card &currenthighest)
    {
        card cardtoplay(0,0);
        for(auto card:hand)
        {
            if(card.getsuit() == status.ledsuit && card.getvalue() > currenthighest.getvalue() && card.getvalue() > cardtoplay.getvalue()) cardtoplay = card;
        }
        return cardtoplay;
    }
    int findlowestcardthatcanbeat(status &status, card &currenthighest)
    {
        card lowestcardthatcanbeat(0,15);
        if(currenthighest.getsuit() == status.trumpsuit)
        {
            for(auto card: hand) if(card.getsuit() == status.trumpsuit && card.getvalue() > currenthighest.getvalue() && card.getvalue() < lowestcardthatcanbeat.getvalue()) lowestcardthatcanbeat = card;
        }
        //if the current highest is not trump suit
        else
        {
            //first pass looking for ledsuit
            for(auto card:hand)
            {
                if(card.getsuit() == status.ledsuit && card.getvalue() > currenthighest.getvalue() && card.getvalue() < lowestcardthatcanbeat.getvalue()) lowestcardthatcanbeat = card;
            }
            if(lowestcardthatcanbeat.getvalue() != 15) return lowestcardthatcanbeat;
            //second pass looking for trumpsuit
            for(auto card:hand)
            {
                if(card.getsuit() == status.trumpsuit && card.getvalue() < lowestcardthatcanbeat.getvalue()) lowestcardthatcanbeat = card;
            }
        }
        if(!status.jackcollected && lowestcardthatcanbeat.getsuit() == status.trumpsuit && lowestcardthatcanbeat.getvalue() > 11) return card(0,0);
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
    void playcard(status &status, bool first)
    {
        hand.back().whoplayed = whichplayer;
        status.trick.push_back(hand.back());
        hand.pop_back();
    }
    bool bet(status &status, const int &index)
    {
        status.bet = 2;
        status.whobet = index;
        return true;
    }
    void forcebet(status &status, const int &index)
    {
        status.bet = 2;
        status.whobet = index;
    }
    void printhand()
    {
        for(auto card:hand) cout << char(card.getsuit()) << " " << card.getvalue() << "   ";
        cout << endl;
    }
};

class human : public player
{
    void playcard(status &status, bool first)
    {
        printhand();
        cout << whichplayer << endl;
        int index = -1;
        cout << "which card do you want to play?" << endl;
        cin >> index;
        hand[index].whoplayed = whichplayer;
        if(first) status.ledsuit = hand[index].getsuit();
        status.trick.push_back(hand[index]);
        if(index!=hand.size() - 1) swap(*(hand.begin() + index),hand.back());
        hand.pop_back();
    }
    bool bet(status &status, const int &index)
    {
        printhand();
        cout << whichplayer << endl;
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
    void forcebet(status &status, const int &index)
    {
        status.bet = 2;
        status.whobet = index;
    }
    void printhand()
    {
        for(auto card:hand) cout << char(card.getsuit()) << " " << card.getvalue() << "   ";
        cout << endl;
    }
};

void setup(vector<player*> &players, deck &carddeck, status &status);
void dealcards(vector<player*> &players, const deck &carddeck, status &status);
void makebets(const vector<player*> &players, status &status);
void playtrick(const vector<player*> &players, status &status);

int main(int argc, const char * argv[])
{
    deck carddeck;
    vector<player*> players;
    status status;
    setup(players, carddeck, status);
    while(status.team1score < 11 && status.team2score < 11)
    {
        dealcards(players, carddeck, status);
        makebets(players, status);
        for(int round = 0; round < 6; ++round) playtrick(players, status);
        status.resethand();
    }
    if(status.team1score < 11) cout << "You Lose: Team 2 Wins" << endl;
    else cout << "You win!! Congrats" << endl;
    return 0;
}

void setup(vector<player*> &players, deck &carddeck, status &status)
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
        players[currentbetter]->bet(status, currentbetter%2);
        currentbetter = (currentbetter + 1) % 4;
    }
    //if nobody has bet yet, the dealer is screwed
    if(status.bet == 0) players[currentbetter]->forcebet(status,currentbetter);
    else players[currentbetter]->bet(status,currentbetter);
    cout << endl;
}

void playtrick(const vector<player*> &players, status &status)
{
    int currplayer = status.wholeads;
    players[currplayer]->playcard(status,true);
    cout << endl;
    players[(currplayer+1)%4]->playcard(status,false);
    cout << endl;
    players[(currplayer+2)%4]->playcard(status,false);
    cout << endl;
    players[(currplayer+3)%4]->playcard(status,false);
    cout << endl;
    status.resettrick();
    cout << "continue?" << endl;
    cin >> currplayer;
    cout << endl << endl;
    cout << flush;
}
