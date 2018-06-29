//
//  human.h
//  smear
//
//  Created by Ethan Poortenga on 3/6/18.
//  Copyright © 2018 Ethan Poortenga. All rights reserved.
//

#ifndef human_h
#define human_h

#include <stdio.h>

#include "player.h"

class human : public player
{
    public:
        void playcard(status &status, bool first, const bool &forgenerating);
        bool bet(status &status, const int &index, const bool &forgenerating);
        void forcebet(status &status);
        void printhand();
};

#endif /* human_h */
