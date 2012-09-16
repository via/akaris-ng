
#include "assert.h"
#include "queue.h"
#include "physical_memory.h"



physmem_error_t common_physmem_page_alloc(struct physmem *_phys, 
    uint8 node VAR_UNUSED, physaddr_t *address) {

  struct physmem_page * newpage = NULL;

  if (_phys->free_pages <= 0) { 
    return PHYSMEM_ERR_OOM;
  }

  _phys->free_pages--;

  assert(!LIST_EMPTY(&_phys->freelist));

  newpage = LIST_FIRST(&_phys->freelist);
  LIST_REMOVE(newpage, pages);


  *address = physmem_page_to_phys(_phys, newpage);

  return PHYSMEM_SUCCESS;
}

physmem_error_t common_physmem_page_free(struct physmem *_phys, physaddr_t address) {

  struct physmem_page *page = physmem_phys_to_page(_phys, address);


  LIST_INSERT_HEAD(&_phys->freelist, page, pages);

  _phys->free_pages++;

  return PHYSMEM_SUCCESS;
}

struct physmem_stats common_physmem_stats_get(const struct physmem *_phys) {

  struct physmem_stats a;
  a.kernel_pages = _phys->total_pages;
  a.free_pages = _phys->free_pages;

  return a;

}

uint32 common_physmem_page_size(const struct physmem *_phys VAR_UNUSED) {

  assert(0);
  return 0;

}

static physmem_error_t feeder_physmem_page_free(struct physmem *p, 
    physaddr_t addr) {

  struct feeder_physmem *feeder = (struct feeder_physmem *)p;

  /* For the time being, keep all returned pages unless the source is running
   * low */

  if (feeder->source->free_pages < feeder->min_free_source_pages) {
    physmem_page_free(feeder->source, addr);
  } else {
    /* Keep page here */
    feeder->source->v.page_free(p, addr);
  }

  return PHYSMEM_SUCCESS;
}

static physmem_error_t feeder_physmem_page_alloc(struct physmem *p, 
    uint8 node, physaddr_t *addr) {

  struct feeder_physmem *feeder = (struct feeder_physmem *)p;
  
  if (p->free_pages < feeder->pages_to_keep + 1) { /* +1 because we are about
                                                      to alloc one page */
    /* Pull and add (pages_to_keep - free_pages) to freelist */
    int xfer_pages = (feeder->pages_to_keep - p->free_pages + 1);
    physaddr_t newaddr;
    struct physmem_page *newpage;
    while (xfer_pages) {
      if (feeder->source->free_pages <= feeder->min_free_source_pages) break;
      feeder->source->v.page_alloc(feeder->source, node, &newaddr);
      newpage = feeder->source->v.phys_to_page(feeder->source, newaddr);
      LIST_INSERT_HEAD(&p->freelist, newpage, pages);
      p->free_pages++;
      p->total_pages++;
      --xfer_pages;
    }
  } 

  if (p->free_pages > 0) {
    /* Use source's alloc function, but using this data */
    return feeder->source->v.page_alloc(p, node, addr);
  } else {
    return physmem_page_alloc(feeder->source, node, addr);
  }

}


struct physmem_stats feeder_physmem_stats_get(const struct physmem *p) {

  struct physmem_stats a;
  a.kernel_pages = p->total_pages;
  a.free_pages = p->free_pages;

  return a;
}

void feeder_physmem_create(struct feeder_physmem *dest,
    struct physmem *source, unsigned int kept_pages,
    unsigned int min_source_pages) {

  dest->source = source;
  dest->pages_to_keep = kept_pages;
  dest->min_free_source_pages = min_source_pages;
  dest->p.parent = source->parent;
  dest->p.name = "feeder";
  dest->p.v.phys_to_page = source->v.phys_to_page;
  dest->p.v.page_to_phys = source->v.page_to_phys;
  dest->p.v.page_alloc = feeder_physmem_page_alloc;
  dest->p.v.page_free = feeder_physmem_page_free;
  dest->p.v.stats_get = feeder_physmem_stats_get;
  dest->p.v.page_size = source->v.page_size;

  LIST_INIT(&dest->p.freelist);
  
  dest->p.total_pages = 0;
  dest->p.free_pages = 0;




}

/* Interfaces */

struct physmem_page * physmem_phys_to_page(struct physmem *p, physaddr_t addr) {

  return p->v.phys_to_page(p, addr);

}


physaddr_t physmem_page_to_phys(struct physmem *p, const struct physmem_page *page) {

  return p->v.page_to_phys(p, page);

}

physmem_error_t
physmem_page_alloc(struct physmem *p, uint8 n, physaddr_t *addr) {

  return p->v.page_alloc(p, n, addr);

}

physmem_error_t
physmem_page_free(struct physmem *p, physaddr_t addr) {

  return p->v.page_free(p, addr);

}

struct physmem_stats
physmem_stats_get(const struct physmem *p) {

  return p->v.stats_get(p);

}

uint32 physmem_page_size(const struct physmem *p) {

  return p->v.page_size(p);

}
