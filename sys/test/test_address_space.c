
#include <check.h>
#include "test_address_space.h"

START_TEST (check_memory_region_compare_to_location) {
  struct memory_region mr;
  virtaddr_t loc;

  mr.start = (virtaddr_t)0x100000;
  mr.length = 8192;

  loc = (virtaddr_t)0x80000;
  fail_unless(memory_region_compare_to_location(&mr, loc) < 0);

  loc = (virtaddr_t)0x100100;
  fail_unless(memory_region_compare_to_location(&mr, loc) == 0);

  loc = (virtaddr_t)0x102000;
  fail_unless(memory_region_compare_to_location(&mr, loc) > 0);
} END_TEST

void check_initialize_address_space_tests(TCase *t) {
   tcase_add_test(t, check_memory_region_compare_to_location);
}
