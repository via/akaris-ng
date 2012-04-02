#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "types.h"
#include "sysconf.h"

typedef void* virtaddr_t;

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

inline static virtaddr_t
kernel_alloc(struct virtmem *v, unsigned int n_pages) {
  return v->v.kernel_alloc(v, n_pages);
}

inline static void
kernel_free(struct virtmem *v, virtaddr_t addr) {
  v->v.kernel_free(v, addr);
}

#endif

