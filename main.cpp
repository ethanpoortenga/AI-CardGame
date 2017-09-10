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
#include <cassert>
#include <cmath>
#include <ctime>
#include <fstream>

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
    bool operator==(const card &other) { return (other.suit == suit && other.value == value); }
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
        seedrand();
        shuffle();
    }
    void shuffle()
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
    void seedrand()
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
    vector<card> carddeck;
    int seed;
};

struct status
{
    //FOR THE LEARNER
    vector<int> gamestatus;
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
        int whowon = findtrickwinner().whoplayed;
        //if team 1 won
        for(auto &card:trick) card.whotookin = (whowon % 2);
        //clean up
        for(auto card:trick)
        {
            cardsplayed.push_back(card);
            if(card.gethierarchy(trumpsuit) == 25) jackcollected = true;
            points[whowon % 2] += card.getgame();
        }
        cout << "Player " << whowon << " won the trick!" << endl;
        wholeads = whowon;
        nummovestaken = 0;
        trick.clear();
    }
    void resethand()
    {
        int handpoints[2] = {0,0};
        //add all of the points
        ++handpoints[findhighestcard()];
        ++handpoints[findlowestcard()];
        if(jackcollected) ++handpoints[findjack()];
        if(points[0] != points[1]) ++handpoints[(points[0] < points[1])];
        //adjust the overall team score accordingly
        if(handpoints[whobet % 2] >= bet) for(int i = 0; i < 2; ++i) overallscore[i] += handpoints[i];
        else overallscore[whobet % 2] -= bet;
        //update statistics
        cardsplayed.clear();
        jackcollected = false;
        for(int i = 0; i < 4; ++i) jackqueenkingace[i] = false;
        points[0] = 0, points[1] = 0;
        bet = 0;
        cout << "Team A won " << handpoints[0] << ", Team B won " << handpoints[1] << endl;
        cout << "Current Score: Team A: " << overallscore[0] << ", Team B: " << overallscore[1] << endl << endl;
    }
    void resetgame()
    {
        overallscore[0] = 0, overallscore[1] = 0;
    }
    card findtrickwinner() //return 0 = team1, 1 = team2
    {
        card currenthighest(0,0);
        for(auto card : trick) if(card.gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit))currenthighest = card;
        return currenthighest;
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
    vector<int> getstatus(vector<card> hand)
    {
        vector<int> binarystatus(120); //hand - 40, trick - 40, collected - 40
        for(auto card: hand)
        {
            //convert value to 0-9
            int value = card.getvalue() - 2;
            if(value > 6) value = value - 3;
            //convert suit to 0-3 (0-clubs,1-diamonds,2-hearts,3-spades)
            int suit = card.getsuit();
            suit = (suit > 'c') + (suit > 'd') + (suit > 'h');
            binarystatus[suit*10 + value] = 1;
        }
        for(auto card: trick)
        {
            //convert value to 0-9
            int value = card.getvalue() - 2;
            if(value > 6) value = value - 3;
            //convert suit to 0-3 (0-clubs,1-diamonds,2-hearts,3-spades)
            int suit = card.getsuit();
            suit = (suit > 'c') + (suit > 'd') + (suit > 'h');
            binarystatus[suit*10 + value + 40] = 1;

        }
        for(auto card: cardsplayed)
        {
            //convert value to 0-9
            int value = card.getvalue() - 2;
            if(value > 6) value = value - 3;
            //convert suit to 0-3 (0-clubs,1-diamonds,2-hearts,3-spades)
            int suit = card.getsuit();
            suit = (suit > 'c') + (suit > 'd') + (suit > 'h');
            binarystatus[suit*10 + value + 80] = 1;
            
        }
        return binarystatus;
    }
};

class player
{
public:
    virtual bool bet(status &status, const int &index) = 0;
    virtual void forcebet(status &status) = 0;
    virtual void playcard(status &status, bool first) = 0;
    virtual void printhand() = 0;
    vector<card> hand;
    int whichplayer = 0;
};

class trainer : public player
{
    bool bet(status &status, const int &index)
    {
        printhand();
        cout << "Player " << whichplayer << ": Trainer" << endl;
        vector<int> suits = {0,0,0,0};
        for(auto card: hand)
        {
            if(card.getvalue() == 14) bethelper(card.getsuit(), 10, suits);
            if(card.getvalue() == 13) bethelper(card.getsuit(), 9, suits);
            if(card.getvalue() == 12) bethelper(card.getsuit(), 6, suits);
            if(card.getvalue() == 11) bethelper(card.getsuit(), 9, suits);
            if(card.getvalue() == 10) bethelper(card.getsuit(), 4, suits);
            if(card.getvalue() == 3) bethelper(card.getsuit(), 4, suits);
            if(card.getvalue() == 2) bethelper(card.getsuit(), 7, suits);
            else bethelper(card.getsuit(), 1, suits);
        }
        int maxstrength = 0, maxindex = 0;
        for(int i = 0; i < 3; ++i) if(suits[i] > maxstrength)
        {
            maxstrength = suits[i];
            maxindex = i;
        }
        if(maxstrength > 42 && status.bet < 4)
        {
            status.bet = 4;
            status.whobet = whichplayer;
            settrumpsuit(maxindex,status);
            cout << "Bet 4 with strength " << maxstrength << endl << endl;
            return true;
        }
        if(maxstrength > 32 && status.bet < 3)
        {
            status.bet = 3;
            status.whobet = whichplayer;
            settrumpsuit(maxindex,status);
            cout << "Bet 3 with strength " << maxstrength << endl << endl;
            return true;
        }
        if(maxstrength > 22 && status.bet < 2)
        {
            status.bet = 2;
            status.whobet = whichplayer;
            settrumpsuit(maxindex,status);
            cout << "Bet 2 with strength " << maxstrength << endl << endl;
            return true;
        }
        cout << "Bet 0 with strength " << maxstrength << endl << endl;
        return false;
    }
    void bethelper(int suit, int worth, vector<int> &suits)
    {
        int suitindex;
        if(suit == 's') suitindex = 3;
        else if(suit == 'c') suitindex = 2;
        else suitindex = (suit == 'h');
        if(worth == 1) suits[suitindex] += 1;
        else
        {
            for(int i = 0; i < 3; ++i)
            {
                if(i == suitindex) suits[i] += worth;
                else suits[i] += worth*0.5;
            }
        }
    }
    void settrumpsuit(int suit, status &status)
    {
        if(suit == 3) status.trumpsuit = 's';
        if(suit == 2) status.trumpsuit = 'c';
        if(suit == 1) status.trumpsuit = 'h';
        if(suit == 0) status.trumpsuit = 'd';
    }
    void forcebet(status &status)
    {
        printhand();
        cout << whichplayer << endl;
        cout << "Forced to bet" << endl << endl;
        status.bet = 2;
        status.whobet = whichplayer;
        int whichsuit[4] = {0,0,0,0};
        for(auto card:hand)
        {
            if(card.getvalue() < 4) whichsuit[forcebethelper(card)] += 6;
            else if(card.getvalue() > 13) whichsuit[forcebethelper(card)] += 12;
            else if(card.getvalue() > 11) whichsuit[forcebethelper(card)] += 8;
            else if(card.getvalue() == 11) whichsuit[forcebethelper(card)] += 6;
            else if(card.getvalue() == 10) whichsuit[forcebethelper(card)] += 4;
        }
        int max = 0, index = 0;
        for(int i = 0; i < 4; ++i)
        {
            if(whichsuit[i] > max)
            {
                index = i;
                max = whichsuit[i];
            }
        }
        if(index == 0) status.trumpsuit = 's';
        if(index == 1) status.trumpsuit = 'c';
        if(index == 2) status.trumpsuit = 'h';
        if(index == 3) status.trumpsuit = 'd';
    }
    int forcebethelper(card helper)
    {
        if(helper.getsuit() == 's') return 0;
        if(helper.getsuit() == 'c') return 1;
        if(helper.getsuit() == 'h') return 2;
        return 3;
    }
    void playcard(status &status, bool first)
    {
        cout << "Player " << whichplayer << ": Trainer" << endl;
        printhand();
        //if first player lead out (different logic)
        if(first)
        {
            leadout(status);
            return;
        }
        playablecards.clear();
        bool ledsuitseen = false;
        for(auto card:hand)
        {
            if(card.getsuit() == status.ledsuit)
            {
                playablecards.push_back(card);
                ledsuitseen = true;
            }
            else if(card.getsuit() == status.trumpsuit) playablecards.push_back(card);
        }
        if(playablecards.size() == 1)
        {
            playcardhelper(pair<bool,card>(true,playablecards[0]), status);
            return;
        }
        if(!ledsuitseen) playablecards = hand;
        pair<bool,card> result(false,card(0,0));
        //check to see whether the jack can be taken in
        if(!status.jackcollected) result = proirity1(status);
        if(result.first == true)
        {
            playcardhelper(result, status);
            return;
        }
        //check to see whether a significant amount of game can be taken in/contributed
        result = proirity2(status);
        if(result.first == true)
        {
            playcardhelper(result, status);
            return;
        }
        // attempt to take in the trick, if not possible return worst card
        result = proirity3(status);
        playcardhelper(result, status);
        return;
    }
    void playcardhelper(pair<bool,card> result, status &status)
    {
        assert(result.second.getsuit() != 0);
        result.second.whoplayed = whichplayer;
        ++status.nummovestaken;
        cout << "Card Played: " << char(result.second.getsuit() - 32) << " " << result.second.getvalue() << endl;
        status.trick.push_back(result.second);
        for(int i = 0; i < (int)hand.size(); ++i) if(hand[i].getsuit() == result.second.getsuit() && hand[i].getvalue() == result.second.getvalue()) hand.erase(hand.begin() + i);
    }
    void leadout(status &status)
    {
        pair<bool,card> choice(true,card(0,0));
        choice.first = true;
        //if it's the first play of the hand
        if(status.cardsplayed.size() == 0)
        {
            card greaterthanjack(status.trumpsuit,11), lessthanten(status.trumpsuit,10);
            for(auto card: hand)
            {
                if(card.gethierarchy(status.trumpsuit) > greaterthanjack.gethierarchy(status.trumpsuit))greaterthanjack = card;
                if(card.gethierarchy(status.trumpsuit) < lessthanten.gethierarchy(status.trumpsuit) && card.gethierarchy(status.trumpsuit) > 15) lessthanten = card;
            }
            if(greaterthanjack.getvalue() != 11)
            {
                choice.second = greaterthanjack;
                status.ledsuit = status.trumpsuit;
                playcardhelper(choice, status);
                return;
            }
            if(lessthanten.getvalue() != 11)
            {
                choice.second = lessthanten;
                status.ledsuit = status.trumpsuit;
                playcardhelper(choice, status);
                return;
            }
            else
            {
                for(auto card: hand)
                {
                    if(card.getsuit() == status.trumpsuit)
                    {
                        choice.second = card;
                        status.ledsuit = status.trumpsuit;
                        playcardhelper(choice, status);
                    }
                }
            }
        }
        //if you're just leading out a trick
        else
        {
            //proirity 1 = high trump suit
            //proitity 2 = high other suit
            //priority 3 = low trump suit
            //proirity 4 = any other suit
            char trumpsuit = status.trumpsuit;
            card hightrumpsuit(0,0), highothersuit(0,0), lowtrumpsuit(0,0), anyothersuit(0,0);
            for(auto card: hand)
            {
                //hightrumpsuit
                if(card.gethierarchy(trumpsuit) > 25) //above jack
                {
                    if(hightrumpsuit.getvalue() == 0) hightrumpsuit = card;
                    else if(hightrumpsuit.gethierarchy(trumpsuit) < card.gethierarchy(trumpsuit)) hightrumpsuit = card;
                }
                //high other suit
                else if(card.gethierarchy(trumpsuit) < 15 && card.gethierarchy(trumpsuit) > 10)
                {
                    if(highothersuit.getvalue() == 0) highothersuit = card;
                    else if(highothersuit.gethierarchy(trumpsuit) < card.gethierarchy(trumpsuit)) highothersuit = card;
                }
                //lowtrumpsuit
                else if(card.gethierarchy(trumpsuit) < 24 && card.gethierarchy(trumpsuit) > 15)
                {
                    if(lowtrumpsuit.getvalue() == 0) lowtrumpsuit = card;
                    else if(lowtrumpsuit.gethierarchy(trumpsuit) > card.gethierarchy(trumpsuit)) lowtrumpsuit = card;
                }
                //any other suit
                else if(card.getvalue() < 10)
                {
                    anyothersuit = card;
                }
            }
            if(hightrumpsuit.getvalue() != 0)
            {
                choice.second = hightrumpsuit;
                status.ledsuit = hightrumpsuit.getsuit();
                playcardhelper(choice, status);
                return;
            }
            if(highothersuit.getvalue() != 0)
            {
                choice.second = highothersuit;
                status.ledsuit = highothersuit.getsuit();
                playcardhelper(choice, status);
                return;
            }
            if(lowtrumpsuit.getvalue() != 0)
            {
                choice.second = lowtrumpsuit;
                status.ledsuit = lowtrumpsuit.getsuit();
                playcardhelper(choice, status);
                return;
            }
            if(anyothersuit.getvalue() != 0)
            {
                choice.second = anyothersuit;
                status.ledsuit = anyothersuit.getsuit();
                playcardhelper(choice, status);
                return;
            }
            //should never get here but for compile reasons
            choice.second = hand[0];
            status.ledsuit = hand[0].getsuit();
            playcardhelper(choice, status);
            return;
        }
    }
    pair<bool,card> proirity1(status &status) //jack
    {
        card winningtrick = status.findtrickwinner();
        bool myteamwinning = (winningtrick.whoplayed % 2 == whichplayer % 2);
        const char trumpsuit = status.trumpsuit;
        vector<int> cardsjackandabove = status.jackqueenkingace; //0=unknown,1=played,2=hand,3=trick
        //update some information
        for(auto card: playablecards) if(card.gethierarchy(trumpsuit) > 24) cardsjackandabove[card.getvalue()-11] = 2;
        for(auto card: status.trick) if(card.gethierarchy(trumpsuit) > 24) cardsjackandabove[card.getvalue()-11] = 3;
        //if you can play the jack and take it in
        if(cardsjackandabove[0] == 2)
        {
            if(status.nummovestaken == 3) //last move
            {
                if(myteamwinning) return pair<bool,card>(true,card(trumpsuit,11));
                if(winningtrick.gethierarchy(trumpsuit)< 25) return pair<bool,card>(true,card(trumpsuit,11));
                return pair<bool,card>(false,card(0,0));
            }
            //check if I can get rid of the jack some other way
            for(int i = 3; i >=0; --i)
            {
                if(i == 0) return pair<bool,card>(true,card(trumpsuit,11));
                else if(cardsjackandabove[i] == 0) return pair<bool,card>(false,card(0,0));
                else if(cardsjackandabove[i] == 3)
                {
                    if(myteamwinning) return pair<bool,card>(true,card(trumpsuit,11));
                    else return pair<bool,card>(false,card(0,0));
                }
            }
        }
        //in the trick, can I take it in?
        else if(cardsjackandabove[0] == 3)
        {
            int couldtakein = -1;
            if(myteamwinning)
            {
                if(status.nummovestaken == 3) return playmostgame(status.ledsuit, trumpsuit, winningtrick);
                for(int i = 3; i >=0; --i)
                {
                    if(cardsjackandabove[i] == 0)
                    {
                        if(couldtakein == -1) return pair<bool,card>(false,card(0,0));
                        else return pair<bool,card>(true,card(trumpsuit,i+11));
                    }
                    if(cardsjackandabove[i] == 2) couldtakein = i;
                    else if(cardsjackandabove[i] == 3) return playmostgame(status.ledsuit, trumpsuit, winningtrick);
                }
            }
            else
            {
                for(int i = 3; i >=0; --i)
                {
                    if(cardsjackandabove[i] == 0 && status.nummovestaken != 3) return pair<bool,card>(false,card(0,0));
                    else if(cardsjackandabove[i] == 2) return pair<bool,card>(true,card(trumpsuit,i+11));
                    else if(cardsjackandabove[i] == 3) return playworstcard(status.ledsuit, trumpsuit);
                }
            }
        }
        return pair<bool,card>(false,card(0,0));
    }
    pair<bool,card> proirity2(status &status) //game
    {
        //check whether game has already been won
        if(status.points[0] > 40 || status.points[1] > 40) return pair<bool,card>(false,card(0,0));
        card winningtrick = status.findtrickwinner();
        bool myteamwinning = (winningtrick.whoplayed % 2 == whichplayer % 2);
        //check whether you have any 10s
        bool tentrumpsuit = false, tenledsuit = false;
        char othersuit = 0;
        for(auto card: playablecards)
        {
            if(card.getvalue() == 10 && card.getsuit() == status.ledsuit) tenledsuit = true;
            else if(card.getvalue() == 10 && card.getsuit() == status.trumpsuit) tentrumpsuit = true;
            else if(card.getvalue() == 10 && card.getsuit() != status.trumpsuit) othersuit = card.getsuit();
        }
        //check to see whether you can take in a ten
        if(othersuit != 0 || tentrumpsuit || tenledsuit)
        {
            if(status.nummovestaken == 3)
            {
                if(myteamwinning)
                {
                    if(othersuit != 0) return pair<bool,card>(true,card(othersuit,10));
                    if(tenledsuit) return pair<bool,card>(true,card(status.ledsuit,10));
                }
                else if(winningtrick.gethierarchy(status.trumpsuit)< 10 && tenledsuit) return pair<bool,card>(true,card(status.ledsuit,10));
                else if(winningtrick.gethierarchy(status.trumpsuit)< 24 && tentrumpsuit) return pair<bool,card>(true,card(status.trumpsuit,10));
            }
            else
            {
                vector<int> cardsjackandabove = status.jackqueenkingace; //0=unknown,1=played,2=hand,3=trick
                //update some information
                for(auto card: playablecards) if(card.gethierarchy(status.trumpsuit) > 24) cardsjackandabove[card.getvalue()-11] = 2;
                for(auto card: status.trick) if(card.gethierarchy(status.trumpsuit) > 24) cardsjackandabove[card.getvalue()-11] = 3;
                for(int i = 3; i > -1; --i)
                {
                    if(cardsjackandabove[i] == 0) break;
                    if(cardsjackandabove[i] == 3)
                    {
                        if(myteamwinning)
                        {
                            if(othersuit != 0) return pair<bool,card>(true,card(othersuit,10));
                            if(tenledsuit) return pair<bool,card>(true,card(status.ledsuit,10));
                            if(tentrumpsuit) return pair<bool,card>(true,card(status.trumpsuit,10));
                        }
                        else break;
                    }
                }
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
            //my team winning
            if(myteamwinning)
            {
                if(status.nummovestaken == 3) return playmostgame(status.ledsuit, status.trumpsuit, winningtrick);
                if(gameintrick >= 10)
                {
                    card currentmax(0,0);
                    for(auto card: playablecards) if(card.gethierarchy(status.trumpsuit) > currentmax.gethierarchy(status.trumpsuit)) currentmax = card;
                    if(currentmax.gethierarchy(status.trumpsuit) > winningtrick.gethierarchy(status.trumpsuit)) return pair<bool,card>(true,currentmax);
                }
                if(gameintrick < 10)
                {
                    card cardtoplay = findlowestcardthatcanbeat(status.trumpsuit, winningtrick);
                    if(cardtoplay.getvalue() != -1) return pair<bool,card>(true,cardtoplay);
                }
                return pair<bool,card>(false,card(0,0));
            }
            //other team winning
            else
            {
                if(status.nummovestaken == 3)
                {
                    card cardtoplay = findlowestcardthatcanbeat(status.trumpsuit, winningtrick);
                    if(cardtoplay.getvalue() != -1) return pair<bool,card>(true,cardtoplay);
                    return pair<bool,card>(false,card(0,0));
                }
                card cardtoplay(0,0);
                if(gameintrick > 10)
                {
                    for(int i = 0; i < (int)playablecards.size(); ++i) if(playablecards[i].gethierarchy(status.trumpsuit) > winningtrick.gethierarchy(status.trumpsuit) && playablecards[i].gethierarchy(status.trumpsuit) >= cardtoplay.gethierarchy(status.trumpsuit) && !(!status.jackcollected && playablecards[i].gethierarchy(status.trumpsuit) > 24)) cardtoplay = playablecards[i];
                    if(playablecards[0].gethierarchy(status.trumpsuit) > winningtrick.gethierarchy(status.trumpsuit)) return pair<bool,card>(true,cardtoplay);
                }
                if(gameintrick < 10)
                {
                    cardtoplay = findlowestcardthatcanbeat(status.trumpsuit, winningtrick);
                    if(cardtoplay.getvalue() != -1) return pair<bool,card>(true,cardtoplay);
                }
                return pair<bool,card>(false,card(0,0));
            }
        }
        //no substantial game
        else return pair<bool,card>(false,card(0,0));
    }
    pair<bool,card> proirity3(status &status) //taking in the trick
    {
        //find which card is currently winning the trick
        card trickwinner = status.findtrickwinner();
        //see whether I can win
        bool canwin = false;
        for(auto card: playablecards)
        {
            if(card.gethierarchy(status.trumpsuit) > trickwinner.gethierarchy(status.trumpsuit) && card.gethierarchy(status.trumpsuit) < 24)
            {
                canwin = true;
                break;
            }
        }
        if(!canwin) return playworstcard(status.ledsuit, status.trumpsuit);
        else
        {
            if(status.nummovestaken == 3) return playworstcard(status.ledsuit, status.trumpsuit);
            card cardtoplay(0,0);
            for(auto card: playablecards) if(card.gethierarchy(status.trumpsuit) > trickwinner.gethierarchy(status.trumpsuit) && card.gethierarchy(status.trumpsuit) >= cardtoplay.gethierarchy(status.trumpsuit) && !(!status.jackcollected && card.gethierarchy(status.trumpsuit) > 24)) cardtoplay = card;
            if(playablecards[0].gethierarchy(status.trumpsuit) > trickwinner.gethierarchy(status.trumpsuit)) return pair<bool,card>(true,cardtoplay);
        }
        return pair<bool,card>(true,playablecards[0]); //should never get here
    }
    pair<bool,card> playworstcard(const char &ledsuit, const char &trumpsuit)
    {
        card worstcard(0,0);
        for(auto card: playablecards)
        {
            if(worstcard.getvalue() == 0) worstcard = card;
            else if(card.gethierarchy(trumpsuit) < worstcard.gethierarchy(trumpsuit)) worstcard = card;
        }
        return pair<bool,card>(true,worstcard);
    }
    pair<bool,card> playmostgame(const char &ledsuit, const char &trumpsuit, card winningcard)
    {
        card bestcard = playworstcard(ledsuit, trumpsuit).second;
        for(int i = 1; i < (int)playablecards.size(); ++i) if(playablecards[i].getgame() >= bestcard.getgame() && winningcard.gethierarchy(trumpsuit) > playablecards[i].gethierarchy(trumpsuit)) bestcard = playablecards[i];
        return pair<bool,card>(true,bestcard);
    }
    card findlowestcardthatcanbeat(const char &trumpsuit, card &currenthighest)
    {
        card lowestthatcanbeat(0,-1);
        for(auto card: playablecards)
        {
            if(card.gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit))
            {
                if(lowestthatcanbeat.getsuit() == 0) lowestthatcanbeat = card;
                else if (card.gethierarchy(trumpsuit) < lowestthatcanbeat.gethierarchy(trumpsuit)) lowestthatcanbeat = card;
            }
        }
        return lowestthatcanbeat;
    }
    void printhand()
    {
        cout << "Hand: ";
        for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
        cout << endl;
    }
    void binaryoutput(card cardplayed)
    {
        int size = (int)hand.size();
        for(int i = 0; i < size; ++i) cout << (hand[i] == cardplayed) << " ";
        for(int i = 0; i < (6-size); ++i) cout << 0 << " ";
        cout << endl;
    }
private:
    vector<card> playablecards;
};

class learner : public player
{
    learner()
    {
        initialize();
    }
    bool bet(status &status, const int &index)
    {
        printhand();
        cout << "Player " << whichplayer << ": Trainer" << endl;
        vector<int> suits = {0,0,0,0};
        for(auto card: hand)
        {
            if(card.getvalue() == 14) bethelper(card.getsuit(), 10, suits);
            if(card.getvalue() == 13) bethelper(card.getsuit(), 9, suits);
            if(card.getvalue() == 12) bethelper(card.getsuit(), 6, suits);
            if(card.getvalue() == 11) bethelper(card.getsuit(), 9, suits);
            if(card.getvalue() == 10) bethelper(card.getsuit(), 4, suits);
            if(card.getvalue() == 3) bethelper(card.getsuit(), 4, suits);
            if(card.getvalue() == 2) bethelper(card.getsuit(), 7, suits);
            else bethelper(card.getsuit(), 1, suits);
        }
        int maxstrength = 0, maxindex = 0;
        for(int i = 0; i < 3; ++i) if(suits[i] > maxstrength)
        {
            maxstrength = suits[i];
            maxindex = i;
        }
        if(maxstrength > 42 && status.bet < 4)
        {
            status.bet = 4;
            status.whobet = whichplayer;
            settrumpsuit(maxindex,status);
            cout << "Bet 4 with strength " << maxstrength << endl << endl;
            return true;
        }
        if(maxstrength > 32 && status.bet < 3)
        {
            status.bet = 3;
            status.whobet = whichplayer;
            settrumpsuit(maxindex,status);
            cout << "Bet 3 with strength " << maxstrength << endl << endl;
            return true;
        }
        if(maxstrength > 22 && status.bet < 2)
        {
            status.bet = 2;
            status.whobet = whichplayer;
            settrumpsuit(maxindex,status);
            cout << "Bet 2 with strength " << maxstrength << endl << endl;
            return true;
        }
        cout << "Bet 0 with strength " << maxstrength << endl << endl;
        return false;
    }
    void bethelper(int suit, int worth, vector<int> &suits)
    {
        int suitindex;
        if(suit == 's') suitindex = 3;
        else if(suit == 'c') suitindex = 2;
        else suitindex = (suit == 'h');
        if(worth == 1) suits[suitindex] += 1;
        else
        {
            for(int i = 0; i < 3; ++i)
            {
                if(i == suitindex) suits[i] += worth;
                else suits[i] += worth*0.5;
            }
        }
    }
    void settrumpsuit(int suit, status &status)
    {
        if(suit == 3) status.trumpsuit = 's';
        if(suit == 2) status.trumpsuit = 'c';
        if(suit == 1) status.trumpsuit = 'h';
        if(suit == 0) status.trumpsuit = 'd';
    }
    void forcebet(status &status)
    {
        printhand();
        cout << whichplayer << endl;
        cout << "Forced to bet" << endl << endl;
        status.bet = 2;
        status.whobet = whichplayer;
        int whichsuit[4] = {0,0,0,0};
        for(auto card:hand)
        {
            if(card.getvalue() < 4) whichsuit[forcebethelper(card)] += 6;
            else if(card.getvalue() > 13) whichsuit[forcebethelper(card)] += 12;
            else if(card.getvalue() > 11) whichsuit[forcebethelper(card)] += 8;
            else if(card.getvalue() == 11) whichsuit[forcebethelper(card)] += 6;
            else if(card.getvalue() == 10) whichsuit[forcebethelper(card)] += 4;
        }
        int max = 0, index = 0;
        for(int i = 0; i < 4; ++i)
        {
            if(whichsuit[i] > max)
            {
                index = i;
                max = whichsuit[i];
            }
        }
        if(index == 0) status.trumpsuit = 's';
        if(index == 1) status.trumpsuit = 'c';
        if(index == 2) status.trumpsuit = 'h';
        if(index == 3) status.trumpsuit = 'd';
    }
    int forcebethelper(card helper)
    {
        if(helper.getsuit() == 's') return 0;
        if(helper.getsuit() == 'c') return 1;
        if(helper.getsuit() == 'h') return 2;
        return 3;
    }
    void initialize()
    {
        double weight;
        ifstream weightinput("weights.txt");
        for(int i = 0; i < 80; ++i)
        {
            layer1weights.push_back(vector<double>(120));
            for(int j = 0; j < 120; ++j)
            {
                weightinput >> weight;
                layer1weights[i].push_back(weight);
            }
        }
        for(int i = 0; i < 6; ++i)
        {
            layer2weights.push_back(vector<double>(120));
            for(int j = 0; j < 80; ++j)
            {
                weightinput >> weight;
                layer2weights[i].push_back(weight);
            }
        }
    }
    void playcard(status &status, bool first)
    {
        vector<double> output;
        hiddenlayer.resize(80);
        for(int i = 0; i < 80; ++i)
        {
            hiddenlayer[i] = 0;
            for(int j = 0; j < 120; ++j) hiddenlayer[i] += inputneurons[j] * layer1weights[i][j];
        }
        //activation function for hidden layer neurons
        for(int i = 0; i < 80; ++i) hiddenlayer[i] = 1 / (1 + pow(2.71828182846, -1 * hiddenlayer[i]));
        for(int i = 0; i < 6; ++i)
        {
            outputlayer[i] = 0;
            for(int j = 0; j < 80; ++j) outputlayer[i] += hiddenlayer[j] * layer2weights[i][j];
        }
        //output is weights in doubles
        for(int i = 0; i < 6; ++i) outputlayer[i] = 1 / (1 + pow(2.71828182846, -1 * outputlayer[i]));
        int choice = findmaxposition(outputlayer);
        //housekeeping
        hand[choice].whoplayed = whichplayer;
        if(first) status.ledsuit = hand[choice].getsuit();
        status.trick.push_back(hand[choice]);
        if(choice != hand.size() - 1) swap(*(hand.begin() + choice),hand.back());
        hand.pop_back();
    }
    int findmaxposition(vector<double> outputlayer)
    {
        int max = 0;
        for(int i = 0; i < 6; ++i) if(outputlayer[i] >= outputlayer[max]) max = i;
        return max;
    }
    void backpropagate(vector<double> outputlayer)
    {
        vector<double> targetoutput(6);
        for(int i = 0; i < 6; ++i) cin >> targetoutput[i];
        //compute layer 2
        for(int i = 0; i < 6; i++)
        {
            double constant = 0.05*(targetoutput[i]-outputlayer[i])*outputlayer[i]*(1-outputlayer[i]);
            for(int j = 0; j < 80; ++j) layer2weights[i][j] = constant*hiddenlayer[j];
        }
        //compute layer 1
        for(int i = 0; i < 80; ++i)
        {
            double constant = 0.05;
            for(int j = 0; j < 6; ++j)
            {
                constant *= (targetoutput[j]-outputlayer[j])*(1-outputlayer[j])*(outputlayer[j]);
                layer1weights[i][j] += constant;
            }
        }
    }
    void printhand()
    {
        cout << "Hand: ";
        for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
        cout << endl;
    }
private:
    vector<vector<double>> layer1weights; //120 rows, 80 columns
    vector<vector<double>> layer2weights; //80 rows, 6 columns
    vector<int> inputneurons;//first 40 = hand, second 40 = trick, third 40 = collected;
    vector<double> hiddenlayer;
    vector<double> outputlayer;
};

class human : public player
{
    void playcard(status &status, bool first)
    {
        cout << "Player " << whichplayer << ": You" << endl;
        printhand();
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
        cout << "Player " << whichplayer << ": Human" << endl;
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
    void forcebet(status &status)
    {
        status.bet = 2;
        status.whobet = whichplayer;
    }
    void printhand()
    {
        cout << "Hand: ";
        for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
        cout << endl;
    }
};

void setup(vector<player*> &players, deck &carddeck, status &status);
void dealcards(vector<player*> &players, const deck &carddeck, status &status);
void makebets(const vector<player*> &players, status &status);
void playtrick(const vector<player*> &players, status &status);

int main(int argc, const char * argv[])
{
    ofstream weightoutput("output.txt");
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
            for(int round = 0; round < 6; ++round) playtrick(players, status);
            status.resethand();
        }
        if(status.overallscore[0] < 11) cout << "You Lose: Team 2 Wins" << endl;
        else cout << "You win!! Congrats" << endl;
    }
    remove("weights.txt");
    rename("output.txt","weights.txt");
    return 0;
}

void setup(vector<player*> &players, deck &carddeck, status &status)
{
    carddeck.makedeck();
    players.push_back(new trainer());
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
    if(status.bet == 0) players[currentbetter]->forcebet(status);
    else players[currentbetter]->bet(status,currentbetter);
    status.wholeads = status.whobet;
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
    cout << endl << endl;
    cout << flush;
}
