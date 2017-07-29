/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "SpacingFeature.h"
#include "Document.h"
#include "TokenStream.h"
using namespace std;
using namespace rapidxml;
//
//
//
//class Spacing_Feature

bool Spacing_Feature::addAttribute(rapidxml::xml_attribute<>* a)
{
    if (strcasecmp(a->name(),"maxSeparation")==0)
    {
        m_maxSeparation = atoi(a->value()); return true;
    }
    if (strcasecmp(a->name(),"minMatches")==0)
    {
        m_minMatches = atoi(a->value()); return true;
    }
    if (strcasecmp(a->name(),"surround")==0)
    {
        m_surround = atoi(a->value()); return true;
    }
    return Group_Feature::addAttribute(a);
}
bool Spacing_Feature::evaluateInner(Document* d, Context& context)
{
    if (findAllFeatures(d,context)>0)
    {
        if (evaluateProximity(d,context)) return true;
    }
    return false;
}

bool Spacing_Feature::evaluateProximity(Document* d, Context& context)
{
    bool fires = false;
    for (int i=0; i<d->numStreams(); i++)
    {
        if (evaluateStream(d,d->getStream(i), context))
        {
            fires = true;
        }
    }
    for (int j=0; j<d->numSubDocs(); j++)
    {
        if (evaluateProximity(d->getSubDoc(j),context))
        {
            fires = true;
        }
    }
    return fires;
}

bool Spacing_Feature::evaluateStream(Document* d, DocumentStream* s, Context& context)
{
    // combine all markers into 1 stream, then look for matches...
    vector<FeatureMatch> allMatches;
    context.mergeChildMatchesForStream(d,s,this, allMatches);// get all matches for this stream from the children.
    int tcount = 0;
    if (allMatches.size()>0)
    {
        // walk the matches.
        // looks for a window of matches that fits the criteria
        // CRUDE TEMP TEST BELOW!!!
        // just finds largest section.
        FeatureMatch* last = NULL;
        Match current;
        int numMatches = 0;
        bool found = false;
        for (int i=0; i<allMatches.size(); i++)
        {
            FeatureMatch& m = allMatches[i];
            if (last != NULL)
            {
                if ((m.startPos - last->endPos)<=m_maxSeparation)
                {
                    if (!found)
                    {
                        current = *last;
                        numMatches = 1;
                    }
                    current.endPos = m.endPos;
                    found = true;
                    numMatches++;
                } else {
                    if (found && numMatches >=m_minMatches)
                    {
                        tcount++;
                        // expand match to include surround.
                        context.addMatch(this, d,s, current);
                        string snippet(current.startPos, current.endPos);
                        cout<<"proximity match:"<<snippet<<endl;
                    }
                    found = false;
                    numMatches = 0;
                }
            }
            last = &m;
        }
        if (found && numMatches >=m_minMatches)
        {
            tcount++;
            context.addMatch(this, d,s, current);
            string snippet(current.startPos, current.endPos);
            cout<<"proximity match:"<<snippet<<endl;
        }
    }
    return tcount>0;
    
}


//
bool Sequence_Feature::addAttribute(rapidxml::xml_attribute<>* a)
{
    if (strcasecmp(a->name(),"maxSeparation")==0)
    {
        m_maxSeparation = atoi(a->value()); return true;
    }
    return Group_Feature::addAttribute(a);
}
bool Sequence_Feature::evaluateInner(Document* d, Context& context)
{
    for(int i=0; i<m_features.size();i++)
    {
        if (!m_features[i]->evaluate(d, context))
        {
            return false;
        }
    }
    return (evaluateProximity(d,context));
}

bool Sequence_Feature::evaluateProximity(Document* d, Context& context)
{
    bool fires = false;
    for (int i=0; i<d->numStreams(); i++)
    {
        if (evaluateStream(d,d->getStream(i), context))
        {
            fires = true;
        }
    }
    for (int j=0; j<d->numSubDocs(); j++)
    {
        if (evaluateProximity(d->getSubDoc(j),context))
        {
            fires = true;
        }
    }
    return fires;
}

//
// TODO: implement propper back tracking state machine for matching sequences
// of found sub features.
// Just like FS_Expression sequences.
bool Sequence_Feature::evaluateStream(Document* d, DocumentStream* s, Context& context)
{
    return true;
}

