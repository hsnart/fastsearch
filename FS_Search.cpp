/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "FS_Search.h"
#include "FS_Expr.h"
//class FS_StartPointsVisitor : public FS_Visitor

void FS_Expression::FS_StartPointsVisitor::accept(const FS_SequenceExpr& e)
{
    if (e.exprs().size()>0)
    {
        e.exprs()[0]->visit(*this);
    }
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_ORExpr& e)
{
    for (int i=0; i<e.exprs().size(); i++)
    {
        e.exprs()[i]->visit(*this);
    }
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_RepeatExpr& e)
{
    if (e.rep()) e.rep()->visit(*this);
    if (e.next()) e.next()->visit(*this);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_TokenExpr& e)
{
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_WordExpr& e)
{
    m_startPoints.push_back(&e);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_AnyExpr& e)
{
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_RegionExpr& e)
{
    
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_DigitsExpr& e)
{
    m_startPoints.push_back(&e);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_WildExpr& e)
{
    m_startPoints.push_back(&e);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_MarkExpr& e)
{
    e.body()->visit(*this);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_ReturnExpr& e)
{
    e.body()->visit(*this);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_PrecededByExpr& e)
{
    e.body()->visit(*this);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_AbortExpr& e){}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_MatchCaseExpr& e)
{
    e.body()->visit(*this);
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_DigitExpr& e)
{
    // TODO do better here
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_AlphaExpr& e)
{
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_CharExpr& e)
{
    // TODO do better here
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_EndExpr& e)
{
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_AnyExpr& e)
{
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_SpaceExpr& e)
{
    m_any = true;
}
void FS_Expression::FS_StartPointsVisitor::accept(const FS_C_StarExpr& e)
{
    m_any = true;
}

void FS_Expression::FS_StartPointsVisitor::getStartPoints(vector<StartPoint>& sp)
{
    if (!m_any){
        vector<const FS_Expr*>::iterator it = m_startPoints.begin();
        vector<const FS_Expr*>::iterator it_end = m_startPoints.end();
        for (; it != it_end; ++it)
        {
            sp.push_back((*it)->getStartPoint());
            if (sp.back().m_type == StartPoint::Any)
            {
                sp.clear();
                return;
            }
        }
    }
}


//
//
//
//class FS_OptimiseVisitor : public FS_SM_Visitor
void FS_Expression::FS_OptimiseVisitor::process(const FS_StateMachine& s)
{
    if (m_visited.find(&s) == m_visited.end())
    {
        m_visited.insert(&s);
        m_stack.push_back(&s);
        s.recurse(*this);
        m_stack.pop_back();
    }
};
void FS_Expression::FS_OptimiseVisitor::visit(const FS_NullSm& s)
{
    if (m_stack.size()>0)
    {
        const_cast<FS_StateMachine*>(m_stack.back())->replaceLink(&s,s.getNext());
    }
    s.recurse(*this);
}

