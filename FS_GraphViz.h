/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__FS_GraphViz__
#define __textsearchtest__FS_GraphViz__

#include <iostream>
#include <set>
#include <sstream>
#include "FS_StateMachine.h"
#include "FS_Char_StateMachine.h"

class FS_GraphViz : public FS_SM_Visitor
{
private:
    std::set<const void*> m_visited;
    std::ostringstream m_links;
    std::ostringstream m_nodes;
    std::vector<const void*> m_stack;
public:
    void getGraph(std::string& g)
    {
        std::ostringstream o;
        o<<"digraph g{"<<endl;
        o<<m_nodes.str();
        o<<m_links.str();
        o<<"}";
        g.append(o.str());
    }
    bool visited(const FS_StateMachine& s)
    {
        return (m_visited.find(&s) != m_visited.end());
    }
    void recurse(const FS_StateMachine& s)
    {
        m_visited.insert(&s);
        m_stack.push_back(&s);
        s.recurse(*this);
        m_stack.pop_back();
    }
    void emitLink(const FS_StateMachine& s)
    {
        if (m_stack.size()>0)
        {
            m_links<<"\""<<m_stack.back()<<"\""<<"->"<<"\""<<&s<<"\";"<<endl;
        }
    }
    void emitNode(const FS_StateMachine& s, const std::string& label)
    {
        m_nodes<<"\""<<&s<<"\""<<" [label=\""<<label<<"\"];"<<endl;
    }
    void process(const FS_StateMachine& s, const std::string& label)
    {
        emitLink(s);
        if (!visited(s))
        {
            emitNode(s,label);
            recurse(s);
        }
    }
    virtual void visit(const FS_NullSm& s)
    {
        process(s,"");
    }
    virtual void visit(const FS_ORSm& s)
    {
        process(s,"");
    }
    virtual void visit(const FS_StartRepeatSm& s)
    {
       process(s,"rep");
    }
    virtual void visit(const FS_LoopRepeatSm& s)
    {
        process(s,"LR");
    }
    virtual void visit(const FS_WordSm& s)
    {
        process(s,std::string(s.begin(),s.end()));
    }
    virtual void visit(const FS_AnySm& s)
    {
        process(s,"any");
    }
    virtual void visit(const FS_DigitsSm& s)
    {
        process(s,"digits");
    }
    virtual void visit(const FS_WildSm& s)
    {
        process(s,std::string(s.begin(),s.end()));
    }
    virtual void visit(const FS_MarkSm& s)
    {
        process(s,"MARK");
    }
    virtual void visit(const FS_ReturnSm& s)
    {
        process(s,"RETURN");
    }
    virtual void visit(const FS_PrecededBySm& s)
    {
        process(s,"PRECEDEDBY");
    }
    virtual void visit(const FS_AbortSm& s)
    {
        process(s,"ABORT");
    }
    virtual void visit(const FS_C_EndSm& s)
    {
        process(s,"END_C");
    }
    virtual void visit(const FS_C_DigitSm& s)
    {
        process(s,"+d");
    }

    virtual void visit(const FS_C_AlphaSm& s)
    {
        process(s,"+a");
    }
    virtual void visit(const FS_C_SpaceSm& s)
    {
        process(s," ");
    }
    virtual void visit(const FS_C_CharSm& s)
    {
        string c(s.begin(),s.end());
        process(s,c);
    }
    virtual void visit(const FS_C_AnySm& s)
    {
        process(s,"?");
    }
    virtual void visit(const FS_C_StarSm& s)
    {
        process(s,"*");
    }

};

#endif /* defined(__textsearchtest__FS_GraphViz__) */
