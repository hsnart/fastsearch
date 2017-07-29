/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Feature__
#define __textsearchtest__Feature__

#include <iostream>
#include "Context.h"
#include "rapidxml.hpp"

class Document;
class Group_Feature;
class Expression_Feature;
class And_Feature;
class Or_Feature;
class Not_Feature;
class Spacing_Feature;
class Within_Feature;
class Scored_Feature;

class FeatureVisitor
{
public:
    virtual ~FeatureVisitor(){};
    virtual void visit(Feature&){};
    virtual void visit(Expression_Feature& f){visit((Group_Feature&)f);}
    virtual void visit(Group_Feature& f){visit((Feature&)f);}
    virtual void visit(And_Feature& f){visit((Group_Feature&)f);}
    virtual void visit(Or_Feature& f) {visit((Group_Feature&)f);}
    virtual void visit(Not_Feature& f){visit((Group_Feature&)f);}
    virtual void visit(Spacing_Feature& f){visit((Group_Feature&)f);}
    virtual void visit(Within_Feature& f){visit((Group_Feature&)f);}
    virtual void visit(Scored_Feature& f){visit((Group_Feature&)f);}
};

class GetMatchCountVisitor : public FeatureVisitor
{
private:
    Results& results;
    Document* document;
public:
    int count = 0;
    GetMatchCountVisitor(Results& r, Document* d):results(r),document(d),count(0){}
    virtual void visit(Expression_Feature& f);
    virtual void visit(Group_Feature& f);
    virtual void visit(Spacing_Feature& f);
};

class Feature
{
public:
    virtual ~Feature(){};
    //
    // returns true if feature found.
    // returns what matched in context.results
    virtual bool evaluate(Document* d, Context& context){return false;}
    void init(rapidxml::xml_node<>* xml);
    virtual void accept(FeatureVisitor& v){v.visit(*this);}
    virtual void recurse(FeatureVisitor& v){};
protected:
    virtual bool addAttribute(rapidxml::xml_attribute<>* xml){return false;};
    virtual bool addNode(rapidxml::xml_node<>* xml){return false;};
};


template<class T> T* CreateFeature(rapidxml::xml_node<>* node)
{
    T* t = new T();
    t->init(node);
    return t;
};


#endif /* defined(__textsearchtest__Feature__) */
