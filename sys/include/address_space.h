#ifndef COMMON_ADDRESS_SPACE_H
#define COMMON_ADDRESS_SPACE_H

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
  address_space_err_t (*clone) (struct memory_region *dst, 
      struct memory_region *src, int cow);
  address_space_err_t (*map)(struct memory_region *);
  void (*memory_region_fault)(struct memory_region *, virtaddr_t location);

};

struct memory_region {
  virtaddr_t start;
  size_t length;

  struct memory_region_vfuncs v;

  /* flags */
  int writable : 1;
  int executable : 1;
  int copy_on_write : 1;

  /* entry for list of regions in address space*/
  LIST_ENTRY(memory_region) regions;
  /* entry for list of regions currently COW with this one */
  LIST_ENTRY(memory_region) cow_regions;

};

struct address_space_vfuncs {
  address_space_err_t (*address_space_destroy)(struct address_space *);
  address_space_err_t (*address_space_get_memory_region)
    (struct address_space, struct memory_region **, virtaddr_t loc);
};


struct address_space {
  struct address_space_vfuncs v;

  LIST_HEAD(, memory_region) regions;

};

/* arch-independent region functions */
address_space_err_t common_memory_region_set_location(
    struct memory_region *, virtaddr_t start, size_t len);
address_space_err_t common_memory_region_set_flags(
    struct memory_region *, int writable, int executable);
address_space_err_t common_memory_region_clone(
    struct memory_region *dst, struct memory_region *src, int cow);
address_space_err_t common_memory_region_map(struct memory_region *);
void common_memory_region_cow_fault(struct memory_region *, 
    int location);
void common_memory_region_fault(struct memory_region *, int location);

/* arch independent space functions */
address_space_err_t common_address_space_destroy(struct address_space *);
address_space_err_t common_address_space_get_memory_region(
    struct address_space *, struct memory_region **, virtaddr_t loc);

/* Global init/allocation functions */
address_space_err_t memory_region_free(struct memory_region *);
address_space_err_t memory_region_alloc(struct memory_region **);
address_space_err_t address_space_free(struct address_space *);
address_space_err_t address_space_alloc(struct address_space **);
void address_space_init(kmem_cache_ctor as_ctor, kmem_cache_ctor mr_ctor);

#ifdef HOSTED
/* Embedded test cases for static functions */
#include <check.h>
void check_address_space_add_static_tests(TCase *);
#endif


#endif

