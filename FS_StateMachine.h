/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__FS_StateMachine__
#define __textsearchtest__FS_StateMachine__

#include <iostream>
#include "TokenStream.h"

class FS_StateMachine;
class FS_NullSm;
class FS_ORSm;
class FS_StartRepeatSm;
class FS_LoopRepeatSm;
class FS_WordSm;
class FS_AnySm;
class FS_DigitsSm;
class FS_WildSm;
class FS_MarkSm;
class FS_ReturnSm;
class FS_PrecededBySm;
class FS_AbortSm;
class FS_C_EndSm;
class FS_C_DigitSm;
class FS_C_AlphaSm;
class FS_C_SpaceSm;
class FS_C_CharSm;
class FS_C_AnySm;
class FS_C_StarSm;

class FS_SM_Visitor
{
public:
    virtual void process(const FS_StateMachine& s){};
    virtual void visit(const FS_NullSm&);
    virtual void visit(const FS_ORSm&);
    virtual void visit(const FS_StartRepeatSm&);
    virtual void visit(const FS_LoopRepeatSm&);
    virtual void visit(const FS_WordSm&);
    virtual void visit(const FS_AnySm&);
    virtual void visit(const FS_DigitsSm&);
    virtual void visit(const FS_WildSm&);
    virtual void visit(const FS_MarkSm&);
    virtual void visit(const FS_ReturnSm&);
    virtual void visit(const FS_PrecededBySm&);
    virtual void visit(const FS_AbortSm&);
    virtual void visit(const FS_C_EndSm&);
    virtual void visit(const FS_C_DigitSm&);
    virtual void visit(const FS_C_AlphaSm&);
    virtual void visit(const FS_C_SpaceSm&);
    virtual void visit(const FS_C_CharSm&);
    virtual void visit(const FS_C_AnySm&);
    virtual void visit(const FS_C_StarSm&);
};


class MatchContext
{
private:
    unsigned m_repeatCounter20[20];
    unsigned* m_repeatCounters;
    int m_mark =0;
    bool m_abort = false;
public:
    struct MatchExtents
    {
        const char* m_matchStart = 0;
        const char* m_matchEnd = 0;
        inline void init(TokenStream::iterator& it)
        {
            m_matchStart = it.pos();
            m_matchEnd = it.pos()+it.size();
        }
        inline void set(TokenStream::iterator& it)
        {
            if (it.pos()<m_matchStart) m_matchStart = it.pos();
            if ((it.pos()+it.size())>m_matchEnd) m_matchEnd = it.pos()+it.size();
        }
    };
    
    MatchExtents m_match;
    
    MatchContext(int repNum, TokenStream::iterator& it)
    {
        m_mark = it.getPos();
        m_match.init(it);
        m_repeatCounters = m_repeatCounter20;
        if (repNum>20) m_repeatCounters = new unsigned[repNum];
    }
    ~MatchContext()
    {
        if (m_repeatCounters !=m_repeatCounter20) delete[] m_repeatCounters;
    }
    int setRepeatCounter(int index, unsigned value){unsigned c = m_repeatCounters[index];
        m_repeatCounters[index]=value; return c;}
    int incRepeatCounter(int index){return m_repeatCounters[index]++;}
    int getRepeatCounter(int index){return m_repeatCounters[index];}
    int setMark(int m){int old = m_mark; m_mark = m; return old;}
    int getMark(){return m_mark;}
    void setExtents(TokenStream::iterator& it, MatchExtents& old)
    {
        old = m_match;
        m_match.set(it);
    }
    void setExtents(TokenStream::iterator& it)
    {
        m_match.set(it);
    }
    void setExtents(MatchExtents& m)
    {
        m_match = m;
    }
    void abortMatch(){m_abort = true;}
    inline bool isAborted(){return m_abort;}
};


class FS_StateMachine
{
protected:
    FS_StateMachine* m_next = 0;
public:
    bool matchRecursiveBranch(TokenStream::iterator& it, MatchContext& c)
    {
        TokenStream::iterator::State state;
        it.getState(state);
        if (matchRecursive(it,c)) return true;
        it.setState(state);
        return false;
    }
    bool matchRecursive(TokenStream::iterator& it, MatchContext& c)
    {
        if (c.isAborted()) return false;
        FS_StateMachine* s = this;
        do
        {
            if (!s->matches(it,c)) return false;
            if (c.isAborted()) return false;// ? is this one necessary?
            s = s->m_next;
        }while(s);
        return true;
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)=0;
    virtual void addNext(FS_StateMachine* next){ASSERT(m_next==NULL); m_next = next;};
    virtual void accept(FS_SM_Visitor& v)=0;
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_next) m_next->accept(v);
    }
    virtual void replaceLink(const FS_StateMachine* old, FS_StateMachine* n)
    {
        if (m_next == old) m_next = n;
    }
protected:
    inline void absorbWhiteSpace(TokenStream::iterator& it)
    {
        if (it.isSpace()) it.next();
    }
};

class FS_NullSm : public FS_StateMachine
{
public:
    virtual bool matches(TokenStream::iterator& it, MatchContext& c){return true;}
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
    FS_StateMachine* getNext()const {return m_next;}
};

//
//
//
class FS_WordSm : public FS_StateMachine
{
public:
    static FS_WordSm* create(const std::string& s, bool matchCase, bool matchSpace);
    virtual const char* begin()const=0;
    virtual const char* end()const=0;
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}

};

template <bool MatchCase, bool MatchSpace, int NumChars>
class FS_WordSmImplN : public FS_WordSm
{
protected:
    char m_word[NumChars];
public:
    FS_WordSmImplN(const std::string& w)
    {
        memcpy(m_word,w.c_str(),NumChars);
    }
    
    virtual ~FS_WordSmImplN(){}
    virtual const char* begin()const{return m_word;}
    virtual const char* end()const{return m_word+NumChars;}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (!it.end()
            && matchWord(it))
        {
            if (m_next && it.next())
            {
                if (!MatchSpace) absorbWhiteSpace(it);
            }
            return true;
        }
        return false;
    }
    inline bool matchWord(TokenStream::iterator& it)
    {
        if (MatchCase)
        {
            return (NumChars==it.size()     && memcmp(m_word,it.pos(),NumChars)==0);
        } else {
            return (NumChars==it.lc_size()  && memcmp(m_word,it.lc_pos(),NumChars)==0);
        }
    }
};

template <bool MatchCase, bool MatchSpace>
class FS_WordSmImpl : public FS_WordSm
{
protected:
    char* m_word;
    unsigned m_size;
public:
    static FS_WordSm* create(const std::string& s)
    {
        switch (s.size())
        {
            case(1):{return new FS_WordSmImplN<MatchCase,MatchSpace,1>(s);}
            case(2):{return new FS_WordSmImplN<MatchCase,MatchSpace,2>(s);}
            case(3):{return new FS_WordSmImplN<MatchCase,MatchSpace,3>(s);}
            case(4):{return new FS_WordSmImplN<MatchCase,MatchSpace,4>(s);}
            case(5):{return new FS_WordSmImplN<MatchCase,MatchSpace,5>(s);}
            case(6):{return new FS_WordSmImplN<MatchCase,MatchSpace,6>(s);}
            default:{return new FS_WordSmImpl(s);}
        }
    }
    FS_WordSmImpl(const std::string& w)
    {
        m_size = w.size();
        m_word = new char[m_size];
        memcpy(m_word,w.c_str(),m_size);
    }
    
    virtual ~FS_WordSmImpl(){delete[] m_word;}
    virtual const char* begin()const{return m_word;}
    virtual const char* end()const{return m_word+m_size;}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (!it.end()
            && matchWord(it))
        {
            if (m_next && it.next())
            {
                if (!MatchSpace) absorbWhiteSpace(it);
            }
            return true;
        }
        return false;
    }
    inline bool matchWord(TokenStream::iterator& it)
    {
        if (MatchCase)
        {
            return (m_size==it.size()       && memcmp(m_word,it.pos(),m_size)==0);
        } else {
            return (m_size==it.lc_size()    && memcmp(m_word,it.lc_pos(),m_size)==0);
        }
    }
};

/*
class FS_WordSequenceSm : public FS_StateMachine
{
private:
    FS_WordSm* m_words[];
    
};*/

//
//
// todo: follow same procedure as FS_WordSm::create.
class FS_WildSm : public FS_StateMachine
{
protected:
    char* m_word;
    unsigned m_size;
public:
    FS_WildSm(const std::string& w)
    {
        m_size = w.size();
        m_word = new char[m_size+1];
        memcpy(m_word,w.c_str(),m_size);
        m_word[m_size] = 0;
    }
    virtual ~FS_WildSm()
    {
        delete[] m_word;
    }
    const char* begin()const{return m_word;}
    const char* end()const{return m_word+m_size;}
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
protected:
    // from http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html
    bool wildcardMatch(const char* pat, const char* str, const char* str_end) {
        const char* s;
        const char* p;
        bool star = false;
        
    loopStart:
        for (s = str, p = pat; s!=str_end; ++s, ++p) {
            switch (*p) {
                case '?':
                    if (*s == '.') goto starCheck;
                    break;
                case '*':
                    star = TRUE;
                    str = s, pat = p;
                    if (!*++pat) return TRUE;
                    goto loopStart;
                default:
                    if (*s != *p)
                        //   if (mapCaseTable[*s] != mapCaseTable[*p])
                        goto starCheck;
                    break;
            } /* endswitch */
        } /* endfor */
        if (*p == '*') ++p;
        return (!*p);
        
    starCheck:
        if (!star) return FALSE;
        str++;
        goto loopStart;
    }
};

template <bool MatchCase, bool MatchSpace>
class FS_WildSmImpl : public FS_WildSm
{
public:
    FS_WildSmImpl(const std::string& w):FS_WildSm(w)
    {
        if (!MatchCase)
        {
            for (int i=0; i<m_size; i++)
            {
                m_word[i] = tolower(m_word[i]);
            }
        }
    }
    virtual ~FS_WildSmImpl(){}
    
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (!it.end()
            && matchWord(it))
        {
            if (m_next && it.next())
            {
                if (!MatchSpace) absorbWhiteSpace(it);
            }
            return true;
        }
        return false;
    }
    inline bool matchWord(TokenStream::iterator& it)
    {
        if (MatchCase)
        {
            return (wildcardMatch(m_word,it.pos(),it.pos()+it.size()));
        } else {
            return (wildcardMatch(m_word,it.lc_pos(),it.lc_pos()+it.lc_size()));
        }
    }
};

class FS_ORSm : public FS_StateMachine
{
public:
    static FS_ORSm* create(int n);
};

template <int N>
class FS_ORSmImplN : public FS_ORSm
{
private:
    FS_StateMachine* m_sm[N];
public:
    FS_ORSmImplN()    {for (int i=0; i<N; i++){m_sm[i] =0;};}
    virtual void addNext(FS_StateMachine* next)
    {
        for (int i=0; i<N; i++)
        {
            if (m_sm[i] ==0)
            {
                m_sm[i]=next;
                return;
            }
        }
        ASSERT(true);
    }
    virtual void replaceLink(const FS_StateMachine* old, FS_StateMachine* n)
    {
        for (int i=0; i<N; i++)
        {
            if (m_sm[i] ==old)
            {
                m_sm[i]=n;
                return;
            }
        }
        FS_StateMachine::replaceLink(old,n);
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        int i=0;
        for (; i<(N-1); i++)
        {
            if (m_sm[i]->matchRecursiveBranch(it,c)) return true;
        }
        return (m_sm[i]->matchRecursive(it,c));
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
    virtual void recurse(FS_SM_Visitor& v)const
    {
        for (int i=0; i<N; i++)
        {
            m_sm[i]->accept(v);
        }
    }
};

class FS_ORSmImpl : public FS_ORSm
{
private:
    vector<FS_StateMachine*> m_sm;
public:
    virtual ~FS_ORSmImpl(){}
    virtual void addNext(FS_StateMachine* next)
    {
        m_sm.push_back(next);
    }
    virtual void replaceLink(const FS_StateMachine* old, FS_StateMachine* n)
    {
        for (int i=0; i<m_sm.size(); i++)
        {
            if (m_sm[i] ==old)
            {
                m_sm[i]=n;
                return;
            }
        }
        FS_StateMachine::replaceLink(old,n);
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        vector<FS_StateMachine*>::iterator sit = m_sm.begin();
        vector<FS_StateMachine*>::iterator sit_end = m_sm.end();
        for(; sit != sit_end; ++sit)
        {
            if ((*sit)->matchRecursiveBranch(it,c)) return true;
        }
        return false;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
    virtual void recurse(FS_SM_Visitor& v)const
    {
        vector<FS_StateMachine*>::const_iterator sit = m_sm.begin();
        vector<FS_StateMachine*>::const_iterator sit_end = m_sm.end();
        for(; sit != sit_end; ++sit)
        {
            (*sit)->accept(v);
        }
    }
};

//
//
class FS_StartRepeatSm : public FS_StateMachine
{
private:
    int m_repNum=0;
    FS_StateMachine* m_body = 0;
    FS_StateMachine* m_bypass = 0;
public:
    FS_StartRepeatSm(int i):m_repNum(i){}
    void setIndex(int i){m_repNum = i;}
    void setBody(FS_StateMachine* b){m_body=b;}
    void setBypass(FS_StateMachine*b){m_bypass=b;}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        int oldVal = c.setRepeatCounter(m_repNum, 0);
        bool ret = false;
        if (m_bypass)
        {
            if (m_bypass->matchRecursiveBranch(it,c)) ret = true;
        }
        if (!ret) ret = m_body->matchRecursive(it,c);
        c.setRepeatCounter(m_repNum,oldVal);
        return ret;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_bypass) m_bypass->accept(v);
        if (m_body) m_body->accept(v);
    }
};

class FS_LoopRepeatSm : public FS_StateMachine
{
private:
    int m_repNum=0;
    int m_max = -1;
    int m_min = 0;
    bool m_greedy = false;
    FS_StateMachine* m_body = 0;
    FS_StateMachine* m_cont = 0;
public:
    FS_LoopRepeatSm(int i, int min, int max, bool greedy):m_repNum(i),m_min(min),m_max(max),m_greedy(greedy){}
    void setBody(FS_StateMachine* b){m_body=b;}
    virtual void addNext(FS_StateMachine* s){m_cont = s;}
    virtual void replaceLink(const FS_StateMachine* old, FS_StateMachine* n)
    {
        if (m_cont == old) m_cont = n;
    }
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (m_greedy)
        {
            if (loopB(it,c)) return true;
            return cont(it,c);
        } else {
            if (contB(it,c)) return true;
            return loop(it,c);
        }
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_body) m_body->accept(v);
        if (m_cont) m_cont->accept(v);
        if (m_next) m_next->accept(v);
    }
private:
    bool loopB(TokenStream::iterator& it, MatchContext& c)
    {
        bool ret = false;
        int oldVal =c.incRepeatCounter(m_repNum);
        if (oldVal<m_max)
        {
            ret = m_body->matchRecursiveBranch(it,c);
        }
        c.setRepeatCounter(m_repNum,oldVal);
        return ret;
    }
    bool loop(TokenStream::iterator& it, MatchContext& c)
    {
        bool ret = false;
        int oldVal =c.incRepeatCounter(m_repNum);
        if (oldVal<m_max)
        {
            ret = m_body->matchRecursive(it,c);
        }
        c.setRepeatCounter(m_repNum,oldVal);
        return ret;
    }
    bool contB(TokenStream::iterator& it, MatchContext& c)
    {
        if (!m_cont) return true;
        return m_cont->matchRecursiveBranch(it,c);
    }
    bool cont(TokenStream::iterator& it, MatchContext& c)
    {
        if (!m_cont) return true;
        return m_cont->matchRecursive(it,c);
    }
};

class FS_AnySm : public FS_StateMachine{};

template <bool MatchSpace>
class FS_AnySmImpl : public FS_AnySm
{
public:
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (it.end()) return false;
        if (m_next && it.next()) {
            if (!MatchSpace) absorbWhiteSpace(it);
        }
        return true;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_DigitsSm : public FS_StateMachine{};

template <bool MatchSpace>
class FS_DigitsSmImpl : public FS_DigitsSm
{
public:
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        if (it.end()) return false;
        const char* pos = it.pos();
        const char* end = it.pos()+it.size();
        for (;pos != end; ++pos)
        {
            if (!isdigit(*pos)) return false;
        }
        if (m_next && it.next())
        {
            if (!MatchSpace) absorbWhiteSpace(it);
        }
        return true;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_MarkSm : public FS_StateMachine
{
private:
    FS_StateMachine* m_body = 0;
public:
    FS_MarkSm(FS_StateMachine* s){m_body = s;}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        int oldVal = c.setMark(it.getPos());
        bool ret = m_body->matchRecursive(it,c);
        c.setMark(oldVal);
        return ret;
    }
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_body) m_body->accept(v);
        FS_StateMachine::recurse(v);
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_ReturnSm : public FS_StateMachine
{
private:
    FS_StateMachine* m_body = 0;
public:
    FS_ReturnSm(FS_StateMachine* s):m_body(s){}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        int oldPos = it.getPos();
        MatchContext::MatchExtents ext;
        c.setExtents(it, ext);
        it.setPos(c.getMark());
        bool ret = m_body->matchRecursive(it,c);
        it.setPos(oldPos);
        if (!ret) c.setExtents(ext);
        return ret;
    }
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_body) m_body->accept(v);
        FS_StateMachine::recurse(v);
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}

};

// %precededby[5]%{}
class FS_PrecededBySm : public FS_StateMachine
{
private:
    int m_distance = 5;
    FS_StateMachine* m_body = 0;
public:
    FS_PrecededBySm(FS_StateMachine* s, int d){m_body = s;m_distance=d;}
    void setDistance(int d){m_distance = d;}
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        bool ret = false;
        int oldPos = it.getPos();
        it.prev();
        if (!m_next) oldPos = it.getPos();
        
        MatchContext::MatchExtents ext;
        c.setExtents(it, ext);
        it.setPos(c.getMark());
        for (int i=0; !ret && i<=m_distance && it.prev(); i++)
        {
            c.setExtents(it);
            ret= m_body->matchRecursiveBranch(it,c);
        }
        
        it.setPos(oldPos);
        if (!ret) c.setExtents(ext);
        return ret;
    }
    virtual void recurse(FS_SM_Visitor& v)const
    {
        if (m_body) m_body->accept(v);
        FS_StateMachine::recurse(v);
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

class FS_AbortSm : public FS_StateMachine
{
public:
    virtual bool matches(TokenStream::iterator& it, MatchContext& c)
    {
        c.abortMatch();
        return false;
    }
    virtual void accept(FS_SM_Visitor& v){v.visit(*this);}
};

#endif /* defined(__textsearchtest__FS_StateMachine__) */
