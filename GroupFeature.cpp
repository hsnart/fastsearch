/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "GroupFeature.h"
#include "Document.h"
#include "ScoredFeature.h"
#include "SpacingFeature.h"
#include "BooleanFeatures.h"
#include "WithinFeature.h"
#include "GroupFeature.h"
#include "ExpressionFeature.h"
#include <vector>
using namespace std;
using namespace rapidxml;


//
//
bool SimpleSubDocScope::getDocs(Document* d, std::vector<Document*>& subDocs)const
{
    d = d->getSubDocPath(m_path);
    if (d)
    {
        subDocs.push_back(d);
        return true;
    }
    return false;
}


Group_Feature::Group_Feature(){};
Group_Feature::~Group_Feature()
{
    clearPtrVector(m_features);
    clearPtrVector(m_scopes);
}

bool Group_Feature::addAttribute(rapidxml::xml_attribute<>* a)
{
    if (strcasecmp(a->name(),"name")==0)
    {
        m_name = a->value(); return true;
    }
    if (strcasecmp(a->name(),"description")==0)
    {
        m_description = a->value(); return true;
    }
    if (strcasecmp(a->name(),"scope")==0)
    {
        m_scopes.push_back(new SimpleSubDocScope(a->value())); return true;
    }
    return false;
}
bool Group_Feature::addNode(rapidxml::xml_node<>* child)
{
    if (strcasecmp(child->name(),"feature")==0 || strcasecmp(child->name(),"f")==0)
    {
        m_features.push_back(CreateFeature<Group_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"scored")==0)
    {
        m_features.push_back(CreateFeature<Scored_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"spacing")==0)
    {
        m_features.push_back(CreateFeature<Spacing_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"and")==0)
    {
        m_features.push_back(CreateFeature<And_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"or")==0)
    {
        m_features.push_back(CreateFeature<Or_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"not")==0)
    {
        m_features.push_back(CreateFeature<Not_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"where")==0)
    {
        m_features.push_back(CreateFeature<Within_Feature>(child)); return true;
    }
    if (strcasecmp(child->name(),"keystring")==0 || strcasecmp(child->name(),"expression")==0 || strcasecmp(child->name(),"exp")==0)
    {
        addExpression(child); return true;
    }
    return false;
}

void Group_Feature::addExpression(xml_node<>* node)
{
    if (node->value_size()>0)
    {
        m_features.push_back(new Expression_Feature(node->value()));
    }
}
bool Group_Feature::evaluate(Document* d, Context& context)
{
    // Go through each scope, and each document identified.
    // e.g. Body + all attachment bodies.
    bool ret = false;
    if (m_scopes.size()>0)
    {
        for (int i=0; i<m_scopes.size(); i++)
        {
            vector<Document*> docs;
            if (m_scopes[i]->getDocs(d,docs))
            {
                context.pushScope(m_scopes[i]); // scopes control recursion through sub docs and streams.
                for (int j=0; j<docs.size(); j++)
                {
                    if (evaluateInner(docs[j], context))
                    {
                        ret = true;
                    }
                }
                context.popScope();
            }
        }
    } else {
        ret = evaluateInner(d,context);
    }
    cout<<"feature:"<<m_name<<" returned:"<<ret<<endl;
    return ret;
}
void Group_Feature::recurse(FeatureVisitor& v)
{
    for(int i=0; i<m_features.size();i++)
    {
        m_features[i]->accept(v);
    }
}
//
// TODO: lookup cached results if there are any
int Group_Feature::findAllFeatures(Document* d, Context& context)
{
    int featuresFound=0;
    
    for(int i=0; i<m_features.size();i++)
    {
        if (m_features[i]->evaluate(d, context))
        {
            featuresFound++;
        }
    }
    return featuresFound;
}

bool Group_Feature::evaluateInner(Document* d, Context& context)
{
    if (findAllFeatures(d,context)>0)
    {
        cout<<"feature::"<<m_name<<" fired on document:"<<d->getName()<<endl;
        return true;
    }
    return false;
}

