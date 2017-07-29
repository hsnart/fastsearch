/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "InvertedIndex.h"
#include "TokenStream.h"
//class InvertedIndex
void InvertedIndex::init(int tableSize)
{
    m_tableSize = tableSize;
    m_index.resize(tableSize);
}
//    class Builder
InvertedIndex::Builder::Builder(int tableSize):m_tableSize(tableSize)
{
    m_tmpIndex.resize(m_tableSize);
}
void InvertedIndex::Builder::add(unsigned key, unsigned value){m_tmpIndex[key].push_back(value);}


void InvertedIndex::Builder::dump(char key)
{
    cout<<"tmp table size="<<m_tmpIndex[key%m_tableSize].size()<<endl;
    vector<uint32_t>::iterator it = m_tmpIndex[key%m_tableSize].begin();
    vector<uint32_t>::iterator it_end = m_tmpIndex[key%m_tableSize].end();
    for(; it != it_end; ++it)
    {
        cout<<*it<<",";
    }
    cout<<endl;
}

//   class iterator
void InvertedIndex::iterator::init(uint32_t* it, uint32_t* it_end)
{
    m_it = it;
    m_it_end = it_end;
    m_numInts = 0;
    m_pos = -1;
    m_val = 0;
}

InvertedIndex::InvertedIndex(){}

bool InvertedIndex::get(unsigned key, iterator& it)
{
    unsigned size =m_index[key].second;
    if (size ==0) return false;
    uint32_t* start = m_data.data() + m_index[key].first;
    uint32_t* end = start+m_index[key].second;
    it.init(start,end);
    return true;
}

void InvertedIndex::dump(char key)
{
    cout<<"compressed table"<<endl;
    iterator it;
    get(key,it);
    while(it.next())
    {
        cout<<it.get()<<",";
        cout.flush();
    }
    cout<<endl;
}


InvertedIndex* InvertedIndex::Builder::complete()
{
    InvertedIndex* index = new InvertedIndex();
    complete(index);
    return index;
}
void InvertedIndex::Builder::complete(InvertedIndex* index)
{
    index->init(m_tableSize);
    integer_encoding::internals::Simple9 comp;
    vector<uint32_t> tmpBuffer;
    tmpBuffer.resize(16000);
    
    int us = 0; // uncompressed table size
    int cs = 0; // compressed size
    vector<uint32_t> dc; // delta compressed integers.
    for (int i=0;i<m_tmpIndex.size(); ++i)
    {
        size_t inSize = m_tmpIndex[i].size();
        us+=inSize;
        if (inSize>0)
        {
            dc.clear();
            dc.reserve(inSize);
            {
                uint32_t last = 0;
                vector<uint32_t>::iterator vit=m_tmpIndex[i].begin();
                vector<uint32_t>::iterator vit_end=m_tmpIndex[i].end();
                for (; vit != vit_end; ++vit)
                {
                    dc.push_back(*vit - last);
                    last = *vit;
                }
            }
            if (dc.size()>tmpBuffer.size())
            {
                tmpBuffer.resize(dc.size());
            }
            uint64_t outSize = tmpBuffer.size();
            comp.encodeArray(dc.data(), dc.size(), (uint32_t*)tmpBuffer.data(), &outSize);
            // store compressed data.
            m_tmpIndex[i].resize(outSize);
            memcpy((uint32_t*)m_tmpIndex[i].data(),tmpBuffer.data(),outSize*sizeof(uint32_t));
     //       cout<<i<<" : in size="<<dc.size()<<" out size="<<outSize<<endl;
            cs+=outSize;
        }
    }
    // put all the arrays into 1 block;
    index->m_data.resize(cs);
    {
        int pos = 0;
        for (int i=0; i<m_tableSize; i++)
        {
            size_t size =m_tmpIndex[i].size();
            index->m_index[i].first = pos;
            index->m_index[i].second = size;
            memcpy(&index->m_data[pos],m_tmpIndex[i].data(),size*sizeof(uint32_t));
            pos += size;
        }
        m_tmpIndex.clear();
    }
 //   cout<<"uncompressed size="<<us<<" compressed size="<<cs<<endl;
}

unsigned int djb2(const char* wordStart, const char* wordEnd)
{
    // djb2 hash. Very good distribution.
    unsigned int hash = 5381;
    //  if ((wordEnd-wordStart)>2) wordEnd = wordStart+2;
    const char* pos = wordStart;
    for (;pos != wordEnd; ++pos)
    {
        hash =((hash << 5) + hash) + tolower(*pos);
    }
    return hash;
}


//class PrefixIndex: public InvertedIndex

//class Builder: public InvertedIndex::Builder
unsigned int PrefixIndex::prefixHash(const char* wordStart, const char* wordEnd)
{
    // djb2 hash. Very good distribution.
    if ((wordEnd-wordStart)>2) wordEnd = wordStart+2;
    return djb2(wordStart, wordEnd);
}
PrefixIndex::Builder::Builder(TokenStream& text)
: InvertedIndex::Builder(51)// replace with heuristic based on stream size.
{
    TokenStream::iterator it = text.begin();
    for (;!it.end();it.next())
    {
        if (!it.isSpace())
        {
            add(PrefixIndex::prefixHash(it.lc_pos(), it.lc_pos()+it.lc_size())%m_tableSize,it.getPos());
        }
    }
}
PrefixIndex* PrefixIndex::Builder::complete()
{
    PrefixIndex* index = new PrefixIndex();
    InvertedIndex::Builder::complete(index);
    return index;
}

bool PrefixIndex::get(const char* startWord, const char* endWord, iterator& it)
{
    return InvertedIndex::get(PrefixIndex::prefixHash(startWord, endWord)%m_tableSize, it);
}

bool PrefixIndex::get(unsigned int prefixHash, iterator& it)
{
    return InvertedIndex::get(prefixHash%m_tableSize, it);
}

//class WordIndex: public InvertedIndex

//class Builder: public InvertedIndex::Builder
unsigned int WordIndex::hash(const char* wordStart, const char* wordEnd)
{
   return djb2(wordStart, wordEnd);
}
WordIndex::Builder::Builder(TokenStream& text)
: InvertedIndex::Builder(89)//151)// replace with heuristic based on stream size.
{
    TokenStream::iterator it = text.begin();
    for (;!it.end();it.next())
    {
        if (!it.isSpace())
        {
            add(WordIndex::hash(it.lc_pos(), it.lc_pos()+it.lc_size())%m_tableSize,it.getPos());
        }
    }
}
WordIndex* WordIndex::Builder::complete()
{
    WordIndex* index = new WordIndex();
    InvertedIndex::Builder::complete(index);
    return index;
}

bool WordIndex::get(const char* startWord, const char* endWord, iterator& it)
{
    return InvertedIndex::get(WordIndex::hash(startWord, endWord)%m_tableSize, it);
}

bool WordIndex::get(unsigned int hash, iterator& it)
{
    return InvertedIndex::get(hash%m_tableSize, it);
}

