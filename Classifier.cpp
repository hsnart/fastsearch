/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "Classifier.h"
#include "FS_Search.h"
#include "Document.h"
#include "Feature.h"
#include "GroupFeature.h"
#include "HTMLResults.h"


using namespace std;
using namespace rapidxml;


//
//
//
// Classifier

void Classifier::addFeature(Feature* f)
{
    m_features.push_back(f);
}

bool Classifier::evaluate(Document* d)
{
    bool res = false;
    
    Results results;
    Context context(results);
    for(int i=0; i<m_features.size(); i++)
    {
        if (m_features[i]->evaluate(d,context)) res = true;
    }
    HTMLResults hr;
    hr.generate(m_features[0], &results,d);
    cout<<"classifier::evaluate returning:"<<res<<endl;
    return res;
}

Classifier::Classifier(xml_node<>* node)
{
    for (xml_attribute<>* a = node->first_attribute(); a; a=a->next_attribute())
    {
        cout<<"classifier::attribute: name = "<<a->name()<<endl;
        if (strcmp(a->name(),"name")==0)
        {
            m_name = a->value();
        }
    }
    
    for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
    {
        cout<<"classifier::node: name = "<<child->name()<<endl;
        if (strcasecmp(child->name(),"feature")==0 || strcasecmp(child->name(),"f")==0)
        {
            m_features.push_back(CreateFeature<Group_Feature>(child));
        }
    }
}
