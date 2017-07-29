/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__FS_Char_StateMachine__
#define __textsearchtest__FS_Char_StateMachine__

#include <iostream>
#include "FS_StateMachine.h"

class FS_C_EndSm : public FS_StateMachine
{
public:
    virtual ~FS_C_EndSm(){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (!it.end() && !it.c_end()) return false;
        if (!m_next)// end of match. Position cursor over last matched token.
        {
            it.prev();// is there a better way to do this? This is a fixup procedure.
        } else {
            absorbWhiteSpace(it);
        }
        return true;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_C_DigitSm : public FS_StateMachine
{
protected:
    FS_C_DigitSm(){};
public:
    static FS_C_DigitSm* create(int n);
    virtual ~FS_C_DigitSm(){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (it.end()) return false;
        if (isdigit(it.c_pos()))
        {
            it.nextChar();
            return true;
        }
        return false;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_C_DigitSmN : public FS_C_DigitSm
{
private:
    int m_n;
public:
    FS_C_DigitSmN(int n):m_n(n){}
    virtual ~FS_C_DigitSmN(){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        for (int i=0; i<m_n; i++)
        {
            if (it.end()) return false;
            if (!isdigit(it.c_pos())) return false;
            it.nextChar();
        }
        return true;
    }
};
        
class FS_C_AlphaSm : public FS_StateMachine
{
public:
    virtual ~FS_C_AlphaSm(){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (it.end()) return false;
        if (isalpha(it.c_pos()))
        {
            it.nextChar();
            return true;
        }
        return false;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_C_AnySm : public FS_StateMachine
{
public:
    virtual ~FS_C_AnySm(){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (it.end()) return false;
        if (!it.isSpace())
        {
            it.nextChar();
            return true;
        }
        return false;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_C_SpaceSm : public FS_StateMachine
{
public:
    virtual ~FS_C_SpaceSm(){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (it.end()) return false;
        if (it.isSpace())
        {
            it.next();
            return true;
        }
        return false;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_C_CharSm : public FS_StateMachine
{
public:
    static FS_C_CharSm* create(char c, bool matchCase);
    static FS_C_CharSm* create(const std::string& c, bool matchCase);
    virtual ~FS_C_CharSm(){}
    virtual const char* begin()const=0;
    virtual const char* end()const=0;
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};


template <bool MatchCase, int NumChars>
class FS_C_CharSmImplN : public FS_C_CharSm
{
private:
    char m_c[NumChars];
protected:
    FS_C_CharSmImplN(const std::string& s)
    {
        memcpy(m_c,s.c_str(),NumChars);
    }
public:
    static FS_C_CharSm* create(const std::string& s)
    {
        return new FS_C_CharSmImplN(s);
    }
    // TODO when dealing with utf 8 char is not right. Need a utf8 char.
    
    virtual ~FS_C_CharSmImplN(){}
    virtual const char* begin()const{return m_c;};
    virtual const char* end()const{return m_c+NumChars;};
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        for (int i=0; i<NumChars; i++)
        {
            if (it.end()) return false;
            if (MatchCase)
            {
                if (it.c_pos()!=m_c[i]) return false;
            } else {
                if (it.c_lc_pos()!=m_c[i]) return false;
            }
            it.nextChar();
        }
        return true;
    }
};


template <bool MatchCase>
class FS_C_CharSmImpl : public FS_C_CharSm
{
private:
    char* m_c;
    int m_size;
protected:
    FS_C_CharSmImpl(const std::string& w)
    {
        m_size = w.size();
        m_c = new char[m_size];
        memcpy(m_c,w.c_str(),m_size);
    }
public:
    static FS_C_CharSm* create(const std::string& c)
    {
        switch (c.size())
        {
            case(1): return FS_C_CharSmImplN<MatchCase,1>::create(c);
            case(2): return FS_C_CharSmImplN<MatchCase,2>::create(c);
            case(3): return FS_C_CharSmImplN<MatchCase,3>::create(c);
            case(4): return FS_C_CharSmImplN<MatchCase,4>::create(c);
            case(5): return FS_C_CharSmImplN<MatchCase,5>::create(c);
            case(6): return FS_C_CharSmImplN<MatchCase,6>::create(c);
            case(7): return FS_C_CharSmImplN<MatchCase,7>::create(c);
            case(8): return FS_C_CharSmImplN<MatchCase,8>::create(c);
            case(9): return FS_C_CharSmImplN<MatchCase,9>::create(c);
            case(10): return FS_C_CharSmImplN<MatchCase,10>::create(c);
            case(11): return FS_C_CharSmImplN<MatchCase,11>::create(c);
        }
        return new FS_C_CharSmImpl(c);
    }
    // TODO when dealing with utf 8 char is not right. Need a utf8 char.
    
    virtual ~FS_C_CharSmImpl()
    {
        delete m_c;
    }
    virtual const char* begin()const{return m_c;};
    virtual const char* end()const{return m_c+m_size;};
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        for (int i=0; i<m_size; i++)
        {
            if (it.end()) return false;
            if (MatchCase)
            {
                if (it.c_pos()!=m_c[i]) return false;
            } else {
                if (it.c_lc_pos()!=m_c[i]) return false;
            }
            it.nextChar();
        }
        return true;
    }
};

class FS_C_StarSm : public FS_StateMachine
{
private:
    FS_StateMachine* m_cont;
public:
    FS_C_StarSm():m_cont(0){}
    virtual ~FS_C_StarSm(){}
    virtual void addNext(FS_StateMachine* s){m_cont = s;}
    virtual void replaceLink(const FS_StateMachine* old, FS_StateMachine* n)
    {
        if (m_cont == old) m_cont = n;
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        while (!it.end() && !it.isSpace())
        {
            if (!m_cont || m_cont->matchRecursiveBranch(it,c)) return true;
            it.nextChar();
        }
        return (!m_cont || m_cont->matchRecursive(it,c));
    }
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_cont) m_cont->accept(v);
        FS_StateMachine::recurse(v);
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};


class FS_C_CharRangeSM : public FS_StateMachine
{
private:
    struct Range
    {
        int m_from;
        int m_to;
        bool matches(int c)
        {
            return (c>=m_from && c<=m_to);
        }
    };
    std::vector<Range> m_ranges;
    std::string m_ch;
public:
    void addChars(std::string& ch)
    {
        m_ch.append(ch);
    }
    void addRange(int from, int to)
    {
        Range r;
        r.m_from = from;
        r.m_to = to;
        m_ranges.push_back(r);
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (!it.end())
        {
            char c = it.c_lc_pos();
            if (m_ch.size()>0 && m_ch.find(c) != string::npos)
            {
                it.nextChar();
                return true;
            }
            for (int i = 0; i<m_ranges.size(); i++)
            {
                if (m_ranges[i].matches(c))
                {
                    it.nextChar();
                    return true;
                }
            }
            return false;
        }
        while (!it.end() && !it.isSpace())
        {
            if (!m_cont || m_cont->matchRecursiveBranch(it,c)) return true;
            it.nextChar();
        }
        return (!m_cont || m_cont->matchRecursive(it,c));
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};






#endif /* defined(__textsearchtest__FS_Char_StateMachine__) */
