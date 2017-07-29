/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "BooleanFeatures.h"
#include "Document.h"
#include "Context.h"
using namespace std;
using namespace rapidxml;

bool And_Feature::evaluateInner(Document* d, Context& context)
{
    if (findAllFeatures(d,context)<m_features.size()) return false;
    return true;
}

bool Or_Feature::evaluateInner(Document* d, Context& context)
{
    if (findAllFeatures(d,context)==0) return false;
    return true;
}

//
// not sure what to do here.
// implemented a Not-OR.
bool Not_Feature::evaluateInner(Document* d, Context& context)
{
    return findAllFeatures(d,context)==0;
}