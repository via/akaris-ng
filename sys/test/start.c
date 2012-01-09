#include "kernel.h"
#include "physmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <check.h>

START_TEST (check_physmem_alloc) {

  struct kernel test_kernel;
  const int n_pages = 24;
  physmem_error_t err;

  test_kernel.phys = test_physmem_alloc(&test_kernel, n_pages);

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
    err = test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &page);
    fail_unless(err == PHYSMEM_SUCCESS);
    fail_unless(test_kernel.phys->total_pages == n_pages);
    fail_unless(test_kernel.phys->free_pages == n_pages - (count + 1));
  }

  err = test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &page);
  fail_if(err != PHYSMEM_ERR_OOM);

} END_TEST

START_TEST (check_physmem_page_free) {
  fail_if(0);
} END_TEST

Suite *
main_suite() {
  Suite *s = suite_create("Subsystems");
  TCase *tc_physmem = tcase_create("Physmem");
  tcase_add_test(tc_physmem, check_physmem_alloc);
  tcase_add_test(tc_physmem, check_physmem_page_alloc);
  tcase_add_test(tc_physmem, check_physmem_page_free);
  suite_add_tcase(s, tc_physmem);
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
