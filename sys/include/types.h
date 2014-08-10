#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

typedef unsigned long long uint64;
typedef signed long long int64;
typedef unsigned int    uint32;
typedef signed int       int32;
typedef unsigned short  uint16;
typedef signed short     int16;
typedef unsigned char    uint8;
typedef signed char       int8;
 
#ifndef HOSTED
typedef signed long offset_t;
typedef unsigned long size_t;
#else
#include <stddef.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#define VAR_UNUSED __attribute__((unused))
 

#endif
