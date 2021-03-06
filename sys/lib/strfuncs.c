
#include "types.h"
#include "strfuncs.h"

#include <stdarg.h>

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

size_t strcpy(char *dst, const char *src) {
  const char *s = src;

  while (*s != '\0')
    *dst++ = *s++;

  *dst = '\0';
  return (s - src);
}

int 
strncmp(const char *s1, const char *s2, size_t n) {
  while (n-- > 0) {
    if (*s1 == '\0') {
      if (*s2 == '\0')
        return 0; /*Both finished, equal */
      return -1; /*s1 finished first, less */
    }
    if (*s2 == '\0')
      return 1; /*S2 finished first */

    if (*s1 > *s2)
      return 1;
    if (*s1 < *s2)
      return -1;
    s1++;
    s2++;
  }
  return 0; /* N ran out */
}

const char *
strstr(const char *haystack, const char *needle) {
  char c;
  while ( (c = *haystack) ) {
    if (c == needle[0]) {
      if (strncmp(haystack, needle, strlen(needle)) == 0)
        return haystack;
      /* compare */
    }
    haystack++;
  }
  return NULL;
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


void k_snprintf_vaarg(char *output, int maxlen, const char *format,
    va_list ap) {

  char buf[20];
  char *out = output;

  const char *s_arg;
  int d_arg;
  unsigned long ul_arg;
  
  while (*format) {
    if (*format == '%') {
      switch(*++format) {
        case 'd':
          d_arg = va_arg(ap, int);
          itoa(buf, 'd', d_arg);
          out += strlcpy(out, buf, maxlen - (out - output));
          break;
        case 'u':
          ul_arg = va_arg(ap, unsigned long);
          itoa(buf, 'd', ul_arg);
          out += strlcpy(out, buf, maxlen - (out - output));
          break;
        case 'x':
          ul_arg = va_arg(ap, unsigned long);
          itoa(buf, 'x', ul_arg);
          out += strlcpy(out, buf, maxlen - (out - output));
          break;
        case 's':
          s_arg = va_arg(ap, char *);
          out += strlcpy(out, s_arg, maxlen - (out - output));
          break;
      }
      ++format;
    } else {
      *out++ = *format++;
    }

    if (out >= output + maxlen) 
      break;

  }
  if (out < output + maxlen) {
    *out = '\0';
  }
  *(output - 1) = '\0';
}


void
k_snprintf(char *output, int maxlen, const char *format, ...) {

  va_list ap;
  va_start(ap, format);

  k_snprintf_vaarg(output, maxlen, format, ap);
  va_end(ap);
}
