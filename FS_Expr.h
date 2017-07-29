/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__FS_Expr__
#define __textsearchtest__FS_Expr__

#include <iostream>
#include "FS_StateMachine.h"
#include "FS_Char_StateMachine.h"
#include "FS_Utils.h"
#include <string>
#define F_IN //cout<<__PRETTY_FUNCTION__<<endl;

class FS_SequenceExpr;
class FS_ORExpr;
class FS_RepeatExpr;
class FS_TokenExpr;
class FS_WordExpr;
class FS_AnyExpr;
class FS_RegionExpr;
class FS_DigitsExpr;
class FS_WildExpr;
class FS_MarkExpr;
class FS_ReturnExpr;
class FS_PrecededByExpr;
class FS_AbortExpr;
class FS_MatchCaseExpr;
class FS_C_DigitExpr;
class FS_C_AlphaExpr;
class FS_C_SpaceExpr;
class FS_C_CharExpr;
class FS_C_EndExpr;
class FS_C_AnyExpr;
class FS_C_StarExpr;


class FS_Visitor
{
public:
    virtual void accept(const FS_SequenceExpr&) = 0;
    virtual void accept(const FS_ORExpr&) = 0;
    virtual void accept(const FS_RepeatExpr&) = 0;
    virtual void accept(const FS_TokenExpr&) = 0;
    virtual void accept(const FS_WordExpr&) = 0;
    virtual void accept(const FS_AnyExpr&) = 0;
    virtual void accept(const FS_RegionExpr&){};
    virtual void accept(const FS_DigitsExpr&) = 0;
    virtual void accept(const FS_WildExpr&) = 0;
    virtual void accept(const FS_MarkExpr&) = 0;
    virtual void accept(const FS_ReturnExpr&) = 0;
    virtual void accept(const FS_PrecededByExpr&) = 0;
    virtual void accept(const FS_AbortExpr&) = 0;
    virtual void accept(const FS_MatchCaseExpr&) = 0;
    virtual void accept(const FS_C_DigitExpr&) = 0;
    virtual void accept(const FS_C_AlphaExpr&) = 0;
    virtual void accept(const FS_C_SpaceExpr&) = 0;
    virtual void accept(const FS_C_CharExpr&) = 0;
    virtual void accept(const FS_C_EndExpr&) = 0;
    virtual void accept(const FS_C_AnyExpr&) = 0;
    virtual void accept(const FS_C_StarExpr&) = 0;
};

struct StartPoint
{
    enum Type{
        Any,
        Word,
        Prefix
    }m_type;
    unsigned int m_hash;
};

class FS_Expr_Builder
{
    int m_repNum = 0;
    std::vector<FS_StateMachine*> m_exprs;// owns them
    bool m_matchCase = false;
    bool m_matchSpace = false;
public:
    int allocateRepeat(){return m_repNum++;}
    int getNumRepeats(){return m_repNum;}
    FS_StateMachine* add(FS_StateMachine* e){m_exprs.push_back(e); return e;}
    void takeExprs(std::vector<FS_StateMachine*>& v){m_exprs.swap(v);}
    bool setMatchSpace(bool b){bool r = m_matchSpace; m_matchSpace = b; return r;}
    bool setMatchCase(bool b){bool r = m_matchCase; m_matchCase = b; return r;}
    bool matchCase(){ return m_matchCase;}
    bool matchSpace(){ return m_matchSpace;}
};

class FS_Expr
{
public:
    virtual ~FS_Expr(){};
    virtual void visit(FS_Visitor& v) = 0;
    virtual StartPoint getStartPoint()const{StartPoint p; p.m_type = StartPoint::Any; return p;}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end) = 0;
    virtual FS_StateMachine* emitSeq(const std::vector<FS_Expr*>& v, int& pos, FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return (v[pos++]->emit(b,end));
    }
};

class FS_SequenceExpr : public FS_Expr
{
private:
    vector<FS_Expr*> m_exprs;
public:
    static FS_Expr* create(vector<FS_Expr*>& stack)
    {
        if (stack.size()>1)
        {
            return new FS_SequenceExpr(stack);
        } else if (stack.size()==1)
        {
            return stack.back();
        }
        return NULL; // throw!!!??
    }
    FS_SequenceExpr(vector<FS_Expr*>& stack)
    {
        m_exprs.swap(stack);
    }
    virtual ~FS_SequenceExpr()
    {
        clearPtrVector(m_exprs);
    }
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    const vector<FS_Expr*> exprs()const{return m_exprs;}
    
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        FS_StateMachine* start = NULL;
        
        int i=0;
        while (i<m_exprs.size())
        {
            FS_StateMachine* last = NULL;
            FS_StateMachine* s = m_exprs[i]->emitSeq(m_exprs,i,b,last);
            if (!start) start = s;
            if (end != NULL)
            {
                end->addNext(s);
            }
            end = last;
        }
        return start;
    }
};

class FS_MatchCaseExpr : public FS_Expr
{
private:
    FS_Expr* m_body;
    bool m_enable;
public:
    static FS_Expr* create(vector<FS_Expr*>& rhs_stack, bool enable)
    {
        return new FS_MatchCaseExpr(FS_SequenceExpr::create(rhs_stack), enable);
    }
    FS_MatchCaseExpr(FS_Expr* e, bool enable){m_body = e;}
    virtual ~FS_MatchCaseExpr()
    {
        if (m_body) delete m_body;
    }
    FS_Expr* body()const{return m_body;}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        bool oldVal = b.setMatchCase(m_enable);
        FS_StateMachine* s = m_body->emit(b,end);
        b.setMatchCase(oldVal);
        return s;
    }
};

// base class for anything that matches a token.
class FS_TokenExpr : public FS_Expr
{};

class FS_WordExpr : public FS_TokenExpr
{
private:
    std::string m_word;
public:
    FS_WordExpr(const char* start, const char* end): m_word(start,end){};
    virtual ~FS_WordExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual StartPoint getStartPoint()const
    {StartPoint p; p.m_type = StartPoint::Word;
        p.m_hash = WordIndex::hash(m_word.c_str(),m_word.c_str()+m_word.size());
        return p;}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(FS_WordSm::create(m_word,b.matchCase(),b.matchSpace()));
    }
};
class FS_WildExpr : public FS_TokenExpr
{
private:
    std::string m_word;
public:
    FS_WildExpr(const char* start, const char* end): m_word(start,end){};
    virtual ~FS_WildExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual StartPoint getStartPoint()const
    {
        StartPoint p;
        if (m_word.size()>1 && isalnum(*m_word.c_str()) && isalnum(*(m_word.c_str()+1)))
        {
            p.m_type = StartPoint::Prefix;
            p.m_hash = PrefixIndex::prefixHash(m_word.c_str(),m_word.c_str()+m_word.size());
        } else {
            p.m_type = StartPoint::Any;
        }
        return p;
    }
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        FS_StateMachine* s = 0;
        if (b.matchCase() && b.matchSpace()) s = new FS_WildSmImpl<true, true>(m_word);
        else if (b.matchCase() && !b.matchSpace()) s = new FS_WildSmImpl<true, false>(m_word);
        else if (!b.matchCase() && b.matchSpace()) s = new FS_WildSmImpl<false, true>(m_word);
        else if (!b.matchCase() && !b.matchSpace()) s = new FS_WildSmImpl<false, false>(m_word);
        return end = b.add(s);
    }
};

class FS_ORExpr: public FS_Expr
{
private:
    vector<FS_Expr*> m_exprs;
    FS_Expr* m_next = NULL;
public:
    // got this wrong...
  //  rhs stack needs to be appended onto each bit
    static void create(vector<FS_Expr*>& lhs_stack, vector<FS_Expr*>& rhs_stack)
    {
        FS_Expr* rhs = rhs_stack.front(); rhs_stack.front() = NULL;
        FS_Expr* lhs = lhs_stack.back(); lhs_stack.pop_back();
        FS_ORExpr* e;
        if (dynamic_cast<FS_ORExpr*>(lhs) != NULL)
        {
            e = ((FS_ORExpr*)(lhs));
            e->add(rhs);
        } else if (dynamic_cast<FS_ORExpr*>(rhs) != NULL)
        {
            e = ((FS_ORExpr*)(rhs));
            e->push_front(lhs);
        }else {
            e = new FS_ORExpr();
            e->add(lhs);
            e->add(rhs);
        }
        lhs_stack.push_back(e);
        if (rhs_stack.size()>1)
        {
            // add to each expression.
            rhs_stack.erase(rhs_stack.begin());
            e->m_next = FS_SequenceExpr::create(rhs_stack);
        }
    }
    virtual ~FS_ORExpr()
    {
        clearPtrVector(m_exprs);
        delete m_next;
    }
    void add(FS_Expr* e)
    {
        m_exprs.push_back(e);
    }
    void push_front(FS_Expr* e)
    {
        m_exprs.insert(m_exprs.begin(),e);
    }
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    const vector<FS_Expr*> exprs()const{return m_exprs;}
    
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        FS_StateMachine* nextStart = 0;
        if (m_next)
        {
            nextStart = m_next->emit(b,end);
        } else {
            nextStart = b.add(new FS_NullSm());// TODO can this be optimised out?
            end = nextStart;
        }
        if (m_exprs.size()==1)
        {
            return m_exprs[0]->emit(b,end);
        } else {
            FS_ORSm* branch = FS_ORSm::create(m_exprs.size());
            b.add(branch);
            
            vector<FS_Expr*>::iterator it = m_exprs.begin();
            vector<FS_Expr*>::iterator it_end = m_exprs.end();
            for(; it != it_end; ++it)
            {
                FS_StateMachine* e = NULL;
                branch->addNext((*it)->emit(b,e));
                e->addNext(nextStart);
            }
            return branch;
        }
    }
};

class FS_RepeatExpr: public FS_Expr
{
private:
    FS_Expr* m_rep;
    FS_Expr* m_next;
    int m_min = 0;
    int m_max = -1;
    bool m_greedy = false;
public:
    static void create(vector<FS_Expr*>& stack, vector<FS_Expr*>& rhs_stack, int repMin, int repMax, bool greedy)
    {
        FS_Expr* lhs = stack.back(); stack.pop_back();
        stack.push_back(new FS_RepeatExpr(lhs,FS_SequenceExpr::create(rhs_stack),repMin,repMax,greedy));
    }
    FS_RepeatExpr(FS_Expr* rep, FS_Expr* next, int repMin, int repMax, bool greedy)
    : m_rep(rep), m_next(next), m_min(repMin), m_max(repMax), m_greedy(greedy)
    {
        if (m_max ==-1) m_max = 10000; // threshold at 10000.... that is a lot..
    }
    virtual ~FS_RepeatExpr()
    {
        if (m_rep) delete m_rep;
        if (m_next) delete m_next;
    }
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    FS_Expr* rep()const{return m_rep;}
    FS_Expr* next()const{return m_next;}
    
    
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        if (m_min ==0 && m_max ==1)//[?]
        {
            FS_ORSm* start = FS_ORSm::create(2); b.add(start);
            FS_StateMachine* repEnd = 0;
            start->addNext(m_rep->emit(b,repEnd));
            FS_StateMachine* next = NULL;
            if (m_next)
            {
                next = m_next->emit(b,end);
            } else {
                next = new FS_NullSm(); b.add(next);
                end = next;
            }
            repEnd->addNext(next);
            start->addNext(next);
            return start;
        } else {
            int repNum = b.allocateRepeat();
            FS_StartRepeatSm* start = new FS_StartRepeatSm(repNum); b.add(start);
            FS_StateMachine* body = m_rep->emit(b,end);
            start->setBody(body);
            FS_LoopRepeatSm* loop = new FS_LoopRepeatSm(repNum,m_min,m_max,m_greedy); b.add(loop);
            end->addNext(loop);
            loop->setBody(body);
            end = loop;
            
            FS_StateMachine* next = NULL;
            if (m_next)
            {
                next = m_next->emit(b,end);
            } else {
                next = new FS_NullSm(); b.add(next);
                end = next;
            }
            
            loop->addNext(next);
            if (m_min ==0) start->setBypass(next);
            return start;
        }
    }
};


class FS_AnyExpr : public FS_TokenExpr{
public:
    virtual ~FS_AnyExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        FS_StateMachine* s = 0;
        if (b.matchSpace()) s = new FS_AnySmImpl<true>();
        else s = new FS_AnySmImpl<false>();
        return end = b.add(s);
    }
};


// matches a region
// then can apply a constraint
// or extract as a name value pair.
/*
class FS_RegionExpr : public FS_Expr{
private:
    FS_Expr* m_inner;
    FS_Expr* m_next;
public:
    virtual bool matches(TokenStream::iterator& it)
    {
        TokenStream::iterator::Pos pos = it.getPos();
        if (matches_inner(it)) return true;
        it.setPos(pos);
        return false;
    }
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return NULL;
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        return matches_inner(it);
    }
};
*/
class FS_DigitsExpr : public FS_TokenExpr
{
public:
    virtual ~FS_DigitsExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        FS_StateMachine* s = 0;
        if (b.matchSpace()) s = new FS_DigitsSmImpl<true>();
        else s = new FS_DigitsSmImpl<false>();
        b.add(s);
        end= s;
        return s;
    }
};

class FS_MarkExpr : public FS_Expr
{
private:
    FS_Expr* m_body;
public:
    static FS_Expr* create(vector<FS_Expr*>& rhs_stack)
    {
        return new FS_MarkExpr(FS_SequenceExpr::create(rhs_stack));
    }
    FS_MarkExpr(FS_Expr* e){m_body = e;}
    virtual ~FS_MarkExpr()
    {
        if (m_body) delete m_body;
    }
    FS_Expr* body()const{return m_body;}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return b.add(new FS_MarkSm(m_body->emit(b,end)));
    }
};

class FS_ReturnExpr : public FS_Expr
{
private:
    FS_Expr* m_body;
public:
    static FS_Expr* create(vector<FS_Expr*>& rhs_stack)
    {
        return new FS_ReturnExpr(FS_SequenceExpr::create(rhs_stack));
    }
    FS_ReturnExpr(FS_Expr* e){m_body = e;}
    virtual ~FS_ReturnExpr()
    {
        if (m_body) delete m_body;
    }
    FS_Expr* body()const {return m_body;}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return b.add(new FS_ReturnSm(m_body->emit(b,end)));
    }
};

class FS_PrecededByExpr : public FS_Expr
{
private:
    FS_Expr* m_body;
    int m_distance;
public:
    static FS_Expr* create(vector<FS_Expr*>& rhs_stack, int d)
    {
        return new FS_PrecededByExpr(FS_SequenceExpr::create(rhs_stack),d);
    }
    FS_PrecededByExpr(FS_Expr* e, int d):m_body(e),m_distance(d){}
    virtual ~FS_PrecededByExpr()
    {
        if (m_body) delete m_body;
    }
    FS_Expr* body()const {return m_body;}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_PrecededBySm(m_body->emit(b,end), m_distance));
    }
};

class FS_AbortExpr : public FS_Expr
{
public:
    virtual ~FS_AbortExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_AbortSm());
    }
};

//
//
// char matching
class FS_C_DigitExpr : public FS_Expr
{
public:
    virtual ~FS_C_DigitExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(FS_C_DigitSm::create(1));
    }
    virtual FS_StateMachine* emitSeq(const std::vector<FS_Expr*>& v, int& pos, FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        int n = 0;
        for(;pos<v.size() && dynamic_cast<FS_C_DigitExpr*>(v[pos]); ++pos)
        {
            ++n;
        }
        return end = b.add(FS_C_DigitSm::create(n));
    }
};

class FS_C_AlphaExpr : public FS_Expr
{
public:
    virtual ~FS_C_AlphaExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_C_AlphaSm());
    }
};

class FS_C_SpaceExpr : public FS_Expr
{
public:
    virtual ~FS_C_SpaceExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_C_SpaceSm());
    }
};

class FS_C_CharExpr : public FS_Expr
{
private:
    char m_c;
public:
    FS_C_CharExpr(char c):m_c(c){}
    virtual ~FS_C_CharExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end=b.add(FS_C_CharSm::create(m_c,b.matchCase()));
    }
    virtual FS_StateMachine* emitSeq(const std::vector<FS_Expr*>& v, int& pos, FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        string ch;
        for(;pos<v.size() && dynamic_cast<FS_C_CharExpr*>(v[pos]);++pos)
        {
            ch.append(1,((FS_C_CharExpr*)(v[pos]))->m_c);
        }
        return end = b.add(FS_C_CharSm::create(ch,b.matchCase()));
    }
};

class FS_C_EndExpr : public FS_Expr
{
public:
    virtual ~FS_C_EndExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_C_EndSm());
    }
    
};
class FS_C_AnyExpr : public FS_Expr
{
public:
    virtual ~FS_C_AnyExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_C_AnySm());
    }
};
class FS_C_StarExpr : public FS_Expr
{
public:
    virtual ~FS_C_StarExpr(){}
    virtual void visit(FS_Visitor& v){v.accept(*this);}
    virtual FS_StateMachine* emit(FS_Expr_Builder& b, FS_StateMachine*& end)
    {
        return end = b.add(new FS_C_StarSm());
    }
};


#endif /* defined(__textsearchtest__FS_Expr__) */
