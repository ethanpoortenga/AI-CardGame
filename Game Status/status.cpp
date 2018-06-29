//
//  status.cpp
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#include <iostream>
#include <vector>

#include "status.h"

using namespace std;

void status::resettrick()
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
void status::resethand()
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
    points[0] = 0;
    points[1] = 0;
    bet = 0;
    cout << "Team A won " << handpoints[0] << ", Team B won " << handpoints[1] << endl;
    cout << "Current Score: Team A: " << overallscore[0] << ", Team B: " << overallscore[1] << endl << endl;
}
void status::resetgame()
{
    overallscore[0] = 0;
    overallscore[1] = 0;
}
card status::findtrickwinner() //return 0 = team1, 1 = team2
{
    card currenthighest(0,0);
    for(auto card : trick) if(card.gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit))currenthighest = card;
    return currenthighest;
}
int status::findhighestcard() //return 0 = team1, 1 = team2
{
    card currenthighest(0,0);
    for(auto card : cardsplayed) if(card.gethierarchy(trumpsuit) > currenthighest.gethierarchy(trumpsuit)) currenthighest = card;
    //return the team that won
    return (currenthighest.whoplayed % 2);
}
int status::findlowestcard()  //return 0 = team1, 1 = team2
{
    card currentlowest(0,15);
    for(auto card : cardsplayed) if(card.getsuit() == trumpsuit && card.getvalue() < currentlowest.getvalue()) currentlowest = card;
    return (currentlowest.whoplayed % 2);
}
int status::findjack()        //return 0 = team1, 1 = team2
{
    for(auto card:cardsplayed) if(card.getvalue() == 11 && card.getsuit() == trumpsuit) return (card.whotookin);
    return -1; //will never get here but for compile reasons
}
vector<int> status::getstatus(vector<card> hand)
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
