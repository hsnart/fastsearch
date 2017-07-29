/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__Document__
#define __textsearchtest__Document__

#include <iostream>
#include <vector>
#include "rapidxml.hpp"
class DocumentStream;
class TokenStream;

/*
properties
participant
alias
participant list
*/

class Document
{
private:
    std::string m_name;
    std::vector<Document*> m_subDocs;
    std::vector<DocumentStream*> m_streams;
public:
    // e.g.:
    // recipients
    // filenames + aliases
    // file types
    // sizes
    // mapi properties
 //   unsigned numProperties();
 //   Property* getProperty(unsigned i);
    Document(char* xml);// initialise from XML
    Document(rapidxml::xml_node<>* node);
    
    const std::string& getName();
    
    // e.g:
    // attachments
    // sub documents
    unsigned numSubDocs();
    Document* getSubDoc(unsigned i);
    Document* getSubDoc(const char* name);
    Document* getSubDocPath(const std::string& path);
    
    // e.g.
    // subject
    // body
    // header
    // footer
    unsigned numStreams();
    DocumentStream* getStream(unsigned i);
    DocumentStream* getStream(const char* name);
private:
    void init(rapidxml::xml_node<>* node);
};

class DocumentStream
{
private:
    std::string m_name;
    std::string m_text;
    TokenStream* m_tokenStream;
public:
    DocumentStream(const char* start, const char* end);
    TokenStream* getTokenStream();
    const char* getText(){return m_text.c_str();}
};

#endif /* defined(__textsearchtest__Document__) */
