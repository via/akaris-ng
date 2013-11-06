#include <check.h>
#undef strcpy
#undef strstr
#include "test_physmem.h"
#include <string.h>

#include <stdlib.h>
#include "test.h"
#include "kernel.h"


struct physmem *
test_physmem_alloc(struct kernel *kernel, int n_pages) {

  int count;

  struct test_physmem *tphysmem = (struct test_physmem *)malloc(
      sizeof(struct test_physmem));

  memcpy(tphysmem, &t_physmem, sizeof(struct test_physmem));

  LIST_INIT(&tphysmem->p.freelist);
  tphysmem->p.total_pages = n_pages;
  tphysmem->p.free_pages = n_pages;
  tphysmem->p.parent = kernel;
  tphysmem->pagelist = (struct physmem_page *)malloc(
      sizeof(struct physmem) * n_pages);

  for (count = n_pages; count > 0; --count) {
    LIST_INSERT_HEAD(&tphysmem->p.freelist,
        &tphysmem->pagelist[count - 1], pages);
  }

  return &tphysmem->p;

}


static uint32 test_physmem_page_size(const struct physmem *p VAR_UNUSED) {
  return 4096;
}

static struct physmem_page * test_physmem_phys_to_page(const struct physmem *_p, physaddr_t addr) {

  const struct test_physmem *p = (const struct test_physmem *)_p;
  int index = (addr / physmem_page_size(_p));

  return &p->pagelist[index];
}

static physaddr_t test_physmem_page_to_phys(const struct physmem *_p, 
    const struct physmem_page *page) {


  const struct test_physmem *p = (const struct test_physmem *)_p;
  int index = ((void *)page - (void *)p->pagelist) / sizeof(struct physmem_page);

  return index * physmem_page_size(_p);


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

static const unsigned int n_pages = 24;
void check_physmem_setup() {
  kernel()->phys = test_physmem_alloc(kernel(), n_pages);
}

START_TEST (check_physmem_alloc) {

  fail_unless(kernel()->phys->parent == kernel());
  fail_unless(kernel()->phys->total_pages == n_pages);
  fail_unless(kernel()->phys->free_pages == n_pages);

} END_TEST

START_TEST (check_physmem_page_alloc) { 
  
  unsigned int count;
  physmem_error_t err;
  physaddr_t page;

  for (count = 0; count < n_pages; ++count) {
    err = physmem_page_alloc(kernel()->phys, 0, &page);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(kernel()->phys->total_pages == n_pages);
    fail_unless(kernel()->phys->free_pages == n_pages - (count + 1));
  }

  err = physmem_page_alloc(kernel()->phys, 0, &page);
  fail_if(err != PHYSMEM_ERR_OOM);

} END_TEST

START_TEST (check_physmem_page_free) {

  unsigned int count;
  physmem_error_t err;
  physaddr_t page;

  for (count = 0; count < n_pages; ++count) {
    err = physmem_page_alloc(kernel()->phys, 0, &page);
    fail_unless(err == PHYSMEM_SUCCESS);
  }
  
  for (count = 0; count < n_pages; ++count) {
    err = physmem_page_free(kernel()->phys, 0);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(kernel()->phys->free_pages == count + 1);
  }

} END_TEST

START_TEST (check_physmem_stats) {

  struct physmem_stats stats;

  stats = physmem_stats_get(kernel()->phys);
  fail_unless(stats.kernel_pages == n_pages);
  fail_unless(stats.free_pages == n_pages);

} END_TEST

START_TEST (check_physmem_feeder_create) {

  const unsigned int kept_pages = 5;
  const unsigned int min_source_pages = 7;
  struct feeder_physmem f;

  feeder_physmem_create(&f, kernel()->phys, kept_pages, min_source_pages);

  fail_unless(f.source == kernel()->phys);
  fail_unless(f.pages_to_keep == kept_pages);
  fail_unless(f.min_free_source_pages = min_source_pages);
  fail_unless(f.p.parent == kernel());
  fail_unless(strcmp(f.p.name, "feeder") == 0);
  fail_unless(LIST_EMPTY(&f.p.freelist));
  fail_unless(f.p.total_pages == 0);
  fail_unless(f.p.free_pages == 0);

} END_TEST



START_TEST (check_physmem_feeder_feeds_correctly) {
  const unsigned int kept_pages = 5;
  const unsigned int min_source_pages = 7;
  int count;
  struct feeder_physmem f;
  physmem_error_t err;
  physaddr_t addr;

  feeder_physmem_create(&f, kernel()->phys, kept_pages, min_source_pages);

  err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
  fail_unless(err == PHYSMEM_SUCCESS);
  fail_unless(f.p.free_pages >= kept_pages);
  /* Make sure that while the source has sufficient free pages, the feeder will
   * remain replenished */
  while (kernel()->phys->free_pages > min_source_pages ) {
    err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
    fail_unless(err == PHYSMEM_SUCCESS);
  }

  /*For the next min_source_pages, make sure feeder does not draw from source,
   * and allows the feeder supply to reach 0 */
  for (count = kept_pages - 1; count >= 0; --count) {
    err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(f.p.free_pages == (unsigned)count);
    fail_unless(f.source->free_pages == min_source_pages);
  }
  /*After that point, all allocs should directly affect the source, while the
   * feeder remains empty */
  for (count = min_source_pages - 1; count >= 0; --count) {
    err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(f.source->free_pages == (unsigned)count);
    fail_unless(f.p.free_pages == 0);
  }
  /* Now we're out of memory */
  err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
  fail_unless(err == PHYSMEM_ERR_OOM);

} END_TEST


START_TEST (check_physmem_feeder_frees_correctly) {
  
  const unsigned int kept_pages = 5;
  const unsigned int min_source_pages = 5;
  unsigned int count;
  struct feeder_physmem f;
  physaddr_t addr;

  feeder_physmem_create(&f, kernel()->phys, kept_pages, min_source_pages);

  for (count = n_pages; count > 0; --count)
    physmem_page_alloc((struct physmem *)&f, 0, &addr);

  fail_unless(f.p.free_pages == 0);
  fail_unless(kernel()->phys->free_pages == 0);

  for (count = 1; count <= min_source_pages; ++count) {
    physmem_page_free(&f.p, 0);
    fail_unless(f.p.free_pages == 0);
    fail_unless(kernel()->phys->free_pages == count);
  }

  for (count = 1; count <= kept_pages; ++count) {
    physmem_page_free(&f.p, 0);
    fail_unless(f.p.free_pages == count);
    fail_unless(kernel()->phys->free_pages == min_source_pages);
  }
  /* Need to implement max/min for kept pages, and test it */

} END_TEST

void check_initialize_physmem_tests(TCase *t) {
  tcase_add_checked_fixture(t, check_physmem_setup, NULL);
  tcase_add_test(t, check_physmem_alloc);
  tcase_add_test(t, check_physmem_page_alloc);
  tcase_add_test(t, check_physmem_page_free);
  tcase_add_test(t, check_physmem_stats);
  tcase_add_test(t, check_physmem_feeder_create);
  tcase_add_test(t, check_physmem_feeder_feeds_correctly);
  tcase_add_test(t, check_physmem_feeder_frees_correctly);
}
