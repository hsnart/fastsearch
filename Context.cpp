/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "Context.h"
#include "Document.h"
#include "Feature.h"
#include "ExpressionFeature.h"
#include "GroupFeature.h"
#include "SpacingFeature.h"

//class StreamMatches

void StreamMatches::addMatch(const Match& m){   m_matches.push_back(m);}
size_t StreamMatches::size()const{              return m_matches.size();}
Match& StreamMatches::get(unsigned i){          return m_matches[i];}
//class DocumentEvidence

void DocumentEvidence::addMatch(DocumentStream* s, const Match& m)
{
    m_matches[s].addMatch(m);
    m_numMatches++;
}
int DocumentEvidence::numMatches()const{        return m_numMatches;}
int DocumentEvidence::numMatches(DocumentStream* s)const
{
    std::map<DocumentStream*, StreamMatches>::const_iterator it = m_matches.find(s);
    if (it != m_matches.end()) return it->second.size();
    return 0;
}
StreamMatches* DocumentEvidence::getStreamMatches(DocumentStream* s)
{
    std::map<DocumentStream*, StreamMatches>::iterator it = m_matches.find(s);
    if (it != m_matches.end()) return (&(it->second));
    return NULL;
}
void DocumentEvidence::setScore(int i){         m_score = i;}


//class FeatureMatches
FeatureMatches::~FeatureMatches(){}
void FeatureMatches::addMatch(Document* d, DocumentStream* s, const Match& m)
{
    m_evidence[d].addMatch(s,m);
    m_size++;
}
int FeatureMatches::numMatches(Document* d)const
{
    std::map<Document*, DocumentEvidence>::const_iterator it = m_evidence.find(d);
    if (it != m_evidence.end())
    {
        return it->second.numMatches();
    }
    return 0;
}
int FeatureMatches::numMatches(Document* d, DocumentStream* s)const
{
    std::map<Document*, DocumentEvidence>::const_iterator it = m_evidence.find(d);
    if (it != m_evidence.end())
    {
        return it->second.numMatches(s);
    }
    return 0;
}
StreamMatches* FeatureMatches::getStreamMatches(Document* d, DocumentStream* s)
{
    std::map<Document*, DocumentEvidence>::iterator it = m_evidence.find(d);
    if (it != m_evidence.end())
    {
        return it->second.getStreamMatches(s);
    }
    return NULL;
}

//
//
//
//class Context
void Context::pushScope(Scope* s){          m_scopes.push_back(s);}
void Context::popScope(){                   m_scopes.pop_back();}
bool Context::searchDoc(Document* d, int recursionLevel)
{
    for (int i=0; i<m_scopes.size(); i++)
    {
        if (m_scopes[i]->recurseIntoDoc(d,recursionLevel) == false) return false;
    }
    return true;
}
bool Context::searchStream(DocumentStream* s)
{
    for (int i=0; i<m_scopes.size(); i++)
    {
        if (m_scopes[i]->searchStream(s) == false) return false;
    }
    return true;
}
int Context::getNumMatches(Feature* f,Document* d)
{
    GetMatchCountVisitor v(m_results,d);
    f->accept(v);
    return v.count;
}

class MergeChildMatchesForStreamVisitor : public FeatureVisitor
{
private:
    Results& results;
    Document* document;
    DocumentStream* stream;
    std::vector<FeatureMatch>& allMatches;
public:
    MergeChildMatchesForStreamVisitor(Results& r, Document* d, DocumentStream* s, std::vector<FeatureMatch>& m)
    :results(r),document(d),stream(s), allMatches(m){}
    virtual void visit(Expression_Feature& f){     addMatches(f);}
    virtual void visit(Group_Feature& f){          f.recurse(*this);}
    virtual void visit(Spacing_Feature& f){        addMatches(f);}
    
    void addMatches(Feature& f)
    {
        StreamMatches* matches = results.getMatches(&f, document, stream);
        if (matches)
        {
            for (int j=0; j<matches->size(); j++)
            {
                FeatureMatch fm(matches->get(j),&f);
                allMatches.push_back(fm);
            }
        }
    }
};

class MergeMatchesVisitor : public FeatureVisitor
{
private:
    Results& results;
    StreamMatchCallback* m_callback;
    void addMatches(Feature& f)
    {
        const FeatureMatches* fm = results.getFeatureMatches(&f);
        if (fm)
        {
            fm->dumpMatches(&f,m_callback);
        }
    }
public:
    MergeMatchesVisitor(Results& r, StreamMatchCallback* callback)
    :results(r),m_callback(callback){}
    virtual void visit(Expression_Feature& f){      addMatches(f);}
    virtual void visit(Group_Feature& f)    {       f.recurse(*this);}
    virtual void visit(Spacing_Feature& f)  {       addMatches(f);}
};

void Context::mergeChildMatchesForStream(Document* d, DocumentStream* s, Feature* parent, std::vector<FeatureMatch>& allMatches)
{
    MergeChildMatchesForStreamVisitor v(m_results,d,s,allMatches);
    parent->recurse(v);
    std::sort(allMatches.begin(), allMatches.end());
}

void Context::mergeMatches(Feature* f, StreamMatchCallback* callback)
{
    MergeMatchesVisitor v(m_results,callback);
    f->accept(v);
}


bool Context::filterMatch(Document* d, DocumentStream* s, const Match& m)
{
    if (m_whereFilter.size()>0)
    {
        for (int i=0; i<m_whereFilter.size(); i++)
        {
            if (m_whereFilter[i]->contains(m) ==false) return false;
        }
    }
    return true;
}

void Context::pushWhereFilter(MatchFilter* fm){         m_whereFilter.push_back(fm);}
void Context::popWhereFilter(){                         m_whereFilter.pop_back();}




//class MatchFilter
class MatchListBuilder : public StreamMatchCallback
{
private:
    MatchList& ml;
public:
    MatchListBuilder(MatchList& l):ml(l){}
    virtual void addMatch(Feature* f, DocumentStream* s, const char* start, const char* end)
    {
        ml.addMatch(Match(start,end));
    }
};
MatchFilter::MatchFilter(Feature* f, Context& context):m_context(context)
{
    MatchListBuilder builder(m_matches);
    m_context.mergeMatches(f, &builder);
    m_matches.optimise();
    m_context.pushWhereFilter(this);
}
MatchFilter::~MatchFilter()
{
    m_context.popWhereFilter();
}

bool MatchFilter::contains(const Match& m)const
{
    return m_matches.contains(m);
};
