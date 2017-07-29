/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Classifier__
#define __textsearchtest__Classifier__

#include <iostream>
#include <vector>
#include "rapidxml.hpp"
#include "FS_Search.h"

class DocumentStream;
class Document;
class Feature;

class Classifier
{
public:
    Classifier(rapidxml::xml_node<>* xml);
    
    void addFeature(Feature* f);
    
    bool evaluate(Document* d);
private:
    std::string m_name;
    std::vector<Feature*> m_features;
};

#endif /* defined(__textsearchtest__Classifier__) */
