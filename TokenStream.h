/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__TokenStream__
#define __textsearchtest__TokenStream__

#include <iostream>
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "InvertedIndex.h"

using namespace std;


//
// A tokenised version of a text stream
// can support rich tokens,
// and indexes.
class TokenStream
{
private:
    
    struct Token{
        // 0 = lowercase
        // 1 = uppercase
        // 2 = space
        // 3 = special {formatting etc}
        unsigned tokenType : 2;
        unsigned size : 6;
    };
    union TokenChar{
        Token t;
        char c;
    };
    const char* m_origonalText;
    const char* m_origonalTextEnd;
    char* m_tokens;
    int m_numTokens;
    boost::dynamic_bitset<> m_tokenBoundaries;
    uint32_t* m_offsets; // written every N bytes of tokens.
    static const int OFFSET_CHUNK_SIZE = 256;
    PrefixIndex* m_prefixIndex;
    WordIndex* m_wordIndex;
public:
    class iterator
    {
    private:
        TokenStream* m_master;
        uint32_t m_nextPos;
        bool m_end = false;
        
        // current token info
        uint32_t m_tokenSize;
        const char* m_tokenStart;
        uint32_t m_lc_tokenSize;
        const char* m_lc_tokenStart;
        bool m_upper;
        bool m_space;
    public:
        struct State
        {
            uint32_t m_pos = 0;
            uint32_t m_charPos = 0;
            bool operator==(const State& right)
            {
                return (m_pos == right.m_pos && m_charPos == right.m_charPos);
            }
        }m_state;
        iterator(){}
        iterator(TokenStream& master, unsigned tok_pos)     {init(master,tok_pos);}
        void init(TokenStream& master, unsigned tok_pos)    { m_master = &master;m_state.m_pos=tok_pos; expand();}
        const char* pos()   {return m_tokenStart;}          // origonal case text. Normalised w.r.t. char differences.
        const char* lc_pos(){return m_lc_tokenStart;}       // lowercase token start
        uint32_t size()          {return m_tokenSize;}           // size of this token.
        uint32_t lc_size()       {return m_lc_tokenSize;}        // size of this token.
        bool isUpper()      {return m_upper;}               // does token contain uppercase?
        bool isSpace()      {return m_space;}
        bool end()          {return m_end;}                 // end of token stream?
        
        char c_pos()        {return m_tokenStart[m_state.m_charPos];}       // next char
        char c_lc_pos()     {return m_lc_tokenStart[m_state.m_charPos];}    // lowercase char
        bool c_end()        {return m_state.m_charPos==0;}                  // end of word
        bool c_lc_end()     {return m_state.m_charPos==0;}                  // end of lowercase word
        
        
        inline bool next()__attribute((always_inline))
        {
            if (m_nextPos == m_master->m_numTokens)
            {
                m_end = true;
                return false;
            }
            m_state.m_charPos = 0;
            m_state.m_pos = m_nextPos;
            expand();
            return true;
        }
        
        bool nextChar()
        {
            m_state.m_charPos++;
            if (m_state.m_charPos == m_tokenSize)
            {
                return next();
            }
            return true;
        }
        
        bool prev()
        {
            if (m_state.m_pos==0) return false;
            if (m_end)
            {
                m_end = false;
                return true;
            }
            unsigned size = m_master->m_numTokens;
            boost::dynamic_bitset<>::size_type p =m_master->m_tokenBoundaries.find_next(size - m_state.m_pos);
            if (p == boost::dynamic_bitset<>::npos) return false;
            setPos(size - p);
            return true;
        }
        
        inline uint32_t getPos(){return m_state.m_pos;};
        inline void setPos(unsigned p)__attribute((always_inline))
        {
            if (p!= m_state.m_pos)
            {
                m_state.m_pos = p; expand();
                m_end = false;
            }
        };
        
        inline void getState(State& s){s = m_state;}
        inline void setState(State& s){if (!(s == m_state)){m_state = s; m_end = false; expand();}}
        
    private:
        inline void expand()__attribute((always_inline))
        {
            const char* t = m_master->m_tokens;
            unsigned char c;
            TokenChar tc;
            tc.c = t[m_state.m_pos];
            
            m_tokenSize = tc.t.size;
            m_tokenStart = m_master->m_origonalText+m_master->m_offsets[m_state.m_pos/OFFSET_CHUNK_SIZE];
            unsigned p = m_state.m_pos+1;
            {
                unsigned shift = 0;
                do{
                    c = t[p++];
                    m_tokenStart += (c&127)<<shift;
                    shift+=7;
                }while (c&128);
            }
            
            switch (tc.t.tokenType)
            {
                case 0:
                {
                    m_upper = false;
                    m_space = false;
                    m_lc_tokenSize = m_tokenSize;
                    m_lc_tokenStart = m_tokenStart;
                    break;
                };
                case 1:
                {
                    m_upper = true;
                    m_space = false;
                    tc.c = t[p++];
                    m_lc_tokenStart =t + p;
                    m_lc_tokenSize = tc.t.size;
                    p+= m_lc_tokenSize;
                    break;
                }
                case 2:
                {
                    m_space = true;
                    m_upper = false;
                    break;
                }
                case 3:
                {
                    // special token.
                    // expand next one to see exactly what.
                    break;
                }
            };
            m_nextPos = p;
        }
    };

    friend class iterator;
    TokenStream();
    TokenStream(const string& s);
    TokenStream(const char* start, const char* end);
    ~TokenStream();
    iterator begin();
    iterator at(int pos);
    void at(int pos,iterator& it);
    void init(const char* start, size_t size);
    
    string getSnippet(int startToken, int endToken);
    PrefixIndex* getPrefixIndex();
    WordIndex* getWordIndex();
};



#endif /* defined(__textsearchtest__TokenStream__) */
