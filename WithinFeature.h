/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__WhereFeature__
#define __textsearchtest__WhereFeature__

#include <iostream>

#include "GroupFeature.h"

class Within_Feature : public Group_Feature
{
private:
    Feature* m_location;
protected:
    virtual bool addNode(rapidxml::xml_node<>* xml);
   /* bool pruneMatches(Document* d, int& count, Results& results);
    bool pruneMatchesInStream(DocumentStream* s, int& count, Results& results);
    bool pruneMatchesInStreamForFeature(FeatureMatches* locationMatches, Feature* f, DocumentStream* s, int& count, Results& results);*/
public:
    virtual bool evaluate(Document* d, Context& context);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};


#endif /* defined(__textsearchtest__WhereFeature__) */
