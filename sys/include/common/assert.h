#ifndef COMMON_ASSERT_H
#define COMMON_ASSERT_H

#include <config.h>

#ifdef HOSTED
#define ASSERT(x) if ( !(x) ) { printf("Assertion failed!, Line %d, File %s\n", \
    __LINE__, __FILE__); while (1); }
#else
#define ASSERT(x)
#endif

#endif
