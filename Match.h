/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Match__
#define __textsearchtest__Match__

#include <iostream>
#include <vector>
class Feature;

class Match
{
public:
    const char* startPos;
    const char* endPos;
    Match(const char* start, const char* end):startPos(start),endPos(end){}
    Match(){};
    Match(const Match& right):startPos(right.startPos),endPos(right.endPos){}
    bool contains(const Match& right)const
    {
        return(startPos<=right.startPos && endPos>=right.endPos);
    }
    bool overlaps(const Match& right)const
    {
        if (startPos<right.startPos && endPos>right.endPos) return true;
        if (startPos>=right.startPos && startPos<=right.endPos) return true;
        if (endPos>=right.startPos && endPos<=right.endPos) return true;
        return false;
    }
    bool operator<(const Match& right)const    {return startPos<right.startPos;}
};

class FeatureMatch : public Match{
public:
    FeatureMatch(){};
    FeatureMatch(Match& m, Feature* f):Match(m),feature(f){}
    Feature* feature;
};

class MatchList
{
private:
    std::vector<Match> m_matches;
public:
    void addMatch(const Match& m);
    void optimise();
    bool contains(const Match& m)const;
};

#endif /* defined(__textsearchtest__Match__) */
