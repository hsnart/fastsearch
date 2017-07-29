/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "Profile.h"
#include "rapidxml.hpp"
#include "Classifier.h"
#include "Document.h"

using namespace rapidxml;
using namespace std;

void Profile::init(char* xml)
{
    xml_document<> xmlDoc;
    xmlDoc.parse<0>(xml);

    
    for (xml_node<> *child = xmlDoc.first_node(); child; child = child->next_sibling())
    {
        init(child); // expecting a single profile node
    }

}

void Profile::init(xml_node<>* node)
{
    for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
    {
        cout<<"node name="<<child->name()<<endl;
        if (strcasecmp(child->name(),"classifier") ==0)
        {
            m_classifiers.push_back(new Classifier(child));
        }
    }
}

void Profile::evaluate(Document* d)
{
    for (int i =0; i<m_classifiers.size(); i++)
    {
        m_classifiers[i]->evaluate(d);
    }
}
