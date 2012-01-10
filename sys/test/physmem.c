
#include "physmem.h"
#include "strfuncs.h"

#include <stdlib.h>



struct physmem *
test_physmem_alloc(struct kernel *kernel, int n_pages) {

  int count;

  struct test_physmem *tphysmem = (struct test_physmem *)malloc(
      sizeof(struct test_physmem));

  memcpy(tphysmem, &t_physmem, sizeof(struct test_physmem));

  LIST_INIT(&tphysmem->p.freelist);
  tphysmem->p.total_pages = n_pages;
  tphysmem->p.free_pages = n_pages;
  tphysmem->pagelist = (struct physmem_page *)malloc(
      sizeof(struct physmem) * n_pages);

  for (count = n_pages; count > 0; --count) {
    LIST_INSERT_HEAD(&tphysmem->p.freelist,
        &tphysmem->pagelist[count - 1], pages);
  }

  return &tphysmem->p;

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
      common_physmem_page_alloc,
      common_physmem_page_free,
      common_physmem_stats_get,
      test_physmem_page_size,
    }
  },
};
