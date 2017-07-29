/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "TokenStream.h"

using namespace std;

// TokenStream

//class TokenStream
TokenStream::TokenStream(){};
TokenStream::TokenStream(const string& s)
{
    m_origonalText = s.c_str();
    m_origonalTextEnd = m_origonalText+s.size();
    init(s.c_str(),s.size());
}

TokenStream::TokenStream(const char* start, const char* end)
{
    m_origonalText = start;
    m_origonalTextEnd = end;
    init(start,end-start);
}
TokenStream::~TokenStream()
{
    if (m_tokens) delete m_tokens;
    if (m_offsets)delete m_offsets;
    if (m_prefixIndex) delete m_prefixIndex;
    if (m_wordIndex) delete m_wordIndex;
}
TokenStream::iterator TokenStream::begin()
{
    return iterator(*this, 0);
}
TokenStream::iterator TokenStream::at(int pos)
{
    return iterator(*this, pos);
}
void TokenStream::at(int pos, iterator& it)
{
    it.init(*this,pos);
}
void TokenStream::init(const char* start, size_t size)
{
    vector<char> vc;
    vector<uint32_t> offsets;
    unsigned lastOffset = 0;
    offsets.push_back(0);
    Tokenizer tok(start,start+size);
    while (tok.more())
    {
        if (vc.size()/OFFSET_CHUNK_SIZE >= offsets.size())
        {
            lastOffset = (unsigned)(tok.begin() - start);
            offsets.push_back(lastOffset);
        }
        TokenChar tc;
        ASSERT(tok.size()<64); // implement extended tokens you lazy person.
        tc.t.size = tok.size();
        if (tok.isSpace())
        {
            tc.t.tokenType = 2;
        } else if (tok.isUpper())
        {
            tc.t.tokenType = 1;
        } else {
            tc.t.tokenType = 0;
        }
        vc.push_back(tc.c);
        
        // write vint of index into origonal stream.
        {
            size_t tokenStartPos = tok.begin() - start - lastOffset;
            
            do{
                unsigned char p = tokenStartPos&127;
                tokenStartPos = tokenStartPos>>7;
                if (tokenStartPos >0) p += 128;
                vc.push_back(p);
            }while (tokenStartPos>0);
            
        }
        if (tok.isUpper())
        {
            // store lowercase variant also.
            tc.t.tokenType = 0;
            tc.t.size = tok.size();
            vc.push_back(tc.c);
            const char* pos = tok.begin();
            for (; pos<tok.end(); ++pos)
            {
                vc.push_back(tolower(*pos));
            }
        }
        tok.next();
    }
    m_tokens = new char[vc.size()];
    memcpy(m_tokens,vc.data(),vc.size());
    m_numTokens = vc.size();
    m_offsets = new uint32_t[offsets.size()];
    memcpy(m_offsets,offsets.data(),offsets.size()*sizeof(uint32_t));
    
    // TODO: do the following 3 passes in 1 pass.
    // TODO: selectively build inverted indexes
    // TODO: heuristically size inverted indexes.
    m_tokenBoundaries.resize(m_numTokens+1);
    {
        TokenStream::iterator it = begin();
        for (;!it.end();it.next())
        {
            if (!it.isSpace())
            {
                m_tokenBoundaries.set(m_numTokens-it.getPos());
            }
        }
    }
    {
        PrefixIndex::Builder b(*this);
        m_prefixIndex = b.complete();
    }
    {
        WordIndex::Builder wb(*this);
        m_wordIndex = wb.complete();
    }
}


string TokenStream::getSnippet(int startToken, int endToken)
{
    string s;
    iterator it(*this, startToken);
    iterator it_end(*this, endToken);
    const char* textStart = m_origonalText;
    const char* textEnd = m_origonalTextEnd;
    if (it.next())
    {
        textStart = it.pos();
    }
    if (it_end.next())
    {
        textEnd = it_end.pos();
    }
    s.append(textStart, textEnd);
    return s;
}

PrefixIndex* TokenStream::getPrefixIndex()
{
    return m_prefixIndex;
}

WordIndex* TokenStream::getWordIndex()
{
    return m_wordIndex;
}



