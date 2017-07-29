/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "Feature.h"
#include "ExpressionFeature.h"
#include "SpacingFeature.h"
#include "GroupFeature.h"

using namespace std;
using namespace rapidxml;
//class GetMatchCountVisitor : public FeatureVisitor
void GetMatchCountVisitor::visit(Expression_Feature& f)
{
    const FeatureMatches* fm =results.getFeatureMatches(&f);
    if (fm) count += fm->numMatches(document);
}
void GetMatchCountVisitor::visit(Group_Feature& f)
{
    f.recurse(*this);
}
void GetMatchCountVisitor::visit(Spacing_Feature& f)
{
    const FeatureMatches* fm =results.getFeatureMatches(&f);
    if (fm) count += fm->numMatches(document);
}


void Feature::init(rapidxml::xml_node<>* node)
{
    for (xml_attribute<>* a = node->first_attribute(); a; a=a->next_attribute())
    {
        if (!addAttribute(a))
        {
            cout<<"Warning: unhandled attribute '"<<a->name()<<"'"<<endl;
        }
    }
    
    for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
    {
        if (!addNode(child))
        {
            cout<<"Warning: unhandled node '"<<child->name()<<"'"<<endl;
        }
    }
}



