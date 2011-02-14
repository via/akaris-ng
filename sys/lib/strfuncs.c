
#include "types.h"
#include "strfuncs.h"

size_t strlen(const char *str) {

  const char *tmp = str;
  while (*tmp++);

  return (tmp - str - 1);

}

void *memset(void *dst, int c, size_t len) {

  char *d = dst;

  while (len--) {
    *d++ = (char)c;
  }

  return dst;
}

void *memcpy(void *dst, const void *src, size_t siz) {
  
  char *d = (char *)dst;
  const char *s = (const char *)src;

  while (siz--) {
    *d++ = *s++;
  }

  return dst;
}


void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  if (base == 'd' && d < 0)
  {
    *p++ = '-';
    buf++;
    ud = -d;
  }
  else if (base == 'x')
    divisor = 16;
  do
  {
    int remainder = ud % divisor;

    *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
  }
  while (ud /= divisor);
  *p = 0;
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
  {
    char tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
    p1++;
    p2--;
  }
}

