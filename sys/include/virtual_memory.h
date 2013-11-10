#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "types.h"
#include "sysconf.h"
#include "physical_memory.h"

typedef void* virtaddr_t;
/* Pointer to machine dependent address space component
 * aka page directory */
typedef void* virtmem_md_context_t;

struct virtmem;
struct virtmem_vfuncs;
struct cpu;
struct address_space;

typedef enum {
  VIRTMEM_SUCCESS,
  VIRTMEM_NOTPRESENT,
  VIRTMEM_OOM,
} virtmem_error_t;

typedef enum {
  VIRTMEM_PAGE_READABLE = 1,
  VIRTMEM_PAGE_WRITABLE = 2,
  VIRTMEM_PAGE_EXECUTABLE = 4,
} virtmem_flag_t;

struct virtmem_vfuncs {
  virtmem_error_t (*kernel_alloc)(struct virtmem *v, virtaddr_t *, 
      unsigned int n_pages);
  virtmem_error_t (*kernel_free)(struct virtmem *v, virtaddr_t);
  virtmem_error_t (*kernel_virt_to_phys)(struct virtmem *v, 
      struct physmem_page **p, virtaddr_t addr);
  virtmem_error_t (*kernel_map_virt_to_phys)(struct virtmem *v, 
      physaddr_t p, virtaddr_t addr);
  virtaddr_t (*kernel_brk)(struct virtmem *v, virtaddr_t amt);

  virtmem_error_t (*user_get_page)(struct virtmem *v,
      virtmem_md_context_t c, physaddr_t *paddr, virtaddr_t vaddr);
  virtmem_error_t (*user_map_page)(struct virtmem *v,
      virtmem_md_context_t c, virtaddr_t vaddr, physaddr_t paddr);
  virtmem_error_t (*user_set_page_flags)(struct virtmem *v,
      virtmem_md_context_t c, virtaddr_t vaddr, int page_flags);
  virtmem_error_t (*user_setup_kernelspace)(virtmem_md_context_t);
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
virtmem_kernel_virt_to_phys(struct virtmem *v, struct physmem_page **p, 
    virtaddr_t addr) {
  return v->v.kernel_virt_to_phys(v, p, addr);
}

inline static virtmem_error_t
virtmem_kernel_map_virt_to_phys(struct virtmem *v, physaddr_t p,
    virtaddr_t addr) {
  return v->v.kernel_map_virt_to_phys(v, p, addr);
}

inline static virtaddr_t
virtmem_brk(struct virtmem *v, virtaddr_t amt) {
  return v->v.kernel_brk(v, amt);
}

inline static virtmem_error_t
virtmem_user_get_page(struct virtmem *v, virtmem_md_context_t c,
    physaddr_t *p, virtaddr_t vaddr) {
  return v->v.user_get_page(v, c, p, vaddr);
}

inline static virtmem_error_t
virtmem_user_map_page(struct virtmem *v, virtmem_md_context_t c,
    virtaddr_t vaddr, physaddr_t p) {
  return v->v.user_map_page(v, c, vaddr, p);
}

inline static virtmem_error_t
virtmem_user_set_page_flags(struct virtmem *v, virtmem_md_context_t c,
    virtaddr_t vaddr, int flags) {
  return v->v.user_set_page_flags(v, c, vaddr, flags);
}

inline static virtmem_error_t 
virtmem_user_setup_kernelspace(struct virtmem *v, virtmem_md_context_t ctx) {
  return v->v.user_setup_kernelspace(ctx);
}

#endif

