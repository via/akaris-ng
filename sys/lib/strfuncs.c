
#include "types.h"
#include "strfuncs.h"

size_t strlen(const char *str) {

  const char *tmp = str;
  while (*tmp++);

  return (tmp - str - 1);

}
