
#include "physmem.h"
#include "strfuncs.h"

#include <stdlib.h>



struct physmem *
test_physmem_alloc(struct kernel *kernel, int n_pages) {

  int count;

  LIST_INIT(&t_physmem.p.freelist);
  t_physmem.p.total_pages = n_pages;
  t_physmem.p.free_pages = n_pages;
  t_physmem.pagelist = (struct physmem_page *)malloc(
      sizeof(struct physmem) * n_pages);

  for (count = n_pages; count > 0; --count) {
    LIST_INSERT_HEAD(&t_physmem.p.freelist,
        &t_physmem.pagelist[count - 1], pages);
  }

  return &t_physmem.p;

}


static uint32 test_physmem_page_size(const struct physmem *p) {
  return 4096;
}

static struct physmem_page * test_physmem_phys_to_page(const struct physmem *_p, physaddr_t addr) {

  const struct test_physmem *p = (const struct test_physmem *)_p;
  int index = (addr / _p->v.page_size(_p));

  return &p->pagelist[index];
}

static physaddr_t test_physmem_page_to_phys(const struct physmem *_p, 
    const struct physmem_page *page) {


  const struct test_physmem *p = (const struct test_physmem *)_p;
  int index = ((void *)page - (void *)p->pagelist) / sizeof(struct physmem_page);

  return index * _p->v.page_size(_p);


}

struct test_physmem t_physmem = {
  .p = {
    .name = "testphysmem",
    .v = {
      test_physmem_phys_to_page,
      test_physmem_page_to_phys,
      physmem_page_alloc,
      physmem_page_free,
      physmem_stats_get,
      test_physmem_page_size,
    }
  },
};
