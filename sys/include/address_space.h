#ifndef COMMON_ADDRESS_SPACE_H
#define COMMON_ADDRESS_SPACE_H

#include "queue.h"
#include "slab.h"
#include "kernel.h"
#include "virtual_memory.h"
#include "physical_memory.h"

struct memory_region;
struct address_space;

typedef enum {
  AS_SUCCESS,
  AS_NOTFOUND, /* Memory region does not exist */
  AS_INVALID, /* Invalid assignment of parameters */
  AS_OOM, /* Unable to allocate memory for data structures */
  AS_USED, /* Desired location conflicts with existing region */
} address_space_err_t;

struct memory_region_vfuncs {
  address_space_err_t (*set_location) (struct memory_region *, 
      virtaddr_t start, size_t len);
  address_space_err_t (*set_flags) (struct memory_region *, int writable, 
      int executable);
  void (*fault)(struct memory_region *, virtaddr_t location);

};

struct memory_region {
  virtaddr_t start;
  size_t length;

  struct memory_region_vfuncs v;

  /* flags */
  unsigned int writable : 1;
  unsigned int executable : 1;
  unsigned int copy_on_write : 1;

  /* entry for list of regions in address space*/
  LIST_ENTRY(memory_region) regions;
  /* entry for list of regions currently COW with this one */
  LIST_ENTRY(memory_region) cow_regions;

};

struct address_space_vfuncs {
  address_space_err_t (*destroy)(struct address_space *);
  address_space_err_t (*get_region)
    (struct address_space *, struct memory_region **, virtaddr_t loc);
  address_space_err_t (*init_region)(struct address_space *,
      struct memory_region *, virtaddr_t start, size_t len);
  address_space_err_t (*clone_region) (struct address_space *,
      struct memory_region *dst, struct memory_region *src, int cow);
  address_space_err_t (*map_region)(struct address_space *, 
      struct memory_region *, physaddr_t p);
};


struct address_space {
  struct address_space_vfuncs v;
  virtmem_md_context_t pd;

  LIST_HEAD(, memory_region) regions;

};

/* arch-independent region functions */
address_space_err_t common_memory_region_set_location(
    struct memory_region *, virtaddr_t start, size_t len);
address_space_err_t common_memory_region_set_flags(
    struct memory_region *, int writable, int executable);
void common_memory_region_cow_fault(struct memory_region *, 
    int location);
void common_memory_region_fault(struct memory_region *, int location);

/* arch independent space functions */
address_space_err_t common_address_space_destroy(struct address_space *);
address_space_err_t common_address_space_get_region(
    struct address_space *, struct memory_region **, virtaddr_t loc);
address_space_err_t common_address_space_init_region(struct address_space *,
    struct memory_region *, virtaddr_t start, size_t len);
address_space_err_t common_memory_region_clone(struct address_space *,
    struct memory_region *dst, struct memory_region *src, int cow);
address_space_err_t common_memory_region_map(struct address_space *,
    struct memory_region *, physaddr_t p);

/* Global init/allocation functions */
address_space_err_t memory_region_free(struct memory_region *);
address_space_err_t memory_region_alloc(struct memory_region **);
address_space_err_t address_space_free(struct address_space *);
address_space_err_t address_space_alloc(struct address_space **);
void address_space_init(kmem_cache_ctor as_ctor, kmem_cache_ctor mr_ctor,
                        kmem_cache_dtor as_dtor, kmem_cache_dtor mr_dtor);

/* Pretty interface */
static inline address_space_err_t memory_region_set_location(
    struct memory_region *mr, virtaddr_t start, size_t len) {
  return mr->v.set_location(mr, start, len);
}

static inline address_space_err_t memory_region_set_flags(
    struct memory_region *mr, int w, int e) {
  return mr->v.set_flags(mr, w, e);
}

#ifdef UNITTEST
/* non-static wrappers for static functions needing testing */
int unittest_memory_region_compare_to_location(struct memory_region *mr,
    virtaddr_t addr);
int unittest_memory_region_compare_to_region(struct memory_region *mr,
    struct memory_region *other);
int unittest_memory_region_available_in_address_space(struct address_space *as,
    struct memory_region *);
address_space_err_t unittest_memory_region_map_exact(virtmem_md_context_t pd
    , virtaddr_t vstart, physaddr_t pstart, size_t n_pages, int flags);
address_space_err_t unittest_memory_region_map_allocate(
    virtmem_md_context_t pd, virtaddr_t vstart, size_t n_pages, int flags);
#endif


#endif

