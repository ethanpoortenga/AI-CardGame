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
        for(int i = 0; i < 7; ++i)
        {
            srand(int(time(NULL)));
            for(int i = 39; i > 1; --i)
            {
                int random_number = std::rand(); // rand() return a number between ​0​ and RAND_MAX
                random_number = random_number % i;
                if(i != (40-i)) swap(carddeck[i], carddeck[40-i]);
            }
            swap(carddeck[0], carddeck[20]);
        }
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
        ++handpoints[findlowestcard()];
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
        cout << "Team 0 won " << handpoints[0] << ", Team 1 won " << handpoints[1] << endl;
        cout << "Current Score: Team 0: " << points[0] << ", Team 1: " << points[1] << endl << endl;
    }
    void resetgame()
    {
        overallscore[0] = 0, overallscore[1] = 0;
    }
    card findtrickwinner() //return 0 = team1, 1 = team2
    {
        card currenthighest(0,0);
        for(int i = 0; i < 4; ++i) if(trick[i].gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit))currenthighest = trick[i];
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
    int findgame()        //return 0 = team1, 1 = team2
    {
        return(points[0] < points[1]);
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
        printhand();
        cout << whichplayer << endl;
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
        playablecards.clear();
        for(auto card:hand) if(card.getsuit() == status.trumpsuit || card.getsuit() == status.ledsuit) playablecards.push_back(card);
        if(playablecards.size() == 0) playablecards = hand;
        pair<bool,card> result(false,card(0,0));
        //if first player lead out (different logic)
        if(first)
        {
            leadout(status);
            return;
        }
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
    void playcardhelper(pair<bool,card> &result, status &status)
    {
        result.second.whoplayed = whichplayer;
        cout << "Card Played: " << char(result.second.getsuit() - 32) << " " << result.second.getvalue() << endl;
        status.trick.push_back(result.second);
        for(auto card: hand) if(card.gethierarchy(status))
    }
    void leadout(status &status)
    {
        pair<bool,card> choice(true,card(0,0));
        choice.first = true;
        //if it's the first play of the hand
        if(status.cardsplayed.size() == 0)
        {
            card greaterthanjack(status.trumpsuit,11), lessthanten(status.trumpsuit,10);
            int greaterindex = -1, lessindex = -1;
            for(int i = 0; i < 6; ++i)
            {
                if(hand[i].gethierarchy(status.trumpsuit) > greaterthanjack.gethierarchy(status.trumpsuit))
                {
                    greaterthanjack = hand[i];
                    greaterindex = i;
                }
                if(hand[i].gethierarchy(status.trumpsuit) < lessthanten.gethierarchy(status.trumpsuit) && hand[i].gethierarchy(status.trumpsuit) > 15)
                {
                    lessthanten = hand[i];
                    lessindex = i;
                }
            }
            if(greaterindex != -1)
            {
                choice.second = greaterindex;
                status.ledsuit = status.trumpsuit;
                playcardhelper(choice, status);
                return;
            }
            if(lessindex != -1)
            {
                choice.second = lessindex;
                status.ledsuit = status.trumpsuit;
                playcardhelper(choice, status);
                return;
            }
            else
            {
                for(int i = 0; i < 6; ++i) if(hand[i].getsuit() == status.trumpsuit)
                {
                    choice.second = i;
                    status.ledsuit = status.trumpsuit;
                    playcardhelper(choice, status);
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
            int hightrumpsuit = -1, highothersuit = -1, lowtrumpsuit = -1, anyothersuit = -1;
            for(int i = 0; i < (int)hand.size(); ++i)
            {
                if(hand[i].gethierarchy(trumpsuit) > 25)
                {
                    if(hightrumpsuit == -1) hightrumpsuit = i;
                    else if(hand[hightrumpsuit].gethierarchy(trumpsuit) < hand[i].gethierarchy(trumpsuit)) hightrumpsuit = i;
                }
                else if(hand[i].gethierarchy(trumpsuit) < 24 && hand[i].gethierarchy(trumpsuit) > 15)
                {
                    if(lowtrumpsuit == -1) lowtrumpsuit = i;
                    else if(hand[lowtrumpsuit].gethierarchy(trumpsuit) > hand[i].gethierarchy(trumpsuit)) lowtrumpsuit = i;
                }
                else if(hand[i].gethierarchy(trumpsuit) > 10)
                {
                    if(highothersuit == -1) highothersuit = i;
                    else if(hand[highothersuit].gethierarchy(trumpsuit) < hand[i].gethierarchy(trumpsuit)) highothersuit = i;
                }
                else if(hand[i].getvalue() < 10)
                {
                    anyothersuit = i;
                }
            }
            if(hightrumpsuit != -1)
            {
                choice.second = hightrumpsuit;
                status.ledsuit = hand[hightrumpsuit].getsuit();
                playcardhelper(choice, status);
                return;
            }
            if(lowtrumpsuit != -1)
            {
                choice.second = lowtrumpsuit;
                status.ledsuit = hand[lowtrumpsuit].getsuit();
                playcardhelper(choice, status);
                return;
            }
            if(highothersuit != -1)
            {
                choice.second = highothersuit;
                status.ledsuit = hand[highothersuit].getsuit();
                playcardhelper(choice, status);
                return;
            }
            if(anyothersuit != -1)
            {
                choice.second = anyothersuit;
                status.ledsuit = hand[anyothersuit].getsuit();
                playcardhelper(choice, status);
                return;
            }
            choice.second = 0;
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
                if(status.nummovestaken == 3) return playmostgame(status.ledsuit, trumpsuit);
                for(int i = 3; i >=0; --i)
                {
                    if(cardsjackandabove[i] == 0)
                    {
                        if(couldtakein == -1) return pair<bool,card>(false,card(0,0));
                        else return pair<bool,card>(true,card(trumpsuit,i+11));
                    }
                    if(cardsjackandabove[i] == 2) couldtakein = i;
                    else if(cardsjackandabove[i] == 3) return playmostgame(status.ledsuit, trumpsuit);
                }
            }
            else
            {
                for(int i = 3; i >=0; --i)
                {
                    if(cardsjackandabove[i] == 0) return pair<bool,card>(false,card(0,0));
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
        bool tentrumpsuit = false, tenledsuit = true;
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
                if(status.nummovestaken == 3) return playmostgame(status.ledsuit, status.trumpsuit);
                if(gameintrick >= 10)
                {
                    card currentmax(0,0);
                    for(auto card: playablecards) if(card.gethierarchy(status.trumpsuit) > currentmax.gethierarchy(status.trumpsuit)) currentmax = card;
                    if(currentmax.gethierarchy(status.trumpsuit) > winningtrick.gethierarchy(status.trumpsuit)) return pair<bool,card>(true,currentmax);
                }
                if(gameintrick < 10)
                {
                    cardtoplay = findlowestcardthatcanbeat(status.trumpsuit, status.trick[index]);
                    if(cardtoplay != -1) return pair<bool,card>(true,cardtoplay);
                }
                return pair<bool,card>(false,0);
            }
            //other team winning
            else
            {
                if(status.nummovestaken == 3)
                {
                    int cardtoplay = findlowestcardthatcanbeat(status.trumpsuit, status.trick[index]);
                    if(cardtoplay != -1) return pair<bool,card>(true,cardtoplay);
                    return pair<bool,card>(false,0);
                }
                int cardtoplay = 0;
                if(gameintrick > 10)
                {
                    for(int i = 0; i < (int)hand.size(); ++i) if(hand[i].gethierarchy(status.trumpsuit) > status.trick[index].gethierarchy(status.trumpsuit) && hand[i].gethierarchy(status.trumpsuit) >= hand[cardtoplay].gethierarchy(status.trumpsuit) && !(!status.jackcollected && hand[i].gethierarchy(status.trumpsuit) > 24)) cardtoplay = i;
                    if(hand[0].gethierarchy(status.trumpsuit) > status.trick[index].gethierarchy(status.trumpsuit)) return pair<bool,card>(true,cardtoplay);
                }
                if(gameintrick < 10)
                {
                    cardtoplay = findlowestcardthatcanbeat(status.trumpsuit, status.trick[index]);
                    if(cardtoplay != -1) return pair<bool,card>(true,cardtoplay);
                }
                return pair<bool,card>(false,0);
            }
        }
        //no substantial game
        else return pair<bool,card>(false,0);
    }
    pair<bool,card> proirity3(status &status) //taking in the trick
    {
        //find which card is currently winning the trick
        int index = 0;
        for(int i = 0; i < status.nummovestaken; ++i) if(status.trick[i].gethierarchy(status.trumpsuit)>=status.trick[index].gethierarchy(status.trumpsuit)) index = i;
        //see whether I can win
        bool canwin = false;
        for(auto card: hand)
        {
            if(card.gethierarchy(status.trumpsuit) > status.trick[index].gethierarchy(status.trumpsuit) && card.gethierarchy(status.trumpsuit) < 24)
            {
                canwin = true;
                break;
            }
        }
        if(!canwin) return playworstcard(status.ledsuit, status.trumpsuit);
        else
        {
            if(status.nummovestaken == 3) return playworstcard(status.ledsuit, status.trumpsuit);
            int cardtoplay = 0;
            for(int i = 0; i < (int)hand.size(); ++i) if(hand[i].gethierarchy(status.trumpsuit) > status.trick[index].gethierarchy(status.trumpsuit) && hand[i].gethierarchy(status.trumpsuit) >= hand[cardtoplay].gethierarchy(status.trumpsuit) && !(!status.jackcollected && hand[i].gethierarchy(status.trumpsuit) > 24)) cardtoplay = i;
            if(hand[0].gethierarchy(status.trumpsuit) > status.trick[index].gethierarchy(status.trumpsuit)) return pair<bool,card>(true,cardtoplay);
        }
        return pair<bool,card>(true,-1); //should never get here
    }
    pair<bool,card> playworstcard(const char &ledsuit, const char &trumpsuit)
    {
        int worstcard = 0;
        for(int i = 0; i < (int)hand.size(); ++i)
        {
            if(hand[i].getsuit() == ledsuit && hand[worstcard].getsuit() != ledsuit) worstcard = i;
            else if (hand[i].getsuit() == ledsuit && hand[worstcard].getsuit() == ledsuit && hand[i].getgame() <= hand[worstcard].getgame()) worstcard = i;
            else if (hand[i].getgame() <= hand[worstcard].getgame() && hand[worstcard].getsuit() != ledsuit) worstcard = i;
        }
        return pair<bool,card>(true,worstcard);
    }
    pair<bool,card> playmostgame(const char &ledsuit, const char &trumpsuit)
    {
        int bestcard = 0;
        bool changed = false;
        for(int i = 0; i < (int)hand.size(); ++i)
        {
            if(hand[i].getgame() >= hand[bestcard].getgame() && (hand[i].getsuit() == ledsuit || hand[i].getsuit() == trumpsuit))
            {
                bestcard = i;
                changed = true;
            }
            else if(!changed && hand[i].getsuit() == ledsuit)
            {
                bestcard = i;
                changed = true;
            }
        }
        return pair<bool,card>(true,bestcard);
    }
    int findlowestcardthatcanbeat(const char &trumpsuit, card &currenthighest)
    {
        int lowestthatcanbeat = 0;
        card cardtoplay(0,15);
        for(int i = 0; i < (int)hand.size(); ++i)
        {
            if(hand[lowestthatcanbeat].gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit) && hand[lowestthatcanbeat].gethierarchy(trumpsuit) < cardtoplay.gethierarchy(trumpsuit))
            {
                lowestthatcanbeat = i;
                cardtoplay = hand[i];
            }
        }
        if(cardtoplay.getvalue() == 15) return -1;
        return lowestthatcanbeat;
    }
    void printhand()
    {
        cout << "Hand: ";
        for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
        cout << endl;
    }
private:
    vector<card> playablecards;
};

class learner : public player
{
    bool bet(status &status, const int &index)
    {
        status.bet = 2;
        status.whobet = index;
        return true;
    }
    void forcebet(status &status)
    {
        status.bet = 2;
        status.whobet = whichplayer;
    }
    void playcard(status &status, bool first)
    {
        hand.back().whoplayed = whichplayer;
        status.trick.push_back(hand.back());
        hand.pop_back();
    }
    vector<double> getcardchoices()
    {
        
    }
    void printhand()
    {
        cout << "Hand: ";
        for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
        cout << endl;
    }
private:
    vector<vector<double>> layer1weights;
    vector<vector<double>> layer2weights;
    vector<int> inputneurons;//first 40 = hand, second 40 = trick, third 40 = collected;
    vector<int> hiddenlayer;
    vector<int> outputlayer;
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
    deck carddeck;
    vector<player*> players;
    status status;
    setup(players, carddeck, status);
    while(status.overallscore[0] < 11 && status.overallscore[1] < 11)
    {
        dealcards(players, carddeck, status);
        makebets(players, status);
        for(int round = 0; round < 6; ++round) playtrick(players, status);
        status.resethand();
    }
    if(status.overallscore[0] < 11) cout << "You Lose: Team 2 Wins" << endl;
    else cout << "You win!! Congrats" << endl;
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
    cout << "continue?" << endl;
    cin >> currplayer;
    cout << endl << endl;
    cout << flush;
}
