#ifndef LIB_STRFUNCS_H
#define LIB_STRFUNCS_H


sprintf(char *dst, const char *format, ...);

size_t strlen(const char *str);
size_t strcpy(char *dst, const char *src);
size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);



#endif
