//
//  training.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef training_h
#define training_h

#include <stdio.h>
#include <fstream>

using namespace std;

class generatetrainingdata
{
public:
    generatetrainingdata();
    void startgenerating();
    void logbinarystatus(vector<int> binarystatus);
    void logcardchoice(vector<int> cardchoice);
private:
    ofstream dataoutput;
};

#endif /* training_h */
