
#include "assert.h"
#include "queue.h"
#include "physical_memory.h"



physmem_error_t physmem_page_alloc(struct physmem *_phys, uint8 node, physaddr_t *address) {

  struct physmem_page * newpage = NULL;

  if (_phys->free_pages <= 0) { 
    return PHYSMEM_ERR_OOM;
  }

  _phys->free_pages--;

  ASSERT(!SLIST_EMPTY(&_phys->freelist));

  newpage = SLIST_FIRST(&_phys->freelist);
  SLIST_REMOVE_HEAD(&_phys->freelist, pages);


  *address = _phys->v.page_to_phys(_phys, newpage);

  return PHYSMEM_SUCCESS;
}

physmem_error_t physmem_page_free(struct physmem *_phys, physaddr_t address) {

  struct physmem_page *page = _phys->v.phys_to_page(_phys, address);

  SLIST_INSERT_HEAD(&_phys->freelist, page, pages);

  _phys->free_pages++;

  return PHYSMEM_SUCCESS;
}

struct physmem_stats physmem_stats_get(const struct physmem *_phys) {

  struct physmem_stats a;
  a.kernel_pages = _phys->total_pages;
  a.free_pages = _phys->free_pages;

  return a;

}

uint32 physmem_page_size(const struct physmem *_phys) {

  ASSERT(0);
  return 0;

}

