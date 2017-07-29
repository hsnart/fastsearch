//
//  Timer.cpp
//  textsearchtest
//
//  Created by howard on 17/01/2014.
//  Copyright (c) 2014 howard. All rights reserved.
//

#include "Timer.h"

//class Timer

Timer::Timer(char* n):name(n)
{
    startTime = clock();
}
Timer::~Timer()
{
    std::cout <<"Time for: "<<name<<":"<<float( clock () - startTime ) /  (CLOCKS_PER_SEC/1000)<<std::endl;
}
