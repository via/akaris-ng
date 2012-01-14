#include "kernel.h"
#include "physmem.h"
#include "slab.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <check.h>

static const int ctor_marker = 0x11;
static const int dtor_marker = 0x22;

START_TEST (check_physmem_alloc) {

  struct kernel test_kernel;
  const int n_pages = 24;
  physmem_error_t err;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);

  fail_unless(test_kernel.phys->parent == &test_kernel);
  fail_unless(test_kernel.phys->total_pages == n_pages);
  fail_unless(test_kernel.phys->free_pages == n_pages);

} END_TEST

START_TEST (check_physmem_page_alloc) { 
  
  struct kernel test_kernel;
  const int n_pages = 24;
  int count;
  physmem_error_t err;
  physaddr_t page;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);

  for (count = 0; count < n_pages; ++count) {
    err = physmem_page_alloc(test_kernel.phys, 0, &page);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(test_kernel.phys->total_pages == n_pages);
    fail_unless(test_kernel.phys->free_pages == n_pages - (count + 1));
  }

  err = physmem_page_alloc(test_kernel.phys, 0, &page);
  fail_if(err != PHYSMEM_ERR_OOM);

} END_TEST

START_TEST (check_physmem_page_free) {

  struct kernel test_kernel;
  const int n_pages = 24;
  int count;
  physmem_error_t err;
  physaddr_t page;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);

  for (count = 0; count < n_pages; ++count) {
    err = physmem_page_alloc(test_kernel.phys, 0, &page);
    fail_unless(err == PHYSMEM_SUCCESS);
  }
  
  for (count = 0; count < n_pages; ++count) {
    err = physmem_page_free(test_kernel.phys, 0);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(test_kernel.phys->free_pages == count + 1);
  }

} END_TEST

START_TEST (check_physmem_stats) {

  struct kernel test_kernel;
  const int n_pages = 24;
  int count;
  struct physmem_stats stats;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);
  stats = physmem_stats_get(test_kernel.phys);
  fail_unless(stats.kernel_pages == n_pages);
  fail_unless(stats.free_pages == n_pages);

} END_TEST

START_TEST (check_physmem_feeder_create) {

  struct kernel test_kernel;
  const int n_pages = 24;
  const int kept_pages = 5;
  const int min_source_pages = 5;
  int count;
  struct feeder_physmem f;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);
  feeder_physmem_create(&f, test_kernel.phys, kept_pages, min_source_pages);

  fail_unless(f.source == test_kernel.phys);
  fail_unless(f.pages_to_keep == kept_pages);
  fail_unless(f.min_free_source_pages = min_source_pages);
  fail_unless(f.p.parent == &test_kernel);
  fail_unless(strcmp(f.p.name, "feeder") == 0);
  fail_unless(LIST_EMPTY(&f.p.freelist));
  fail_unless(f.p.total_pages == 0);
  fail_unless(f.p.free_pages == 0);

} END_TEST



START_TEST (check_physmem_feeder_feeds_correctly) {

  struct kernel test_kernel;
  const int n_pages = 24;
  const int kept_pages = 5;
  const int min_source_pages = 5;
  int count;
  struct feeder_physmem f;
  kmem_error_t err;
  physaddr_t addr;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);
  feeder_physmem_create(&f, test_kernel.phys, kept_pages, min_source_pages);

  err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
  fail_unless(err == PHYSMEM_SUCCESS);
  fail_unless(f.p.free_pages >= kept_pages);
  /* Make sure that while the source has sufficient free pages, the feeder will
   * remain replenished */
  while (test_kernel.phys->free_pages > min_source_pages) {
    err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
    fail_unless(err == PHYSMEM_SUCCESS);
  }
  fail_unless(f.p.free_pages == kept_pages);
  /*For the next min_source_pages, make sure feeder does not draw from source,
   * and allows the feeder supply to reach 0 */
  for (count = min_source_pages - 1; count >= 0; --count) {
    err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(f.p.free_pages == count);
    fail_unless(f.source->free_pages == min_source_pages);
  }
  /*After that point, all allocs should directly affect the source, while the
   * feeder remains empty */
  for (count = min_source_pages - 1; count >= 0; --count) {
    err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(f.source->free_pages == count);
    fail_unless(f.p.free_pages == 0);
  }
  /* Now we're out of memory */
  err = physmem_page_alloc((struct physmem *)&f, 0, &addr);
  fail_unless(err == PHYSMEM_ERR_OOM);

} END_TEST


START_TEST (check_physmem_feeder_frees_correctly) {
  
  struct kernel test_kernel;
  const int n_pages = 24;
  const int kept_pages = 5;
  const int min_source_pages = 5;
  int count;
  struct feeder_physmem f;
  kmem_error_t err;
  physaddr_t addr;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);
  feeder_physmem_create(&f, test_kernel.phys, kept_pages, min_source_pages);

  for (count = n_pages; count > 0; --count)
    physmem_page_alloc((struct physmem *)&f, 0, &addr);

  fail_unless(f.p.free_pages == 0);
  fail_unless(test_kernel.phys->free_pages == 0);

  for (count = 0; count < min_source_pages; ++count) {
    physmem_page_free(&f.p, 0);
    fail_unless(f.p.free_pages == 0);
    fail_unless(test_kernel.phys->free_pages == count);
  }

  for (count = 1; count <= kept_pages; ++count) {
    physmem_page_free(&f.p, 0);
    fail_unless(f.p.free_pages == count);
    fail_unless(test_kernel.phys->free_pages == min_source_pages);
  }
  /* Need to implement max/min for kept pages, and test it */

} END_TEST

void check_kmem_cache_ctor(void *_obj) {
  int *obj = (int *)_obj;
  *obj = ctor_marker;
}

void check_kmem_cache_dtor(void *_obj) {
  int *obj = (int *)_obj;
  *obj = dtor_marker;
}

/* I know this is huge, need to learn how to use fixtures to more easily split
 * apart the source
 */
START_TEST (check_kmem_cache_create) {

  const int slab_size = 32;
  kmem_error_t err;

  struct kmem_cache *k = (struct kmem_cache *)
    malloc(sizeof(struct kmem_cache));

  struct kmem_allocator allocator = {
    .cv = {
      .reap = NULL,
      .alloc = NULL,
      .free = NULL,
      .new_slab = NULL,
    },
  };

  struct cpu c = {
    .allocator = &allocator,
  };

  err = common_kmem_cache_create(k, &c, "test-slab-32", slab_size,
      check_kmem_cache_ctor, check_kmem_cache_dtor);

  fail_unless(err == KMEM_SUCCESS);
  fail_unless(k->ctor == check_kmem_cache_ctor);
  fail_unless(k->dtor == check_kmem_cache_dtor);
  fail_unless(strcmp(k->name, "test-slab-32") == 0);
  fail_unless(k->objsize == slab_size);
  fail_unless(k->used == 0);
  fail_unless(k->cpu == &c);
  fail_unless(SLIST_EMPTY(&k->slabs_empty));
  fail_unless(SLIST_EMPTY(&k->slabs_full));
  fail_unless(SLIST_EMPTY(&k->slabs_partial));
  fail_unless(k->v == &allocator.cv);

  /*Now test case of size < 4 */
  err = common_kmem_cache_create(k, &c, "test-slab-32", 2,
      check_kmem_cache_ctor, check_kmem_cache_dtor);
  fail_unless(err == KMEM_ERR_INVALID);

  free(k);

} END_TEST

START_TEST (check_kmem_cache_alloc_ctor_dtor) {

  fail_if(1);

} END_TEST



Suite *
main_suite() {
  Suite *s = suite_create("Subsystems");
  TCase *tc_physmem = tcase_create("Physmem");
  tcase_add_test(tc_physmem, check_physmem_alloc);
  tcase_add_test(tc_physmem, check_physmem_page_alloc);
  tcase_add_test(tc_physmem, check_physmem_page_free);
  tcase_add_test(tc_physmem, check_physmem_stats);
  tcase_add_test(tc_physmem, check_physmem_feeder_create);
  tcase_add_test(tc_physmem, check_physmem_feeder_feeds_correctly);
  tcase_add_test(tc_physmem, check_physmem_feeder_frees_correctly);
  suite_add_tcase(s, tc_physmem);

  TCase *tc_slab = tcase_create("SLAB Allocator");
  tcase_add_test(tc_slab, check_kmem_cache_create);
  tcase_add_test(tc_slab, check_kmem_cache_alloc_ctor_dtor);
  suite_add_tcase(s, tc_slab);

  return s;
}

int main(void) {
  int num_failed;
  Suite *s = main_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);
  num_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
