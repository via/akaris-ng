#include "kernel.h"
#include "slab.h"
#include "virtual_memory.h"
#include "address_space.h"

static struct kmem_cache as_cache;
static struct kmem_cache mr_cache;

void 
common_memory_region_set_location(struct memory_region *mr, virtaddr_t start, size_t len) {

}

void 
common_memory_region_set_flags(struct memory_region *mr, int writable, int executable) {

}

void 
common_memory_region_clone(struct memory_region *dst, struct memory_region *src, int cow) {

}

void 
common_memory_region_map(struct memory_region *mr) {

}

void 
common_memory_region_cow_fault(struct memory_region *mr, int location) {

}

void 
common_memory_region_fault(struct memory_region *mr, int location) {

}

void 
common_memory_region_alloc(struct address_space *as, struct memory_region *mr) {
  *mr = kmem_cache_alloc(mr_cache);
}

void 
common_memory_region_free(struct address_space *as, struct memory_region *mr) {

}

void 
common_address_space_free(struct address_space *as) {

}

void 
common_address_space_alloc(struct address_space **as) {
  *as = kmem_cache_alloc(as_cache);
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
