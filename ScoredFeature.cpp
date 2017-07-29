/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "ScoredFeature.h"
#include "Document.h"
#include "FS_Utils.h"
using namespace std;
using namespace rapidxml;
//
// Scored_Feature
Scored_Feature::~Scored_Feature()
{
    clearPtrVector(m_sections);
}
bool Scored_Feature::addAttribute(rapidxml::xml_attribute<>* a)
{
    if (strcasecmp(a->name(),"minscore")==0)
    {
        m_minScore = atoi(a->value()); return true;
    }
    if (strcasecmp(a->name(),"scorePerMatch")==0)
    {
        m_scorePerMatch = atoi(a->value()); return true;
    }
    return Group_Feature::addAttribute(a);
}
bool Scored_Feature::addNode(rapidxml::xml_node<>* xml)
{
    if (strcasecmp(xml->name(),"section")==0)
    {
        m_sections.push_back(CreateFeature<Scored_Feature>(xml)); return true;
    }
    return Group_Feature::addNode(xml);
}
bool Scored_Feature::evaluateInner(Document* d, Context& context)
{
    int score = 0;
    return evaluateScore(d,context,score);
}

bool Scored_Feature::evaluateScore(Document* d, Context& context, int& score)
{
    findAllFeatures(d,context);
    int tscore = 0;
    for (int i=0; i<m_sections.size(); i++)
    {
        m_sections[i]->evaluateScore(d,context,tscore);
    }
    
    // score tests
    tscore += context.getNumMatches(this,d)*m_scorePerMatch;
    
    if (tscore>=m_minScore)
    {
        score += tscore;
    //    context.setScore(this,d,score);
        cout<<"feature::"<<m_name<<" fired with score:"<<score<<" on document:"<<d->getName()<<endl;
        return true;
    }
    
    return false;
};


