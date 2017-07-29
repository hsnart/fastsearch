/*****************************************************************
 *
 * INNOVODE INTELLIGENCE
 * Copyright (c) 2014 All rights reserved, Innovode Ltd.
 * Description: Core
 *
 * Author: Howard Snart
 *
 ******************************************************************/

#ifndef __textsearchtest__FS_Utils__
#define __textsearchtest__FS_Utils__

#include <iostream>
template <class Vector> void clearPtrVector(Vector &a)
{
    for(size_t i = 0; i < a.size(); i++) delete a[i];
    a.clear();
}

#endif /* defined(__textsearchtest__FS_Utils__) */
