//
//  FS_OptimiseVisitor.h
//  textsearchtest
//
//  Created by howard on 14/03/2014.
//  Copyright (c) 2014 howard. All rights reserved.
//

#ifndef __textsearchtest__FS_OptimiseVisitor__
#define __textsearchtest__FS_OptimiseVisitor__


#include <iostream>
#include <set>
#include <sstream>
#include "FS_StateMachine.h"
#include "FS_Char_StateMachine.h"

class FS_OptimiseVisitor : public FS_SM_Visitor
{
private:
    std::set<const FS_StateMachine*> m_visited;
    std::vector<const FS_StateMachine*> m_stack;
public:
    void recurse(const FS_StateMachine& s)
    {
        if (m_visited.find(&s) == m_visited.end())
        {
            m_stack.push_back(&s);
            s.recurse(*this);
            m_stack.pop_back();
        }
        m_visited.insert(&s);
    }
    virtual void process(const FS_StateMachine& s)
    {
        recurse(s);
    };
    virtual void visit(const FS_NullSm& s)
    {
        // search back through stack
        // find first non-null sm
        // call replaceLink(&s,s.m_next);
        if (m_stack.size()>0)
        {
            const_cast<FS_StateMachine*>(m_stack.back())->replaceLink(&s,s.getNext());
        }
        s.recurse(*this);
    }
};


#endif /* defined(__textsearchtest__FS_OptimiseVisitor__) */
