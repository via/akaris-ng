#include <check.h>
#include "kernel.h"
#include "test_physmem.h"
#include "virtual_memory.h"
#include "slab.h"
#include "address_space.h"
#include "test_slab.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>


static struct kernel test_kernel;

static struct cpu test_cpu = {
    .allocator = &test_allocator,
};

struct cpu *cpu() {
  return &test_cpu;
}

struct kernel *kernel() {
  return &test_kernel;
}






Suite *
main_suite() {
  Suite *s = suite_create("Subsystems");
  TCase *tc_physmem = tcase_create("Physmem");
  check_initialize_physmem_tests(tc_physmem);
  suite_add_tcase(s, tc_physmem);

  TCase *tc_slab = tcase_create("SLAB Allocator");
  check_initialize_slab_tests(tc_slab);
  suite_add_tcase(s, tc_slab);

  TCase *tc_as = tcase_create("AddressSpace");
  check_address_space_add_static_tests(tc_as);
  suite_add_tcase(s, tc_as);

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
