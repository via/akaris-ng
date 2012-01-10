#ifndef COMMON_ASSERT_H
#define COMMON_ASSERT_H

#include "sysconf.h"
#include "stdlib.h"

#ifdef HOSTED
#define ASSERT(x) if ( !(x) ) { printf("Assertion failed!, Line %d, File %s\n", \
    __LINE__, __FILE__); exit (1); }
#else
#define ASSERT(x) do { if( !(x) ) while(1); } while(0);
#endif

#endif
