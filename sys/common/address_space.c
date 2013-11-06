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
memory_region_compare_to_location(struct memory_region *mr, virtaddr addr) {
  if (addr < mr->start)
    return -1;
  if (addr > mr->start + mr->len)
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
  if (other->start + other->len < mr->start)
    return -1;
  if (other->start > mr->start + mr->len)
    return 1;
  return 0;
}

static int
memory_region_available_in_address_space(struct address_space *as,
    struct memory_region *mr) {

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
  start = start - (start % pgsize);
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
common_memory_region_alloc(struct address_space *as, 
    struct memory_region **mr) {
  *mr = (struct memory_region *)kmem_cache_alloc(mr_cache);
  if (!mr)
    return AS_OOM;
  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_free(struct address_space *as, struct memory_region *mr) {

  return AS_SUCCESS;
}

address_space_err_t 
common_address_space_free(struct address_space *as) {

  return AS_SUCCESS;
}

address_space_err_t 
common_address_space_alloc(struct address_space **as) {
  *as = kmem_cache_alloc(as_cache);
  if (!as)
    return AS_OOM;
  return AS_SUCCESS;
}

void
common_address_space_init() {

  as_cache = kmem_alloc(kernel()->bsp->allocator);
  mr_cache = kmem_alloc(kernel()->bsp->allocator);

  kmem_cache_init(kernel()->bsp->allocator, as_cache, kernel()->bsp, 
      "address_space", sizeof(struct address_space), NULL, NULL);
  kmem_cache_init(kernel()->bsp->allocator, mr_cache, kernel()->bsp, 
      "memory_region", sizeof(struct memory_region), NULL, NULL);

}
