#ifndef _MATH_H
#define _MATH_H


inline static long
maxl(long a, long b) {
  return (a > b) ? a : b;
}

inline static long
minl(long a, long b) {
  return (a < b) ? a : b;
}

inline static int
bittestl(unsigned long field, unsigned long mask) {
  return (field & mask) == mask;
}

inline static int
bitsetl(unsigned long field, unsigned long mask) {
  return field | mask;
}

inline static int
bitclearl(unsigned long field, unsigned long mask) {
  return field & ~mask;
}

#endif

