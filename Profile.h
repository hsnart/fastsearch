/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Profile__
#define __textsearchtest__Profile__

#include <iostream>
#include <vector>
#include "rapidxml.hpp"


class Classifier;
class Document;

class Profile
{
private:
    std::vector<Classifier*> m_classifiers;
public:
    void init(char* xml);
    void init(rapidxml::xml_node<>* node);
    
    void evaluate(Document* d);
};

#endif /* defined(__textsearchtest__Profile__) */
