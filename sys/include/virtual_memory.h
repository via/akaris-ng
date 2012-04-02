#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "types.h"
#include "sysconf.h"
#include "physical_memory.h"

typedef void* virtaddr_t;

struct virtmem;
struct virtmem_vfuncs;

typedef enum {
  VIRTMEM_SUCCESS,
} virtmem_error_t;

struct virtmem_vfuncs {
  virtaddr_t (*kernel_alloc)(struct virtmem *v, unsigned int n_pages);
  void (*kernel_free)(struct virtmem *v, virtaddr_t);
  physaddr_t (*kernel_virt_to_phys)(struct virtmem *v, virtaddr_t addr);
  void (*kernel_map_virt_to_phys)(struct virtmem *v, virtaddr_t addr,
        struct physmem_page *p);
};

struct virtmem {
  struct virtmem_vfuncs v;

};

inline static virtaddr_t
virtmem_kernel_alloc(struct virtmem *v, unsigned int n_pages) {
  return v->v.kernel_alloc(v, n_pages);
}

inline static void
virtmem_kernel_free(struct virtmem *v, virtaddr_t addr) {
  v->v.kernel_free(v, addr);
}

inline static physaddr_t
virtmem_kernel_virt_to_phys(struct virtmem *v, virtaddr_t addr) {
  return v->v.kernel_virt_to_phys(v, addr);
}

inline static void
virtmem_kernel_map_virt_to_phys(struct virtmem *v, virtaddr_t addr,
    struct physmem_page *p) {
  v->v.kernel_map_virt_to_phys(v, addr, p);
}


#endif

