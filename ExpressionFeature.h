/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__ExpressionFeature__
#define __textsearchtest__ExpressionFeature__

#include <iostream>
#include "FS_Search.h"
#include "Feature.h"

// wraps a single expression in a feature.
// need ExpressionList_Feature too, so that we can be more efficient.
class Expression_Feature : public Feature
{
private:
    FS_Expression m_e;
    bool findExprInDocument(Document* d, Context& context, int recursionCount);
public:
    Expression_Feature(const char* exp);
    virtual ~Expression_Feature();
    virtual bool evaluate(Document* d, Context& context);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
};

#endif /* defined(__textsearchtest__ExpressionFeature__) */
