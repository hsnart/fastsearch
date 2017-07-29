/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__BooleanFeatures__
#define __textsearchtest__BooleanFeatures__

#include <iostream>
#include "GroupFeature.h"

class And_Feature : public Group_Feature
{
protected:
    virtual bool evaluateInner(Document* d, Context& context);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};

class Or_Feature : public Group_Feature
{
protected:
    virtual bool evaluateInner(Document* d, Context& context);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};

class Not_Feature : public Group_Feature
{
protected:
    virtual bool evaluateInner(Document* d, Context& context);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};

#endif /* defined(__textsearchtest__BooleanFeatures__) */
