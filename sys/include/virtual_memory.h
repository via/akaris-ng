#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "types.h"
#include "sysconf.h"
#include "physical_memory.h"

typedef void* virtaddr_t;

struct virtmem;
struct virtmem_vfuncs;
struct cpu;

typedef enum {
  VIRTMEM_SUCCESS,
  VIRTMEM_NOTPRESENT,
  VIRTMEM_OOM,
} virtmem_error_t;

struct virtmem_vfuncs {
  virtmem_error_t (*kernel_alloc)(struct virtmem *v, virtaddr_t *, 
      unsigned int n_pages);
  virtmem_error_t (*kernel_free)(struct virtmem *v, virtaddr_t);
  virtmem_error_t (*kernel_virt_to_phys)(const struct virtmem *v, 
      struct physmem_page **p, virtaddr_t addr);
  virtmem_error_t (*kernel_map_virt_to_phys)(struct virtmem *v, 
      struct physmem_page **p, virtaddr_t addr);
  virtaddr_t (*kernel_brk)(struct virtmem *v, virtaddr_t amt);
};

struct virtmem {
  struct virtmem_vfuncs v;
  struct cpu *cpu;

};

inline static virtmem_error_t
virtmem_kernel_alloc(struct virtmem *v, virtaddr_t *a, unsigned int n_pages) {
  return v->v.kernel_alloc(v, a, n_pages);
}

inline static virtmem_error_t
virtmem_kernel_free(struct virtmem *v, virtaddr_t addr) {
  return v->v.kernel_free(v, addr);
}

inline static virtmem_error_t
virtmem_kernel_virt_to_phys(const struct virtmem *v, struct physmem_page **p, 
    virtaddr_t addr) {
  return v->v.kernel_virt_to_phys(v, p, addr);
}

inline static virtmem_error_t
virtmem_kernel_map_virt_to_phys(struct virtmem *v, struct physmem_page **p,
    virtaddr_t addr) {
  return v->v.kernel_map_virt_to_phys(v, p, addr);
}

inline static virtaddr_t
virtmem_brk(struct virtmem *v, virtaddr_t amt) {
  return v->v.kernel_brk(v, amt);
}


#endif

