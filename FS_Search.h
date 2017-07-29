/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__FS_Search__
#define __textsearchtest__FS_Search__

#include <iostream>
#include <vector>
#include <set>
#include "FS_Expr.h"
#include "Tokenizer.h"
#include "TokenStream.h"

#define F_IN cout<<__PRETTY_FUNCTION__<<endl;

using namespace std;


class FS_Expression
{
private:
    FS_StateMachine* m_smRoot;
    // todo: Replace this with a single block allocated memory,
    // then can just delete the lot in 1 go.
    // and removes memory fragmentation.
    std::vector<FS_StateMachine*> m_sms;
    int m_numRepeats = 0;
    std::vector<StartPoint> m_startPoints;
public:
    FS_Expression(string e){init(e);}
    ~FS_Expression()
    {
        clearPtrVector(m_sms);
    }
    class MatchCallback
    {
    public:
        // return true if match allowed.
        virtual bool match(const char* matchStart, const char* matchEnd){return true;};
        virtual bool filterStart(unsigned pos){return true;};
      //  virtual bool filterMatch(unsigned startPos, unsigned endPos){return true;}
    };
    
    void accept(FS_SM_Visitor& v)
    {
        m_smRoot->accept(v);
    }
    unsigned count(TokenStream& doc, MatchCallback* callback = 0)
    {
        unsigned matchCount = 0;
        if (numStartPoints() ==0)
        {
            // no start points defined.
            // full doc trawl
            TokenStream::iterator it = doc.begin();
            for (; !it.end();it.next())
            {
                if (!it.isSpace())
                {
                    int pos = it.getPos();
                    if (matches(it,callback))
                    {
                        matchCount++;
                    }
                    it.setPos(pos);
                }
            }
        } else {
            InvertedIndex::iterator pit;
            TokenStream::iterator it;
            for (int sp = 0; sp<numStartPoints(); sp++)
            {
                if (getStartPointIterator(sp,doc,pit))// go through expression start points
                {
                    while(pit.next()){
                        doc.at(pit.get(),it);
                       // if (callback->filterStart(startPos))
                        {
                            if (matches(it,callback))
                            {
                                matchCount++;
                            }
                        }
                    }
                }
            }
        }
        return matchCount;
    }
    
    class iterator
    {
    private:
        TokenStream& m_doc;
        FS_Expression& m_e;
        bool m_searchAll = false;
        TokenStream::iterator it;
        int startPointPos = 0;
        InvertedIndex::iterator pit;
        bool m_nothingToSearch = false;
    public:
        iterator(TokenStream& doc, FS_Expression& e):m_doc(doc),m_e(e)
        {
            if (m_e.numStartPoints() ==0)
            {
                m_searchAll = true;
                it = doc.begin();
            } else {
                m_nothingToSearch = true;
                for (; startPointPos<m_e.numStartPoints(); startPointPos++)
                {
                    if (m_e.getStartPointIterator(startPointPos,m_doc,pit))// go through expression start points
                    {
                        m_nothingToSearch = false;
                        break;
                    }
                }
            }
        }
        bool next()
        {
            if (m_searchAll)
            {
                while (it.next())
                {
                    TokenStream::iterator tit = it;
                    if (m_e.matches(tit, NULL)) return true;
                }
                return false;
            } else {
                if (m_nothingToSearch) return false;
                if (next1()) return true;
                for (; startPointPos<m_e.numStartPoints(); startPointPos++)
                {
                    if (m_e.getStartPointIterator(startPointPos,m_doc,pit))// go through expression start points
                    {
                        if (next1()) return true;
                    }
                }
                return false;
            }
        }
    private:
        bool next1()
        {
            while(pit.next()){
                int startPos = pit.get();
                TokenStream::iterator tit = m_doc.at(startPos);
                if (tit.next())
                {
                    if (m_e.matches(tit,NULL)) return true;
                }
            }
            return false;
        }
    };
    
    bool matches(TokenStream::iterator& it, MatchCallback* callback)
    {
        MatchContext c(m_numRepeats,it);
        if (m_smRoot->matchRecursive(it,c) && !c.isAborted())
        {
            if (callback)
            {
                c.setExtents(it);
                return callback->match(c.m_match.m_matchStart, c.m_match.m_matchEnd);
            }
            return true;
        }
        return false;
    }
    int numStartPoints()
    {
        return m_startPoints.size();
    }
    bool getStartPointIterator(int i, TokenStream& text, InvertedIndex::iterator& it)
    {
        switch (m_startPoints[i].m_type)
        {
            case StartPoint::Any:
            {
                break;
            }
            case StartPoint::Word:
            {
                return text.getWordIndex()->get(m_startPoints[i].m_hash, it);
                break;
            }
            case StartPoint::Prefix:
            {
                return text.getPrefixIndex()->get(m_startPoints[i].m_hash, it);
                break;
            }
        }
        return false;
    }
private:
    
    class FS_StartPointsVisitor : public FS_Visitor
    {
    private:
        bool m_any = false;
        vector<const FS_Expr*> m_startPoints;
    public:
        virtual void accept(const FS_SequenceExpr& e);
        virtual void accept(const FS_ORExpr& e);
        virtual void accept(const FS_RepeatExpr& e);
        virtual void accept(const FS_TokenExpr& e);
        virtual void accept(const FS_WordExpr& e);
        virtual void accept(const FS_AnyExpr& e);
        virtual void accept(const FS_RegionExpr&);
        virtual void accept(const FS_DigitsExpr&);
        virtual void accept(const FS_WildExpr&);
        virtual void accept(const FS_MarkExpr&);
        virtual void accept(const FS_ReturnExpr&);
        virtual void accept(const FS_PrecededByExpr&);
        virtual void accept(const FS_AbortExpr&);
        virtual void accept(const FS_MatchCaseExpr&);
        virtual void accept(const FS_C_DigitExpr&);
        virtual void accept(const FS_C_AlphaExpr&);
        virtual void accept(const FS_C_CharExpr&);
        virtual void accept(const FS_C_SpaceExpr&);
        virtual void accept(const FS_C_EndExpr&);
        virtual void accept(const FS_C_AnyExpr&);
        virtual void accept(const FS_C_StarExpr&);
        
        
        void getStartPoints(vector<StartPoint>& sp);
    };
    
    class FS_OptimiseVisitor : public FS_SM_Visitor
    {
    private:
        std::set<const FS_StateMachine*> m_visited;
        std::vector<const FS_StateMachine*> m_stack;
    public:
        virtual void process(const FS_StateMachine& s);
        virtual void visit(const FS_NullSm& s);
    };
    
    void init(string e)
    {
        Tokenizer tok(e.c_str(), e.c_str()+e.length());
        vector<FS_Expr*> stack;
        parse(tok,stack);
        FS_Expr* root = FS_SequenceExpr::create(stack);
        // find the start points.
        FS_StartPointsVisitor spv;
        root->visit(spv);
        spv.getStartPoints(m_startPoints);
        FS_Expr_Builder b;
        FS_StateMachine* end = NULL;
        m_smRoot = root->emit(b, end);
        FS_OptimiseVisitor ov;
        m_smRoot->accept(ov);
        delete root;
        m_numRepeats = b.getNumRepeats();
        b.takeExprs(m_sms);
    }
    
    void parse(Tokenizer& tok,vector<FS_Expr*>& stack) {
        parseTerms(tok,stack) ;
        
        if (tok.more() && tok.eat('|')) {
            vector<FS_Expr*> rhs_stack;
            parse(tok,rhs_stack);
            FS_ORExpr::create(stack,rhs_stack);
        }
        if (tok.more() && tok.eat('[')) {
            // parse repeat
            // [1,2] [3]
            int repMin = 0;
            int repMax = -1;
            bool greedy = false;
            if (tok.eat('*'))//ifEat?
            {
            } else if (tok.eat('?'))
            {
                repMax = 1;
            } else if (tok.eat('+'))
            {
                repMin = 1;
            } else {
                if (tok.eat(','))
                {
                    repMax = tok.getInt();  tok.next();
                } else {
                    repMin= tok.getInt();  repMax = repMin;     tok.next();
                    if (tok.eat(','))
                    {
                        repMax = tok.getInt();  tok.next();
                    }
                }
            }
            if (tok.eat('g'))
            {
                greedy = true;
            }
            tok.eat(']');
            vector<FS_Expr*> rhs_stack;
            parse(tok,rhs_stack);//!! wrong
            // need to create the repeat then add it to stack, else [4][4] wont work.
            FS_RepeatExpr::create(stack, rhs_stack, repMin,repMax,greedy);
        }
    }
    
    void parse_c(Tokenizer& tok,vector<FS_Expr*>& stack) {
        parseTerms_c(tok,stack) ;
        
        if (tok.more() && tok.eat('|')) {
            vector<FS_Expr*> rhs_stack;
            parse_c(tok,rhs_stack);
            FS_ORExpr::create(stack,rhs_stack);
        }
        if (tok.more() && tok.eat('[')) {
            // parse repeat
            // [1,2] [3]
            int repMin = 0;
            int repMax = -1;
            bool greedy = false;
            if (tok.eat('*'))//ifEat?
            {
            } else if (tok.eat('?'))
            {
                repMax = 1;
            } else if (tok.eat('+'))
            {
                repMin = 1;
            } else {
                if (tok.eat(','))
                {
                    repMax = tok.getInt();  tok.next();
                } else {
                    repMin= tok.getInt();  repMax = repMin;     tok.next();
                    if (tok.eat(','))
                    {
                        repMax = tok.getInt();  tok.next();
                    }
                }
            }
            if (tok.eat('g'))
            {
                greedy = true;
            }
            tok.eat(']');
            vector<FS_Expr*> rhs_stack;
            parse_c(tok,rhs_stack);//!! wrong
            // need to create the repeat then add it to stack, else [4][4] wont work.
            FS_RepeatExpr::create(stack, rhs_stack, repMin,repMax,greedy);
        }
      /*  if (tok.more() && tok.eat('*'))
        {
            while (tok.eat('*')){}
          //  vector<FS_Expr*> rhs_stack;
          //  parse_c(tok,rhs_stack);
            stack.push_back(new FS_C_StarExpr());
        }*/
    }
    
    bool isOperator(char c)
    {
        return (c=='|' || c=='[');
    }
    
    void parseTerms(Tokenizer& tok, vector<FS_Expr*>& stack) {
        while (tok.more() && *tok.begin() != '}' && !isOperator(*tok.begin())) {
            parseTerm(tok,stack);
        }
    }
    
    void parseTerms_c(Tokenizer& tok, vector<FS_Expr*>& stack) {
        while (tok.more() && *tok.begin() != '%' && *tok.begin() != '}' && !isOperator(*tok.begin())) {
            parseTerm_c(tok,stack);
        }
    }
    
    FS_Expr* parseSpecial(Tokenizer& tok, vector<FS_Expr*>& stack)
    {
        if (tok.is("any"))
        {
            tok.next();tok.eat('%');
            return new FS_AnyExpr();
        } else if (tok.is("digits"))
        {
            tok.next();tok.eat('%');
            return new FS_DigitsExpr();
        } else if (tok.is("abort") || tok.is("false"))
        {
            tok.next();tok.eat('%');
            return new FS_AbortExpr();
        } else if (tok.is("mark"))
        {
            tok.next();tok.eat('%');
            vector<FS_Expr*> rhs_stack;
            parse(tok,rhs_stack);
            return FS_MarkExpr::create(rhs_stack);
        } else if (tok.is("return"))
        {
            tok.next();tok.eat('%');
            vector<FS_Expr*> rhs_stack;
            parse(tok,rhs_stack);
            return FS_ReturnExpr::create(rhs_stack);
        } else if (tok.is("precededby"))
        {
            int distance = 0;
            tok.next();
            if (tok.eat('['))
            {
                distance =tok.getInt();
                tok.next();
                tok.eat(']');
            }
            tok.eat('%');
            vector<FS_Expr*> rhs_stack;
            parse(tok,rhs_stack);
            return FS_PrecededByExpr::create(rhs_stack,distance);
        } else if (tok.is("+"))
        {
            tok.next();
            if (tok.is("matchcase"))
            {
                // turn on case matching.
                tok.next();tok.eat('%');
                vector<FS_Expr*> rhs_stack;
                parse(tok,rhs_stack);
                return FS_MatchCaseExpr::create(rhs_stack,true);
            }
        } else if (tok.is("-"))
        {
            tok.next();
            if (tok.is("matchcase"))
            {
                // turn on case matching.
                tok.next();tok.eat('%');
                vector<FS_Expr*> rhs_stack;
                parse(tok,rhs_stack);
                return FS_MatchCaseExpr::create(rhs_stack,false);
            }
        } else if (tok.is("#"))
        {
            // find next unescaped %.
            tok.next();
            parse_c(tok,stack);
            tok.eat('%');
            return new FS_C_EndExpr();
        }
        
        throw std::runtime_error(std::string("unknown special token:'")+std::string(tok.begin(),tok.end())+"'");
        return NULL;
    }
    
    void parseTerm(Tokenizer& tok, vector<FS_Expr*>& stack) {
        
        switch (tok.peek()) {
            case '{':
            {
                tok.eat('{');
                vector<FS_Expr*> rhs_stack;
                parse(tok,rhs_stack);
                stack.push_back(FS_SequenceExpr::create(rhs_stack));
                tok.eat('}');// eat )
                break;
            }
            case '%':
            {
                tok.eat('%');
                stack.push_back(parseSpecial(tok, stack));
                break;
            }
            default:
            {
                if (!isspace(tok.peek()))
                {
                    if (tok.isWild())
                    {
                        stack.push_back(new FS_WildExpr(tok.begin(), tok.end()));
                    } else {
                        stack.push_back(new FS_WordExpr(tok.begin(), tok.end()));
                    }
                }
                tok.next();
            }
        }
    }
    
    void parseTerm_c(Tokenizer& tok, vector<FS_Expr*>& stack) {
        
        switch (tok.peek()) {
            case '{':
            {
                tok.eat('{');
                vector<FS_Expr*> rhs_stack;
                parse_c(tok,rhs_stack);
                stack.push_back(FS_SequenceExpr::create(rhs_stack));
                tok.eat('}');// eat )
                break;
            }
            case '\\':
            {
                tok.eat('\\');
                switch(tok.peek())
                {
                    case 'd':
                    {
                        stack.push_back(new FS_C_DigitExpr()); break;
                    }
                    case 'a':
                    {
                        stack.push_back(new FS_C_AlphaExpr()); break;
                    }
                    default:
                    {
                        stack.push_back(new FS_C_CharExpr(*tok.begin()));
                        break;
                    }
                }
                tok.nextChar();
                break;
            }
            case '?':
            {
                stack.push_back(new FS_C_AnyExpr()); tok.nextChar(); break;
            }
            case '*':
            {
                stack.push_back(new FS_C_StarExpr()); tok.nextChar(); break;
            }
            case ' ':
            {
                stack.push_back(new FS_C_SpaceExpr()); tok.nextChar(); break;
            }
            default:
            {
                stack.push_back(new FS_C_CharExpr(*tok.begin()));  tok.nextChar();  break;
            }
        }
    }

};

#endif /* defined(__textsearchtest__FS_Search__) */
