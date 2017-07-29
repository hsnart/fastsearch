/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "FS_StateMachine.h"
#include "FS_Char_StateMachine.h"


void FS_SM_Visitor::visit(const FS_NullSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_ORSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_StartRepeatSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_LoopRepeatSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_WordSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_AnySm& s){process(s);}
void FS_SM_Visitor::visit(const FS_DigitsSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_WildSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_MarkSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_ReturnSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_PrecededBySm& s){process(s);}
void FS_SM_Visitor::visit(const FS_AbortSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_EndSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_DigitSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_AlphaSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_CharSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_SpaceSm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_AnySm& s){process(s);}
void FS_SM_Visitor::visit(const FS_C_StarSm& s){process(s);}

FS_WordSm* FS_WordSm::create(const std::string& word, bool matchCase, bool matchSpace)
{
    string s = word;
    if (!matchCase)
    {
        // convert string to lower
        for (int i=0; i<s.size(); i++)
        {
            s[i] = tolower(s[i]);
        }
    }
    FS_WordSm* e = 0;
    if (matchCase && matchSpace)        e = FS_WordSmImpl<true,true>::create(s);
    else if (matchCase && !matchSpace)  e = FS_WordSmImpl<true, false>::create(s);
    else if (!matchCase && matchSpace)  e = FS_WordSmImpl<false, true>::create(s);
    else if (!matchCase && !matchSpace) e = FS_WordSmImpl<false, false>::create(s);
    return e;
}

FS_ORSm* FS_ORSm::create(int n)
{
    switch (n)
    {
        case(2): return new FS_ORSmImplN<2>();
        case(3): return new FS_ORSmImplN<3>();
        case(4): return new FS_ORSmImplN<4>();
        case(5): return new FS_ORSmImplN<5>();
        case(6): return new FS_ORSmImplN<6>();
        case(7): return new FS_ORSmImplN<7>();
        case(8): return new FS_ORSmImplN<8>();
        default: return new FS_ORSmImpl();
    }
}
