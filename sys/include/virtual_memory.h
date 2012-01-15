#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "types.h"
#include "sysconf.h"

#if PHYS_BITS == 32
typedef uint32 virtaddr_t;
#elif PHYS_BITS == 64
typedef uint64 virtaddr_t;
#endif

struct virtmem;
struct virtmem_vfuncs;

typedef enum {
  VIRTMEM_SUCCESS,
} virtmem_error_t;

struct virtmem_vfuncs {
  virtaddr_t (*kernel_alloc)(struct virtmem *v, unsigned int n_pages);
  void (*kernel_free)(struct virtmem *v, virtaddr_t);
};

struct virtmem {
  struct virtmem_vfuncs v;

};

#endif

