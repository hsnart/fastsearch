/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "Document.h"
#include "rapidxml.hpp"
#include "TokenStream.h"

using namespace std;
using namespace rapidxml;

Document::Document(char* xml)// initialise from XML
{
    xml_document<> xmlDoc;
    xmlDoc.parse<0>(xml);
    init(&xmlDoc);
}

Document::Document(xml_node<>* node)
{
    init(node);
}

void Document::init(xml_node<>* node)
{
    m_name = node->name();
    if (node->value_size()>0)
    {
        cout<<"text:"<<node->value()<<endl;
        m_streams.push_back(new DocumentStream(node->value(), node->value()+node->value_size()));
    }
    
    for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
    {
        if (child->type() == node_data)
        {
            if (child->value_size()>0)
            {
                cout<<"more text:"<<child->value()<<endl;
            }
        } else {
            m_subDocs.push_back(new Document(child));
        }
    }
}

const std::string& Document::getName()
{
    return m_name;
}

// e.g:
// attachments
// sub documents
unsigned Document::numSubDocs()
{
    return m_subDocs.size();
}
Document* Document::getSubDoc(unsigned i)
{
    return m_subDocs[i];
}
Document* Document::getSubDoc(const char* name)
{
    for (int i=0; i<m_subDocs.size(); i++)
    {
        if (strcasecmp(m_subDocs[i]->getName().c_str(), name)==0)
        {
            return m_subDocs[i];
        }
    }
    return NULL;
}

Document* Document::getSubDocPath(const std::string& path)
{
    // e.g. mail.subject
    size_t splitPos = path.find('.');
    if (splitPos != string::npos)
    {
        string lhs(path.begin(),path.begin()+splitPos);
        string rhs(path.begin()+splitPos+1,path.end());
        Document* sub=getSubDoc(lhs.c_str());
        if (sub)
        {
            return sub->getSubDocPath(rhs);
        } else {
            return NULL;
        }
    }
    return getSubDoc(path.c_str());
}

// e.g.
// subject
// body
// header
// footer
unsigned Document::numStreams()
{
    return m_streams.size();
}
DocumentStream* Document::getStream(unsigned i)
{
    return m_streams[i];
}
//DocumentStream* Document::getStream(const char* name);


DocumentStream::DocumentStream(const char* start, const char* end)
: m_text(start,end), m_tokenStream(0)
{
}
TokenStream* DocumentStream::getTokenStream()
{
    if (m_tokenStream) return m_tokenStream;
    m_tokenStream = new TokenStream(m_text.c_str(),m_text.c_str()+m_text.size());
    return m_tokenStream;
}
