#include "kernel.h"
#include "physmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <check.h>

int
oldmain() {
  struct kernel test_kernel;
  test_kernel.phys = test_physmem_alloc(&test_kernel, 1024);

  struct feeder_physmem test_feeder;

  printf("Hosted: Startup!\n");

  feeder_physmem_create(&test_feeder, test_kernel.phys, 100, 512);

  physaddr_t addr, old;
  physmem_error_t status = PHYSMEM_SUCCESS;
  int num_pages = 0;
  while (status == PHYSMEM_SUCCESS) {
    num_pages++;
/*    status = test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &addr);*/
    status = test_feeder.p.v.page_alloc(&test_feeder.p, 0, &addr);
    printf(":  Page allocated at address: %x\n", addr);
  }
  printf("Num allocated pages: %d\n", num_pages - 1);


  return 0;
}

START_TEST (test_physmem_page_alloc) {
  fail_if(0);
} END_TEST

START_TEST (test_physmem_page_free) {
  fail_if(0);
} END_TEST

Suite *
main_suite() {
  Suite *s = suite_create("Subsystems");
  TCase *tc_physmem = tcase_create("Physmem");
  tcase_add_test(tc_physmem, test_physmem_page_alloc);
  tcase_add_test(tc_physmem, test_physmem_page_free);
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
