/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "WithinFeature.h"
#include "Document.h"
using namespace std;
using namespace rapidxml;

//class Within_Feature : public Group_Feature

bool Within_Feature::addNode(rapidxml::xml_node<>* xml)
{
    if (strcasecmp(xml->name(),"location")==0)
    {
        m_location = CreateFeature<Group_Feature>(xml); return true;
    }
    if (strcasecmp(xml->name(),"find")==0)
    {
        m_features.push_back(CreateFeature<Group_Feature>(xml)); return true;
    }
    return Group_Feature::addNode(xml);
}
//
// implements a pre-filter aproach.
// setup filter, then search using that filter.
bool Within_Feature::evaluate(Document* d, Context& context)
{
    bool res = false;
    
    if (m_location->evaluate(d,context))
    {
        MatchFilter mf(m_location,context);
        
        res = findAllFeatures(d,context)>0;
    }
    cout<<"feature:"<<m_name<<" returned:"<<res<<endl;
    return res;
}
/*
 // post filter code.
 // trying for pre-filter first
 
 
 
bool Where_Feature::pruneMatches(Document* d, int& count, Results& results)
{
    bool fires = false;
    for (int i=0; i<d->numStreams(); i++)
    {
        if (pruneMatchesInStream(d->getStream(i), count, results))
        {
            fires = true;
        }
    }
    for (int j=0; j<d->numSubDocs(); j++)
    {
        if (pruneMatches(d->getSubDoc(j),count,results))
        {
            fires = true;
        }
    }
    return fires;
}

bool Where_Feature::pruneMatchesInStream(DocumentStream* s, int& count, Results& results)
{
    int c = 0;
    StreamMatches* locationMatches = results.getMatches(m_location,s);
    if (locationMatches)
    {
        for (int i=0; i<m_features.size(); i++)
        {
            pruneMatchesInStreamForFeature(locationMatches, m_features[i],s,c,results);
        }
    } else {
        c = copyMatchesForStream(s,results);
    }
    count +=c;
    return c>0;
}

bool Where_Feature::pruneMatchesInStreamForFeature(StreamMatches* locationMatches, Feature* f, DocumentStream* s, int& count, Results& results)
{
    StreamMatches* matches = results.getMatches(f,s);
    int c =0;
    for(int i=0; i<matches->size(); i++)
    {
        Match& m = (*matches)[i];
        if (locationMatches->contains(s,m))
        {
            results.addMatch(f,s,m);
            c++;
        }
    }
    count += c;
    return c>0;
}*/
