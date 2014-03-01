#include "assert.h"
#include "kernel.h"
#include "slab.h"
#include "virtual_memory.h"
#include "physical_memory.h"
#include "address_space.h"
#include "math.h"

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
  if (other->start + other->length <= mr->start)
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
    if (memory_region_compare_to_region(mr, m) == 0)
      return 0;
  }
  return 1;
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
common_memory_region_set_flags(struct memory_region *mr, 
    address_space_perm_t flags) {
  int executable = bittestl(flags, AS_EXECUTABLE);
  int writable = bittestl(flags, AS_WRITABLE);
  if (!mr)
    return AS_INVALID;
#ifndef NO_W_X
  if (executable & writable)
    return AS_INVALID;
#endif /* TODO: re-enable for W^X */
  mr->writable = writable;
  mr->executable = executable;
  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_clone(struct address_space *as VAR_UNUSED, 
    struct memory_region *dst VAR_UNUSED, 
    struct memory_region *src VAR_UNUSED, int cow VAR_UNUSED) {
  assert(0); /* Not implemented yet */
  return AS_SUCCESS;
}

static address_space_err_t
memory_region_map_exact(virtmem_md_context_t pd, virtaddr_t vstart,
    physaddr_t pstart, size_t n_pages, int flags) {

  unsigned int pg_num = 0;
  unsigned long pgsize = physmem_page_size(cpu()->localmem);
  virtaddr_t cur;
  physaddr_t p = pstart;

  for (cur = vstart; pg_num < n_pages; ++pg_num, 
      cur += pgsize, p += pgsize) {

    virtmem_user_map_page(cpu()->kvirt, pd, cur, p);
    virtmem_user_set_page_flags(cpu()->kvirt, pd, cur, flags);
  }

  return AS_SUCCESS;
}

static address_space_err_t
memory_region_map_allocate(virtmem_md_context_t pd, virtaddr_t vstart,
    size_t n_pages, int flags) {

  unsigned int pg_num = 0;
  unsigned long pgsize = physmem_page_size(cpu()->localmem);
  virtaddr_t cur;
  physaddr_t p;

  for (cur = vstart; pg_num < n_pages; ++pg_num, cur += pgsize) {
    virtmem_user_get_page(cpu()->kvirt, pd, &p, cur);
    if (p == 0) {
      /* Need to fetch a page */
      if (physmem_page_alloc(cpu()->localmem, 0, &p) != PHYSMEM_SUCCESS)
        return AS_OOM;
      virtmem_user_map_page(cpu()->kvirt, pd, cur, p);
    }
    virtmem_user_set_page_flags(cpu()->kvirt, pd, cur, flags);
  }

  return AS_SUCCESS;
}

address_space_err_t 
common_memory_region_map(struct address_space *as, struct memory_region *mr, physaddr_t paddr) {

  unsigned long pgsize = physmem_page_size(cpu()->localmem);
  size_t n_pages;

  assert((unsigned long)mr->start % pgsize == 0);
  assert(mr->length % pgsize == 0);
  assert(paddr == 0 || (paddr % pgsize == 0));

  n_pages = mr->length / pgsize;

  int page_flags = VIRTMEM_PAGE_READABLE | 
    (mr->writable ? VIRTMEM_PAGE_WRITABLE : 0) |
    (mr->executable ? VIRTMEM_PAGE_EXECUTABLE : 0);

  if (!paddr) 
    return memory_region_map_allocate(as->pd, mr->start, n_pages, page_flags);
  else
    return memory_region_map_exact(as->pd, mr->start, paddr, n_pages, 
        page_flags);
}

void 
common_memory_region_cow_fault(struct memory_region *mr VAR_UNUSED, 
    int location VAR_UNUSED) {
  assert(0); /* Not implemented yet */
}

void 
common_memory_region_fault(struct memory_region *mr VAR_UNUSED, 
    int location VAR_UNUSED) {
  assert(0); /* Not implemented yet */
}

address_space_err_t 
common_address_space_destroy(struct address_space *as VAR_UNUSED) {
  assert(0); /* Not implemented yet */
  return AS_SUCCESS;
}

address_space_err_t 
common_address_space_get_region( struct address_space *as, 
    struct memory_region **mr, virtaddr_t loc) {
  struct memory_region *i;

  if (!mr)
    return AS_INVALID;
  if (!as) {
    *mr = NULL;
    return AS_INVALID;
  }

  LIST_FOREACH(i, &as->regions, regions) {
    if (memory_region_compare_to_location(i, loc) == 0) {
      *mr = i;
      return AS_SUCCESS;
    }
  }

  *mr = NULL;
  return AS_NOTFOUND;
}

/* Add a memory region to an address space with given start address and lenth.
 * Attempting to create a region that uses already-occupied space will result in
 * AS_USED being returned.  Start address will be rounded down to the nearest
 * page size, and should be taken as a hint.
 */
address_space_err_t
common_address_space_init_region(struct address_space *as,
    struct memory_region *mr, virtaddr_t start, size_t len) {
  if (start == 0) {
    /* generate random address */  
    return AS_INVALID; /* temporarily */
  }
  long pgsize = physmem_page_size(cpu()->localmem);
  start = start - ((long)start % pgsize);
  len = (len + pgsize - 1) / pgsize * pgsize;

  if (memory_region_set_location(mr, start, len) != AS_SUCCESS)
    return AS_INVALID;

  if (!memory_region_available_in_address_space(as, mr)) 
    return AS_USED;


  LIST_INSERT_HEAD(&as->regions, mr, regions);
  return AS_SUCCESS;

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
  kmem_cache_free(mr_cache, mr);
  return AS_SUCCESS;
}

address_space_err_t 
address_space_free(struct address_space *as) {
  kmem_cache_free(as_cache, as);
  return AS_SUCCESS;
}

address_space_err_t 
address_space_alloc(struct address_space **as) {
  *as = kmem_cache_alloc(as_cache);
  if (!as)
    return AS_OOM;
  LIST_INIT(&(*as)->regions);
  virtmem_user_setup_kernelspace(cpu()->kvirt, (*as)->pd);
  return AS_SUCCESS;
}

/* Initialize address space system
 *
 * @param as_ctor Function to be called to set up arch-dependent as funcs
 * @param mr_dtor Function to be called to set up arch-dependent mr funcs
 *
 */
void
address_space_init(kmem_cache_ctor as_ctor, kmem_cache_ctor mr_ctor,
    kmem_cache_dtor as_dtor, kmem_cache_ctor mr_dtor) {

  as_cache = kmem_alloc(kernel()->bsp->allocator);
  mr_cache = kmem_alloc(kernel()->bsp->allocator);

  kmem_cache_init(kernel()->bsp->allocator, as_cache, kernel()->bsp, 
      "address_space", sizeof(struct address_space), as_ctor, as_dtor);
  kmem_cache_init(kernel()->bsp->allocator, mr_cache, kernel()->bsp, 
      "memory_region", sizeof(struct memory_region), mr_ctor, mr_dtor);

}


#ifdef UNITTEST
int unittest_memory_region_compare_to_location(struct memory_region *mr,
    virtaddr_t addr) {
  return memory_region_compare_to_location(mr, addr);
}

int unittest_memory_region_compare_to_region(struct memory_region *mr,
    struct memory_region *other) {
  return memory_region_compare_to_region(mr, other);
}

int unittest_memory_region_available_in_address_space(struct address_space *as,
    struct memory_region *mr) {
  return memory_region_available_in_address_space(as, mr);
}

address_space_err_t unittest_memory_region_map_exact(virtmem_md_context_t pd
    , virtaddr_t vstart, physaddr_t pstart, size_t n_pages, int flags) {
  return memory_region_map_exact(pd, vstart, pstart, n_pages, flags);
}

address_space_err_t unittest_memory_region_map_allocate(
    virtmem_md_context_t pd, virtaddr_t vstart, size_t n_pages, int flags) {
  return memory_region_map_allocate(pd, vstart, n_pages, flags);
}
#endif

