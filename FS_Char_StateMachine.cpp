/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#include "FS_Char_StateMachine.h"


FS_C_CharSm* FS_C_CharSm::create(char c, bool matchCase)
{
    string s;
    s.append(1,c);
    return create(s,matchCase);
}
FS_C_CharSm* FS_C_CharSm::create(const std::string& c, bool matchCase)
{
    std::string s = c;
    if (!matchCase)
    {
        // convert string to lower
        for (int i=0; i<s.size(); i++)
        {
            s[i] = tolower(s[i]);
        }
    }
    FS_C_CharSm* e = 0;
    if (matchCase)      e = FS_C_CharSmImpl<true>::create(s);
    else                e = FS_C_CharSmImpl<false>::create(s);
    return e;
}

FS_C_DigitSm* FS_C_DigitSm::create(int n)
{
    if (n==1) return new FS_C_DigitSm();
    return new FS_C_DigitSmN(n);
}