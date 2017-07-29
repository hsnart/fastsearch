//
//  Timer.h
//  textsearchtest
//
//  Created by howard on 17/01/2014.
//  Copyright (c) 2014 howard. All rights reserved.
//

#ifndef __textsearchtest__Timer__
#define __textsearchtest__Timer__

#include <iostream>
class Timer
{
private:
    clock_t startTime;
    char* name;
public:
    Timer(char* n);
    ~Timer();
};
#endif /* defined(__textsearchtest__Timer__) */
