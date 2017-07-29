/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__GroupFeature__
#define __textsearchtest__GroupFeature__

#include <iostream>
#include <vector>
#include "Feature.h"

//
// More advanced scopes will determine
// how to recurse through zip files.
class SimpleSubDocScope : public Scope
{
private:
    std::string m_path;
public:
    virtual ~SimpleSubDocScope(){};
    SimpleSubDocScope(const std::string& path):m_path(path){}
    virtual bool getDocs(Document* d, std::vector<Document*>& subDocs)const;
};


class Group_Feature : public Feature
{
protected:
    std::string m_name;
    std::string m_description;
    
    std::vector<Feature*> m_features;
    std::vector<Scope*> m_scopes;
    
    void addExpression(rapidxml::xml_node<>* node);
protected:
    virtual bool addAttribute(rapidxml::xml_attribute<>* xml);
    virtual bool addNode(rapidxml::xml_node<>* xml);
    virtual bool evaluateInner(Document* d, Context& context);
    int findAllFeatures(Document* d, Context& context);
public:
    Group_Feature();
    virtual ~Group_Feature();
    virtual bool evaluate(Document* d, Context& context);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
    virtual void recurse(FeatureVisitor& v);
};

#endif /* defined(__textsearchtest__GroupFeature__) */
