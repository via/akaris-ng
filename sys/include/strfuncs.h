#ifndef LIB_STRFUNCS_H
#define LIB_STRFUNCS_H

void k_snprintf(char *, int, const char *, ...);
#if defined(va_arg)
void k_snprintf_vaarg(char *, int, const char *, va_list);
#endif

void * memset(void *dst, int c, size_t len);
void * memcpy(void *dst, const void *src, size_t siz);
size_t strlen(const char *str);
size_t strcpy(char *dst, const char *src);
size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);

void itoa(char *buf, int, int);


#endif
