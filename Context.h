/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Context__
#define __textsearchtest__Context__

#include <iostream>
#include <vector>
#include <map>
#include "Match.h"

class Document;
class DocumentStream;
class Feature;


class StreamMatchCallback
{
public:
    virtual ~StreamMatchCallback(){};
    virtual void addMatch(Feature* f, DocumentStream* s, const char* start, const char* end)=0;
};

// TODO: optimise the crap out of the following.
// all matches for a feature on a document on a stream
class StreamMatches
{
private:
    std::vector<Match> m_matches;
public:
    void addMatch(const Match& m);
    size_t size()const;
    Match& get(unsigned i);
    void dumpMatches(Feature* f, DocumentStream* s, StreamMatchCallback* callback)const
    {
        std::vector<Match>::const_iterator it = m_matches.begin();
        std::vector<Match>::const_iterator it_end = m_matches.end();
        for (; it != it_end; ++it)
        {
            callback->addMatch(f,s,it->startPos, it->endPos);
        }
    }
};

// All matches for a feature on a document
class DocumentEvidence
{
private:
    std::map<DocumentStream*, StreamMatches> m_matches;
    int m_score = 0;
    int m_numMatches = 0;
public:
    void addMatch(DocumentStream* s, const Match& m);
    int numMatches()const;
    int numMatches(DocumentStream* s)const;
    StreamMatches* getStreamMatches(DocumentStream* s);
    void setScore(int i);
    void dumpMatches(Feature* f, StreamMatchCallback* callback)const
    {
        std::map<DocumentStream*, StreamMatches >::const_iterator it = m_matches.begin();
        std::map<DocumentStream*, StreamMatches >::const_iterator it_end = m_matches.end();
        for (; it != it_end; ++it)
        {
            it->second.dumpMatches(f, it->first,callback);
        }
        
    }
};

// Groups together all matches for a given feature
class FeatureMatches
{
public:
    FeatureMatches():m_size(0){}
    ~FeatureMatches();
    void addMatch(Document* d, DocumentStream* s, const Match& m);
    int numMatches(Document* d)const;
    int numMatches(Document* d, DocumentStream* s)const;
    StreamMatches* getStreamMatches(Document* d, DocumentStream* s);
    void setScore(Document*d, int score)        {m_evidence[d].setScore(score);}
    DocumentEvidence* getEvidence(Document* d);
    
    void dumpMatches(Feature* f, StreamMatchCallback* callback)const
    {
        std::map<Document*, DocumentEvidence >::const_iterator it = m_evidence.begin();
        std::map<Document*, DocumentEvidence >::const_iterator it_end = m_evidence.end();
        for (; it != it_end; ++it)
        {
            it->second.dumpMatches(f,callback);
        }
    }
private:
    unsigned m_size;
    std::map<Document*, DocumentEvidence > m_evidence;
};

class Results
{
private:
    std::map<Feature*, FeatureMatches> m_results;
public:
    StreamMatches* getMatches(Feature* f, Document*d, DocumentStream* s)
    {
        std::map<Feature*, FeatureMatches>::iterator it = m_results.find(f);
        if (it == m_results.end()) return NULL;
        return it->second.getStreamMatches(d,s);
    }
    const FeatureMatches* getFeatureMatches(Feature*f)const
    {
        std::map<Feature*, FeatureMatches>::const_iterator it = m_results.find(f);
        if (it == m_results.end()) return NULL;
        return &it->second;
    }
    void addMatch(Feature* f, Document* d, DocumentStream* s, const Match& m)
    {
        m_results[f].addMatch(d, s, m);
    }
    void addMatch(Feature* f, Document* d, DocumentStream* s, const char* matchStart, const char* matchEnd)
    {
        m_results[f].addMatch(d, s, Match(matchStart, matchEnd));
    }
    void dumpMatches(StreamMatchCallback* callback)
    {
        std::map<Feature*, FeatureMatches>::iterator it = m_results.begin();
        std::map<Feature*, FeatureMatches>::iterator it_end = m_results.end();
        for (; it != it_end; ++it)
        {
            it->second.dumpMatches(it->first, callback);
        }
    }
};

class Scope
{
public:
    virtual ~Scope(){};
    virtual bool getDocs(Document* d, std::vector<Document*>& subDocs)const{return false;}
    virtual bool recurseIntoDoc(Document* d, int recursionLevel)const{return true;}
    virtual bool searchStream(DocumentStream* s)const{return true;}
};

class MatchFilter;

class Context
{
private:
    Results& m_results;
    std::vector<MatchFilter*> m_whereFilter;
    std::vector<const Scope*> m_scopes;
    
    bool filterMatch(Document*d, DocumentStream* s, const Match& m);
public:
    Context(Results& r):m_results(r){}
    const Results& results(){return m_results;}
    
    //
    // support for the "where" clause.
    // filters found matches.
    void pushWhereFilter(MatchFilter* fm);
    void popWhereFilter();
    
    //
    // control which parts of a document are recursed into and searched.
    void pushScope(Scope* s); // scope is the policy of where to search.
    void popScope();
    bool searchDoc(Document* d, int recursionLevel);
    bool searchStream(DocumentStream* s);
    
    //
    // add results
    bool addMatch(Feature* f, Document* d, DocumentStream* s, const Match& m)
    {
        if (filterMatch(d,s,m))// using where filter.
        {
            m_results.addMatch(f,d,s,m);
            return true;
        }
        return false;
    }
    
    // merges all child matches into 1 stream
    // so it can be analysed for derrived matches.
    // e.g. proximity.
    // TODO: apply filter when merging.
    // So we can support post filter operations.
    void mergeChildMatchesForStream(Document* d, DocumentStream* s, Feature* parent, std::vector<FeatureMatch>& allMatches); // merge only this streams matches for all children
    void mergeMatches(Feature* f, StreamMatchCallback* callback);// merge all matches for this feature.
    int getNumMatches(Feature* f,Document* d);
    
    void setScore(Feature* f, Document* d, int score);
    bool getScore(Feature* f, Document* d, int& score);
    
    // sets this NV on currect feature & document.
    void setNV(const char* name, const char* value);
    
    // searches for this NV
    // search this feature,
    //   if not found, search all sub features.
    const char* getNV(const char* name);
};




class MatchFilter 
{
private:
    MatchList m_matches;
    Context& m_context;
public:
    MatchFilter(Feature* f, Context& context);
    ~MatchFilter();
    bool contains(const Match& m)const;
};


#endif /* defined(__textsearchtest__Context__) */
