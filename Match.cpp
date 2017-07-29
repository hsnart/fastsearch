/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "Match.h"
//class MatchList : public StreamMatchCallback
void MatchList::addMatch(const Match& m)
{
    m_matches.push_back(m);
}
void MatchList::optimise()
{
    std::sort(m_matches.begin(), m_matches.end());
}
bool MatchList::contains(const Match& m)const
{
    for (int i=0 ;i<m_matches.size(); i++)
    {
        if (m_matches[i].contains(m)) return true;
    }
    return false;
}
