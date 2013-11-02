#include "kernel.h"
#include "slab.h"
#include "virtual_memory.h"
#include "address_space.h"

static struct kmem_cache *as_cache;
static struct kmem_cache *mr_cache;

address_space_err_t 
common_memory_region_set_location(struct memory_region *mr, virtaddr_t start, size_t len) {

  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_set_flags(struct memory_region *mr, int writable, int executable) {

  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_clone(struct memory_region *dst, struct memory_region *src, int cow) {

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
common_memory_region_alloc(struct address_space *as, struct memory_region **mr) {
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

  kmem_cache_init(kernel()->bsp->allocator, as_cache, kernel()->bsp, "address_space", 
      sizeof(struct address_space), NULL, NULL);
  kmem_cache_init(kernel()->bsp->allocator, mr_cache, kernel()->bsp, "memory_region", 
      sizeof(struct memory_region), NULL, NULL);

}
