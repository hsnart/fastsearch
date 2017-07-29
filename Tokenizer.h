/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Tokenizer__
#define __textsearchtest__Tokenizer__

#include <iostream>
#include <stdlib.h>
#include <unicode/brkiter.h>

//
//
// TODO use ICU for tokenization.
// then add my own tokenization on top of that.
class Tokenizer
{
private:
    const char* m_start;
    const char* m_end;
    const char* m_tokStart = NULL;
    const char* m_pos;
    int m_cpos;
    bool m_more= false;
    bool m_upper = false;
    bool m_space = false;
    bool m_wild  = false;
public:
    Tokenizer(const char* start, const char* end)
    {
        m_start = start;
        m_end = end;
        m_pos = m_start;
        m_more = true;
        m_cpos = 0;
        BreakIterator* boundary;
        UnicodeString stringToExamine("Aaa bbb ccc. Ddd eee fff.");

        UErrorCode status = U_ZERO_ERROR;
        boundary = BreakIterator::createWordInstance(
                                                     Locale::getUS(), status);
        boundary->setText(stringToExamine);
    }
    bool isWild(char c)
    {
        return (c=='?' || c=='*');
    }
    bool next()
    {
        m_cpos = 0;
        m_upper = false;
        m_space = false;
        m_wild = false;
        if (m_pos == m_end){
            m_more = false;
            return false;
        }
        m_tokStart = m_pos;
        if (isalpha(*m_pos)||isWild(*m_pos))
        {
            if (isupper(*m_pos)) m_upper = true;
            while (m_pos != m_end && (isalpha(*m_pos) || isWild(*m_pos))){
                if (isupper(*m_pos)) m_upper = true;
                if (isWild(*m_pos)) m_wild = true;
                ++m_pos;
            }
            return true;
        }
        if (isdigit(*m_pos))
        {
            while (m_pos != m_end && isdigit(*m_pos)) m_pos++;
            return true;
        }
        if (isspace(*m_pos))
        {
            m_space = true;
            while (m_pos != m_end && isspace(*m_pos)) m_pos++;
            return true;
        }
        m_pos++;
        return true;
    }
    bool nextChar()
    {
        ++m_cpos;
        if (m_cpos>=(m_pos-m_tokStart)) return next();
        return true;
    }
    size_t size(){return (m_pos-m_tokStart);}
    bool isUpper(){return m_upper;}
    bool isSpace(){return m_space;}
    bool isWild(){return m_wild;}
    bool eat(char c)
    {
        if (*begin() == c)
        {
            nextChar();
            return true;
        }
        return false;
    }
    const char* begin(){ return m_tokStart+m_cpos;}
    const char* end(){ return m_pos;}
    bool is(const char* s)
    {
        int s_size = strlen(s);
        return (s_size == (m_pos-m_tokStart) && (strncasecmp(s,m_tokStart,s_size) ==0));
    }
    bool more(){if (m_tokStart == NULL) next();
        return m_more;}
    char peek(){return *begin();}
    int getInt()
    {
        if (isdigit(*m_tokStart))
        {
            char* end;
            return strtol(begin(),&end,10);
        } else {
            throw std::runtime_error(std::string("expected number, found:'")+std::string(m_tokStart,m_pos)+"'");
        }
        return 0;
    }
    size_t pos(){return (m_tokStart - m_start);}
};


#endif /* defined(__textsearchtest__Tokenizer__) */
