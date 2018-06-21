//
//  rulesbased.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include <iostream>
#include <cassert>

#include "rulesbased.h"

void rulesbased::playcard(status &status, bool first, const bool &forgenerating)
{
    if(!forgenerating) cout << "Player " << whichplayer << ": Trainer" << endl;
    if(!forgenerating) printhand();
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

void rulesbased::playcardhelper(pair<bool,card> result, status &status)
{
    lastchoice.clear();
    assert(result.second.getsuit() != 0);
    result.second.whoplayed = whichplayer;
    ++status.nummovestaken;
    if(!forgenerating) cout <<"Card Played: "<<char(result.second.getsuit() - 32)<< " " << result.second.getvalue() << endl;
    status.trick.push_back(result.second);
    int handsize = (int)hand.size();
    for(int i = 0; i < handsize; ++i)
    {
        if(hand[i].getsuit()== result.second.getsuit() && hand[i].getvalue() == result.second.getvalue())
        {
            hand.erase(hand.begin() + i);
            lastchoice.push_back(1);
        }
        lastchoice.push_back(0);
    }
    for(int i = 0; i < 6 - handsize; ++i) lastchoice.push_back(0);
}
void rulesbased::leadout(status &status)
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
pair<bool,card> rulesbased::proirity1(status &status) //jack
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
pair<bool,card> rulesbased::proirity2(status &status) //game
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
pair<bool,card> rulesbased::proirity3(status &status) //taking in the trick
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
pair<bool,card> rulesbased::playworstcard(const char &ledsuit, const char &trumpsuit)
{
    card worstcard(0,0);
    for(auto card: playablecards)
    {
        if(worstcard.getvalue() == 0) worstcard = card;
        else if(card.gethierarchy(trumpsuit) < worstcard.gethierarchy(trumpsuit)) worstcard = card;
    }
    return pair<bool,card>(true,worstcard);
}
pair<bool,card> rulesbased::playmostgame(const char &ledsuit, const char &trumpsuit, card winningcard)
{
    card bestcard = playworstcard(ledsuit, trumpsuit).second;
    for(int i = 1; i < (int)playablecards.size(); ++i) if(playablecards[i].getgame() >= bestcard.getgame() && winningcard.gethierarchy(trumpsuit) > playablecards[i].gethierarchy(trumpsuit)) bestcard = playablecards[i];
    return pair<bool,card>(true,bestcard);
}
card rulesbased::findlowestcardthatcanbeat(const char &trumpsuit, card &currenthighest)
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
void rulesbased::printhand()
{
    cout << "Hand: ";
    for(auto card:hand) cout << char(card.getsuit() - 32) << " " << card.getvalue() << "   ";
    cout << endl;
}

vector<int> rulesbased::getchoice()
{
    return lastchoice;
}
