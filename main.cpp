//
//  main.cpp
//  textsearchtest
//
//  Created by howard on 22/12/2013.
//  Copyright (c) 2013 howard. All rights reserved.
//

#include <iostream>
#include "FS_Search.h"
#include <fstream>
#include "unicode/utypes.h"
#include "unicode/stringpiece.h"
#include "unicode/utf8.h"
#include "unicode/uchar.h"
#include "Timer.h"
#include "boost/regex.hpp"
#include "rapidxml.hpp"
#include "Document.h"
#include "Profile.h"
#include "FS_GraphViz.h"

using namespace rapidxml;

int32_t countWhiteSpace(const StringPiece &sp) {
    const char *s=sp.data();
    int32_t length=sp.length();
    int32_t count;
    for(int32_t i=0; i<length;) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        if(u_isUWhiteSpace(c)) {
            ++count;
        }
    }
    return count;
}
using namespace std;
std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

class cout_callback : public FS_Expression::MatchCallback
{
public:
    virtual bool match(const char* matchStart, const char* matchEnd)
    {
        cout<<"match: "<<string(matchStart,matchEnd)<<endl;
        return true;
    }
};

class string_callback : public FS_Expression::MatchCallback
{
public:
    string res;
    virtual bool match(const char* matchStart, const char* matchEnd)
    {
        if (res.size()>0) res.append(1,'|');
        res.append(matchStart,matchEnd);
        return true;
    }
};

int succeeded = 0;
int failed = 0;

bool test(string text, string expr, string expected)
{
    TokenStream tok(text);
    FS_Expression e(expr);
    string_callback cb;
    e.count(tok,&cb);
    if (cb.res == expected)
    {
        cout<<"passed"<<endl;
        succeeded++;
        return true;
    } else {
        cout<<"failed: expected:"<<expected<<" found:"<<cb.res<<endl;
        failed++;
        return false;
    }
}

void graph(string expr)
{
    FS_Expression* e = new FS_Expression(expr);
    FS_GraphViz g;
    e->accept(g);
    string graph;
    g.getGraph(graph);
    {
        ofstream gout("graph.dot");
        gout<<graph;
    }
}


int main(int argc, const char * argv[])
{
    graph("%#he llo%");
    
    cout<<"char matching"<<endl;
    {
        string s = "123abc123 http://www.google.com hsnart@innovode.com";
        test(s,"%#\\d\\d\\d% abc","123abc");
        test(s,"%#\\d\\d\\d%","123|123");
        test(s,"%#http*.com%","http://www.google.com");
        test(s,"%#@?[+g]%","@innovode.com");
        test(s,"%#:?[+g]%","://www.google.com");
        test(s,"%#:*%com","://www.google.com");
        test(s,"%#:*com%","://www.google.com");
    }
    
    cout<<"precededby"<<endl;
    {
        string s = "123abc123 http://www.google.com hsnart@innovode.com";
        test(s,"%#:*com%{%precededby%http} hsnart","http://www.google.com hsnart");
        test(s,"%#:*google%{%precededby%http}.com","http://www.google.com");
        test(s,"%#:*com%%precededby%http","http://www.google.com");
        test(s,"%#@?[+g]%%precededby%%any%","hsnart@innovode.com");
        test(s,"@ %precededby[5]%www","www.google.com hsnart@");
        test(s,"@ %precededby[4]%www","");
    }
    cout<<"basic tests"<<endl;
    {
        string s = "The quick brown Fox jumped over the lazy dog.";
        test(s,s,s);
        test(s,"the","The|the");
    }
    
    cout<<"wildcards"<<endl;
    {
        string s = "The quick brown Fox jumped over the lazy dog.";
        test(s,"t*","The|the");
        test(s,"?he","The|the");
        test(s,"??e","The|the");
        test(s,"???","The|Fox|the|dog");
        test(s,"*ck","quick");
        test(s,"* * fox","quick brown Fox");
    }
    
    cout<<"logic"<<endl;
    {
        string s = "The quick brown Fox jumped over the lazy dog.";
        test(s,"the quick|lazy","The quick|the lazy");
    }
    
    cout<<"repeats"<<endl;
    {
        string s = "The quick brown Fox jumped over the lazy dog.";
        test(s,"the %any%[3] jumped","The quick brown Fox jumped");
        test(s,"the *[*] ???","The quick brown Fox|the lazy dog");
        test(s,"the *[*g] ???","The quick brown Fox jumped over the lazy dog|the lazy dog");
    }
    
    
    
    
    
  /*  string testMail = get_file_contents("testMail.xml");
    
    string profile_xml = get_file_contents("config.xml");
    
    Profile profile;
    profile.init((char*)profile_xml.c_str());
    Document doc((char*)testMail.c_str());
    
    profile.evaluate(&doc);
    
   // FS_Expression* e = new FS_Expression("start (hello %any%[1,4] one)|(%any%[0,2] two) end");
   
    */
 //   FS_Expression* e = new FS_Expression("Purchaser will purchase from the Seller {%any%[,4]  the Seller will sell to the[?] Purchaser} | {%any%[,4] Purchaser the[?] to sell will Seller}");
    
 //   return 0;
  /*  DocumentStream1 text1(t);
    {Timer t("DocumentStream");
        int count = 0;
        for (int i=0; i<100; i++)
        {
            DocumentStream1::iterator it = text1.begin();
            while(it.next())
            {
     //           cout<<string(it.pos(),it.pos()+it.tokenSize());
                if (!it.isSpace()) count++;
            }
        }
        cout<<endl<<"number of non space tokens ="<<count<<endl;
    }*/
//    return 0;
    string t = get_file_contents("test.txt");

    TokenStream text(t);
    
    {
        cout_callback cb;
        FS_Expression* e2 = new FS_Expression("hello %#Wor??%");
        e2->count(text,&cb);
    }
   // return 0;
    {
        int matchCount = 0;
        boost::regex exp("hello (sand)|(hello)|(bye)|(what)|(world) world end", boost::regex::icase);
        //boost::regex exp("hello world world end", boost::regex::icase);
        {
            Timer ti("boost regex");
           for (int k=0; k<1000; k++)
            {
                boost::sregex_token_iterator iter(t.begin(), t.end(), exp, 0);
                boost::sregex_token_iterator end;
                
                for(; iter != end; ++iter ) {
                  //  std::cout<<exp<<'\n';
                    matchCount ++;
                }
                
            }
        }
        cout<<"regex match count ="<<matchCount<<endl;
        
    }

 /*   {
        cout<<" list for key 85"<<endl;
        WordIndex::Builder b(text);
        b.dump(85);
        cout<<endl;
        WordIndex* wi = b.complete();
        wi->dump(85);
    }*/
    
    {
        int matchCount = 0;
        FS_Expression e("hello sand|hello|bye|what|world world end");
        {
            Timer t("1000 searches");
            
            for (int k=0; k<1000; k++)
            {
                matchCount+= e.count(text);
            }
        }
        cout<<"fs_search match count="<<matchCount<<endl;
    }
    
    
    return 0;
}

