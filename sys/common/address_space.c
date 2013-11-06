#include "kernel.h"
#include "slab.h"
#include "virtual_memory.h"
#include "address_space.h"

static struct kmem_cache *as_cache;
static struct kmem_cache *mr_cache;


/** Compared an address to a memory_regions bounds.
 * 0  - Address located in memory region.
 * 1  - Address above region.
 * -1 - Address below region.
 */
static int
memory_region_compare_to_location(struct memory_region *mr, virtaddr_t addr) {
  if (addr < mr->start)
    return -1;
  if (addr >= mr->start + mr->length)
    return 1;
  return 0;
}
  
/** Compared a memory region to a memory_regions bounds.
 * 0  - other either overlaps or matches mr
 * 1  - other is entirely above mr
 * -1 - other is entirely below mr
 */

static int
memory_region_compare_to_region(struct memory_region *mr, 
    struct memory_region *other) {
  if (other->start + other->length < mr->start)
    return -1;
  if (other->start >= mr->start + mr->length)
    return 1;
  return 0;
}

static int
memory_region_available_in_address_space(struct address_space *as,
    struct memory_region *mr) {
  struct memory_region *m;
  LIST_FOREACH(m, &as->regions, regions) {
    if (memory_region_compare_to_region(mr, m) != 0)
      return 0;
  }
  return 1;
}

/* Add a memory region to an address space with given start address and lenth.
 * Attempting to create a region that uses already-occupied space will result in
 * AS_USED being returned.  Start address may be rounded down, and should be
 * taken as a hint.
 */
address_space_err_t
address_space_init_region(struct address_space *as,
    struct memory_region *mr, virtaddr_t start, size_t len) {
  if (start == 0) {
    /* generate random address */  
  }
  long pgsize = physmem_page_size(cpu()->localmem);
  start = start - ((long)start % pgsize);
  len = (len + pgsize - 1) / pgsize * pgsize;

  if (!memory_region_available_in_address_space(as, mr)) 
    return AS_USED;

  LIST_INSERT_HEAD(&as->regions, mr, regions);
  return AS_SUCCESS;

}



address_space_err_t 
common_memory_region_set_location(struct memory_region *mr, 
    virtaddr_t start, size_t len) {
  if (!mr)
    return AS_INVALID;

  mr->start = start;
  mr->length = len;
  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_set_flags(struct memory_region *mr, int writable, 
    int executable) {
  if (executable & writable)
    return AS_INVALID;
  mr->writable = writable;
  mr->executable = executable;
  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_clone(struct memory_region *dst, 
    struct memory_region *src, int cow) {

  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_map(struct memory_region *mr) {

  return AS_SUCCESS;
}

void 
common_memory_region_cow_fault(struct memory_region *mr, int location) {

}

void 
common_memory_region_fault(struct memory_region *mr, int location) {

}

address_space_err_t 
memory_region_alloc(struct memory_region **mr) {
  *mr = (struct memory_region *)kmem_cache_alloc(mr_cache);
  if (!mr)
    return AS_OOM;
  return AS_SUCCESS;
}

address_space_err_t 
memory_region_free(struct memory_region *mr) {

  return AS_SUCCESS;
}

address_space_err_t 
address_space_free(struct address_space *as) {

  return AS_SUCCESS;
}

address_space_err_t 
address_space_alloc(struct address_space **as) {
  *as = kmem_cache_alloc(as_cache);
  if (!as)
    return AS_OOM;
  return AS_SUCCESS;
}

/* Initialize address space system
 *
 * @param as_ctor Function to be called to set up arch-dependent as funcs
 * @param mr_dtor Function to be called to set up arch-dependent mr funcs
 *
 */
void
address_space_init(kmem_cache_ctor as_ctor, kmem_cache_ctor mr_ctor) {

  as_cache = kmem_alloc(kernel()->bsp->allocator);
  mr_cache = kmem_alloc(kernel()->bsp->allocator);

  kmem_cache_init(kernel()->bsp->allocator, as_cache, kernel()->bsp, 
      "address_space", sizeof(struct address_space), as_ctor, NULL);
  kmem_cache_init(kernel()->bsp->allocator, mr_cache, kernel()->bsp, 
      "memory_region", sizeof(struct memory_region), mr_ctor, NULL);

}


#ifdef HOSTED
#include <check.h>

START_TEST (check_memory_region_compare_to_location) {
  struct memory_region mr;
  virtaddr_t loc;

  mr.start = (virtaddr_t)0x100000;
  mr.length = 8192;

  loc = (virtaddr_t)0x80000;
  fail_unless(memory_region_compare_to_location(&mr, loc) < 0);

  loc = (virtaddr_t)0x100100;
  fail_unless(memory_region_compare_to_location(&mr, loc) == 0);

  loc = (virtaddr_t)0x102000;
  fail_unless(memory_region_compare_to_location(&mr, loc) > 0);
} END_TEST

void check_address_space_add_static_tests(TCase *t) {
   tcase_add_test(t, check_memory_region_compare_to_location);
}

#endif

