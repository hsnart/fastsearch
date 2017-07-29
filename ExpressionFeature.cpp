/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "ExpressionFeature.h"
#include "Document.h"
#include "TokenStream.h"
using namespace std;
using namespace rapidxml;


// class Expression_Feature : public Feature

Expression_Feature::Expression_Feature(const char* exp)
: m_e(exp){}
Expression_Feature::~Expression_Feature(){}
bool Expression_Feature::evaluate(Document* d, Context& context)
{
    return findExprInDocument(d,context,0);
}

class Expression_Feature_callback : public FS_Expression::MatchCallback
{
private:
    Feature* feature;
    Context* context;
public:
    Expression_Feature_callback(Feature* f, Context* c):feature(f), context(c){}
    DocumentStream* stream;
    Document* document;
    virtual bool match(const char* matchStart, const char* matchEnd)
    {
        return context->addMatch(feature,document,stream, Match(matchStart,matchEnd));
    };
};

bool Expression_Feature::findExprInDocument(Document* d, Context& context, int recursionCount)
{
    bool found = false;
    Expression_Feature_callback callback(this,&context);
    callback.document = d;
    // search the streams
    for(int k=0; k<d->numStreams(); k++)
    {
        DocumentStream* s =d->getStream(k);
        if (context.searchStream(s))// ok to search this stream?
        {
            callback.stream = s;
            int tc = m_e.count(*(s->getTokenStream()),&callback);
            if (tc>0)
            {
                found = true;
            }
        }
    }
    
    //
    // control sub doc recursion by location set on context.
    for (int j=0;j<d->numSubDocs(); j++)
    {
        Document* subDoc =d->getSubDoc(j);
        if (context.searchDoc(subDoc, recursionCount))// ok to search this doc?
        {
            if (findExprInDocument(subDoc,context,recursionCount+1))
            {
                found = true;
            }
        }
    }
    
    return found;
}
