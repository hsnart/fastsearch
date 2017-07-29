/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__InvertedIndex__
#define __textsearchtest__InvertedIndex__

#include <iostream>
#include <map>
#include <vector>
#include "Tokenizer.h"
#include "Simple9.hpp"

using namespace std;

class TokenStream;

class InvertedIndex
{
protected:
    int m_tableSize = 51;
    vector<pair<uint32_t,uint32_t> > m_index; // finalIndex, size
    vector<uint32_t> m_data; // indexes into the final data.
    void init(int tableSize);
public:
    class Builder
    {
    protected:
        int m_tableSize = 51;
        vector<vector<uint32_t> > m_tmpIndex;
    public:
        Builder(int tableSize = 51);
        void add(unsigned key, unsigned value);
        InvertedIndex* complete();
        void dump(char key);
    protected:
        void complete(InvertedIndex* index);
    };
    
    class iterator
    {
    private:
        // data to decode
        const uint32_t* m_it;
        uint32_t* m_it_end;
        
        // decoded ints
        uint32_t m_ints[30]; // current batch of decompressed ints. Simple9 does at most 28 in a batch.
        size_t m_numInts = 0;
        int m_pos = 0;
        
        // current value
        uint32_t m_val = 0;
    public:
        iterator(){}
        void init(uint32_t* it, uint32_t* it_end);
        inline unsigned get(){return m_val;}
        inline bool next(){
            static integer_encoding::internals::Simple9 m_comp;
            ++m_pos;
            if (m_pos>=m_numInts)
            {
                if (m_it == m_it_end) return false;
                // get next batch
                uint32_t* pos = m_ints;
                m_comp.decodeBatch(m_it, m_it_end-m_it, pos, 30);
                m_numInts = pos-m_ints;
                m_pos = 0;
                if (m_it == m_it_end)
                {
                    // there seems to always be an extra 0 decoded at the end of the sequence.
                    while (m_numInts>0 && m_ints[m_numInts-1] ==0){
                        m_numInts--;
                    }
                }
         //       if (m_numInts ==0) return false; // not possible.. will always have data
            }
            m_val += m_ints[m_pos];
            return true;
        }
    };
    InvertedIndex();
    
    bool get(unsigned key, iterator& it);
    
    void dump(char key);
};

// same 2 char hash based prefix index used in APM.
class PrefixIndex: public InvertedIndex
{
public:
    static unsigned int prefixHash(const char* wordStart, const char* wordEnd);
    class Builder: public InvertedIndex::Builder
    {
    public:
        Builder(TokenStream& text);
        PrefixIndex* complete();
    };
    bool get(const char* word, iterator& it)
    {
        return get(word,word+strlen(word),it);
    }
    bool get(const char* wordStart, const char* wordEnd, iterator& it);
    bool get(unsigned int prefixHash, iterator& it);
};

class WordIndex: public InvertedIndex
{
public:
    static unsigned int hash(const char* wordStart, const char* wordEnd);
    class Builder: public InvertedIndex::Builder
    {
    public:
        Builder(TokenStream& text);
        WordIndex* complete();
    };
    bool get(const char* word, iterator& it)
    {
        return get(word,word+strlen(word),it);
    }
    bool get(const char* wordStart, const char* wordEnd, iterator& it);
    bool get(unsigned int wordHash, iterator& it);
};

#endif /* defined(__textsearchtest__InvertedIndex__) */
