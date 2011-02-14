#ifndef LIB_STRFUNCS_H
#define LIB_STRFUNCS_H


int snprintf(char *dst, int length, const char *format, ...);

void * memset(void *dst, int c, size_t len);
void * memcpy(void *dst, const void *src, size_t siz);
size_t strlen(const char *str);
size_t strcpy(char *dst, const char *src);
size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);

void itoa(char *buf, int, int);


#endif
