/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__ProximityFeature__
#define __textsearchtest__ProximityFeature__

#include <iostream>

#include "GroupFeature.h"

class Spacing_Feature : public Group_Feature
{
private:
    int m_maxSeparation=10;
    int m_minMatches = 0;
    int m_surround = 0;
    // proximity clauses
protected:
    virtual bool addAttribute(rapidxml::xml_attribute<>* a);
    //   virtual bool addNode(rapidxml::xml_node<>* xml);
protected:
    virtual bool evaluateInner(Document* d, Context& context);
    bool evaluateStream(Document* d, DocumentStream* s, Context& context);
    bool evaluateProximity(Document* d, Context& context);
public:
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};


class Sequence_Feature : public Group_Feature
{
private:
    int m_maxSeparation=10;
protected:
    virtual bool addAttribute(rapidxml::xml_attribute<>* a);
protected:
    virtual bool evaluateInner(Document* d, Context& context);
    bool evaluateStream(Document* d, DocumentStream* s, Context& context);
    bool evaluateProximity(Document* d, Context& context);
};
#endif /* defined(__textsearchtest__ProximityFeature__) */
