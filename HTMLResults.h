/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__HTMLResults__
#define __textsearchtest__HTMLResults__

#include <iostream>

#include "Feature.h"
#include "Context.h"
#include "GroupFeature.h"
#include "Document.h"



class HTMLResults : public FeatureVisitor, StreamMatchCallback
{
private:
    // use this stack to determine which are low level matches and which are high level
    // this then changes the sort order of the insert points.
    std::vector<Feature*> m_stack;
    Results* m_results;
    Document* document;
    struct Markup
    {
        Feature* feature;
        const char* point;
        bool start; // if false then end point.
        bool operator<(const Markup& right)const{return point<right.point;}
    };
    std::map<DocumentStream*, std::vector<Markup> > m_markup;
public:
    virtual void visit(Group_Feature& f)
    {
        m_results->dumpMatches(this);
        
    }
    virtual void addMatch(Feature* f, DocumentStream* s, const char* start, const char* end)
    {
        Markup m;
        m.feature = f;
        m.point = start;
        m.start = true;
        m_markup[s].push_back(m);
        m.start = false;
        m.point = end;
        m_markup[s].push_back(m);
    }
    
    void dumpDoc(Document* d, std::string& html)
    {
        html.append("<div>");
        for (int i=0; i<d->numStreams(); i++)
        {
            DocumentStream* s = d->getStream(i);
            std::map<DocumentStream*, std::vector<Markup> >::iterator it = m_markup.find(s);
            if (it != m_markup.end())
            {
                std::vector<Markup>& v = it->second;
                // sort it.
                std::sort(v.begin(),v.end());
                const char* pos =s->getText();
                std::vector<Markup>::iterator it = v.begin();
                std::vector<Markup>::iterator it_end = v.end();
                for (;it != it_end; ++it)
                {
                    html.append(pos,it->point);
                    pos = it->point;
                    if (it->start)
                    {
                        html.append("{");
                    } else {
                        html.append("}");
                    }
                }
                html.append(pos);
            } else {
                html.append(s->getText());
            }
        }
        for(int i=0; i<d->numSubDocs(); i++)
        {
            dumpDoc(d->getSubDoc(i),html);
        }
        html.append("</div>");
    }
    
    void generate(Feature* root, Results* results, Document* d)
    {
        m_results = results;
        m_results->dumpMatches(this);
        
        std::string html;
        dumpDoc(d, html);
        std::cout<<"html="<<html<<std::endl;
    }
};


#endif /* defined(__textsearchtest__HTMLResults__) */
