
#include <check.h>
#include "queue.h"
#include "slab.h"
#include "virtual_memory.h"
#include "address_space.h"
#include "test_address_space.h"

START_TEST (check_memory_region_compare_to_location) {
  struct memory_region mr;
  virtaddr_t loc;

  mr.start = (virtaddr_t)0x100000;
  mr.length = 8192;

  loc = (virtaddr_t)0x80000;
  fail_unless(unittest_memory_region_compare_to_location(&mr, loc) < 0);

  loc = (virtaddr_t)0x100100;
  fail_unless(unittest_memory_region_compare_to_location(&mr, loc) == 0);

  loc = (virtaddr_t)0x102000;
  fail_unless(unittest_memory_region_compare_to_location(&mr, loc) > 0);
} END_TEST

START_TEST (check_memory_region_compare_to_region) {
  struct memory_region mr1, mr2;
  mr1.start = (virtaddr_t)0x100000;
  mr1.length = 0x4000;

  /* Completely below */
  mr2.start = (virtaddr_t)0xfc000;
  mr2.length = 0x4000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) < 0);

  /* Overlaps on low side */
  mr2.start = (virtaddr_t)0xfc000;
  mr2.length = 0x5000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) == 0);

  /* Overlaps on high side */
  mr2.start = (virtaddr_t)0x103000;
  mr2.length = 0x4000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) == 0);

  /* Fully inside */
  mr2.start = (virtaddr_t)0x101000;
  mr2.length = 0x1000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) == 0);

  /* Fully encompassing mr1 */
  mr2.start = (virtaddr_t)0xfc000;
  mr2.length = 0x10000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) == 0);

  /* Matches */
  mr2.start = (virtaddr_t)0x100000;
  mr2.length = 0x4000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) == 0);

  /* Above */
  mr2.start = (virtaddr_t)0x104000;
  mr2.length = 0x4000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) > 0);

} END_TEST

START_TEST (check_memory_region_available_in_address_space) {
  struct address_space as;
  struct memory_region mr1, mr2, mr3;

  mr1.start = (virtaddr_t)0x100000;
  mr1.length = 0x4000;

  mr2.start = (virtaddr_t)0x108000;
  mr2.length = 0x4000;

  LIST_INIT(&as.regions);
  LIST_INSERT_HEAD(&as.regions, &mr1, regions);
  LIST_INSERT_HEAD(&as.regions, &mr2, regions);

  /* Below them both */
  mr3.start = (virtaddr_t)0xfc000;
  mr3.length = 0x4000;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Overlapping one */
  mr3.start = (virtaddr_t)0xfc000;
  mr3.length = 0x8000;
  fail_if(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Overlapping both */
  mr3.start = (virtaddr_t)0xfc000;
  mr3.length = 0x10000;
  fail_if(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Fully between */
  mr3.start = (virtaddr_t)0x105000;
  mr3.length = 0x2000;;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Fully between, touching both */
  mr3.start = (virtaddr_t)0x104000;
  mr3.length = 0x4000;;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Fully after */
  mr3.start = (virtaddr_t)0x110000;
  mr3.length = 0x4000;;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));
} END_TEST

void check_initialize_address_space_tests(TCase *t) {
   tcase_add_test(t, check_memory_region_compare_to_region);
   tcase_add_test(t, check_memory_region_compare_to_location);
   tcase_add_test(t, check_memory_region_available_in_address_space);
}
