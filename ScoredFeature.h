/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__ScoredFeature__
#define __textsearchtest__ScoredFeature__

#include <iostream>
#include "GroupFeature.h"

class Scored_Feature : public Group_Feature
{
private:
    // scoring and threshold tests.
    int m_scorePerMatch = 1;
    int m_minScore = 0;
    std::vector<Scored_Feature*> m_sections;
protected:
    virtual bool addAttribute(rapidxml::xml_attribute<>* xml);
    virtual bool addNode(rapidxml::xml_node<>* xml);
    bool evaluateScore(Document* d, Context& context, int& score);
    virtual bool evaluateInner(Document* d, Context& context);
public:
    virtual ~Scored_Feature();
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};


#endif /* defined(__textsearchtest__ScoredFeature__) */
