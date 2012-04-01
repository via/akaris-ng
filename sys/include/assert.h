#ifndef COMMON_ASSERT_H
#define COMMON_ASSERT_H

#include "sysconf.h"

#ifdef HOSTED
#define assert(x) if ( !(x) ) { printf("Assertion failed!, Line %d, File %s\n", \
    __LINE__, __FILE__); while (1); }
#else
#define assert(x) do { if( !(x) ) while(1); } while(0);
#endif

#endif
