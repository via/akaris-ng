
#include <check.h>
#include "queue.h"
#include "slab.h"
#include "virtual_memory.h"
#include "address_space.h"
#include "test_address_space.h"
#include "test_physmem.h"
#include "mock.h"

static struct address_space as;
static struct memory_region mr1, mr2, mr3;

mock_call_list virtmem_mock_calls;


START_TEST (check_memory_region_compare_to_location) {
  virtaddr_t loc;

  mr1.start = (virtaddr_t)0x100000;
  mr1.length = 8192;

  loc = (virtaddr_t)0x80000;
  fail_unless(unittest_memory_region_compare_to_location(&mr1, loc) < 0);

  loc = (virtaddr_t)0x100100;
  fail_unless(unittest_memory_region_compare_to_location(&mr1, loc) == 0);

  loc = (virtaddr_t)0x102000;
  fail_unless(unittest_memory_region_compare_to_location(&mr1, loc) > 0);
} END_TEST

START_TEST (check_memory_region_compare_to_region) {
  mr1.start = (virtaddr_t)0x100000;
  mr1.length = 0x4000;

  /* Completely below */
  mr2.start = (virtaddr_t)0xfc000;
  mr2.length = 0x4000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) < 0);

  /* same start, finishes inside */
  mr2.start = (virtaddr_t)0x100000;
  mr2.length = 0x1000;
  fail_unless(unittest_memory_region_compare_to_region(&mr1, &mr2) == 0);

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
  mr3.length = 0x2000;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Fully between, touching both */
  mr3.start = (virtaddr_t)0x104000;
  mr3.length = 0x4000;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Starts at beginning of one, finishes before it ends */
  mr3.start = (virtaddr_t)0x100000;
  mr3.length = 0x2000;
  fail_if(unittest_memory_region_available_in_address_space(&as, &mr3));

  /* Fully after */
  mr3.start = (virtaddr_t)0x110000;
  mr3.length = 0x4000;
  fail_unless(unittest_memory_region_available_in_address_space(&as, &mr3));
} END_TEST

static int
address_space_number_of_regions(struct address_space *as) {
  int count = 0;
  struct memory_region *mr;
  LIST_FOREACH(mr, &as->regions, regions) {
    count++;
  }
  return count;
}

START_TEST (check_common_address_space_init_region) {
  address_space_err_t err;

  LIST_INIT(&as.regions);

  err = common_address_space_init_region(&as, &mr1, (virtaddr_t)0x100000, 
      0x4000);
  fail_unless(err == AS_SUCCESS);

  fail_unless(mr1.start == (virtaddr_t)0x100000);
  fail_unless(mr1.length == 0x4000);
  fail_unless(address_space_number_of_regions(&as) == 1);

  err = common_address_space_init_region(&as, &mr2, (virtaddr_t)0x100000, 
      0x20);
  fail_unless(err == AS_USED);
  fail_unless(address_space_number_of_regions(&as) == 1);


} END_TEST

START_TEST (check_common_memory_region_set_location) {
  fail_unless(common_memory_region_set_location(NULL, (virtaddr_t)0, 0) 
      == AS_INVALID);
  fail_unless(common_memory_region_set_location(&mr1, (virtaddr_t)0x1000,
        0x10) == AS_SUCCESS);
  fail_unless(mr1.start == (virtaddr_t)0x1000);
  fail_unless(mr1.length == 0x10);
} END_TEST

START_TEST (check_common_memory_region_set_flags) {
  fail_unless(common_memory_region_set_flags(NULL, 0, 0) == AS_INVALID);
  fail_unless(common_memory_region_set_flags(&mr1, 1, 1) == AS_INVALID);

  fail_unless(common_memory_region_set_flags(&mr1, 1, 0) == AS_SUCCESS);
  fail_unless(mr1.writable == 1);
  fail_unless(mr1.executable == 0);

  fail_unless(common_memory_region_set_flags(&mr1, 0, 1) == AS_SUCCESS);
  fail_unless(mr1.writable == 0);
  fail_unless(mr1.executable == 1);

  fail_unless(common_memory_region_set_flags(&mr1, 0, 0) == AS_SUCCESS);
  fail_unless(mr1.writable == 0);
  fail_unless(mr1.executable == 0);
} END_TEST

START_TEST (check_common_address_space_get_region) {
  struct memory_region *mr = NULL;

  mr1.start = (virtaddr_t)0x100000;
  mr1.length = 0x4000;

  mr2.start = (virtaddr_t)0x108000;
  mr2.length = 0x4000;

  mr3.start = (virtaddr_t)0x110000;
  mr3.length = 0x4000;

  LIST_INIT(&as.regions);
  LIST_INSERT_HEAD(&as.regions, &mr1, regions);
  LIST_INSERT_HEAD(&as.regions, &mr2, regions);
  LIST_INSERT_HEAD(&as.regions, &mr3, regions);

  fail_unless(common_address_space_get_region(&as, &mr, 0x0) == AS_NOTFOUND);
  fail_unless(mr == NULL);

  fail_unless(common_address_space_get_region(&as, &mr, (virtaddr_t)0x100000) 
      == AS_SUCCESS);
  fail_unless(mr == &mr1);

  fail_unless(common_address_space_get_region(&as, &mr, (virtaddr_t)0x100100) 
      == AS_SUCCESS);
  fail_unless(mr == &mr1);

  fail_unless(common_address_space_get_region(&as, &mr, (virtaddr_t)0x104000) 
      == AS_NOTFOUND);
  fail_unless(mr == NULL);

  fail_unless(common_address_space_get_region(&as, &mr, (virtaddr_t)0x108100) 
      == AS_SUCCESS);
  fail_unless(mr == &mr2);

  fail_unless(common_address_space_get_region(&as, &mr, (virtaddr_t)0x110100) 
      == AS_SUCCESS);
  fail_unless(mr == &mr3);

} END_TEST

START_TEST (check_memory_region_map_exact) {

  address_space_err_t err;
  const char *emsg;
  virtmem_md_context_t pd = (virtmem_md_context_t)0x1000;
  unsigned short pgnum;
  err = unittest_memory_region_map_exact(pd, (virtaddr_t)0x10000,
      (physaddr_t)0xC0000000, 5, 
      (int)(VIRTMEM_PAGE_READABLE | VIRTMEM_PAGE_EXECUTABLE));
  fail_unless(err == AS_SUCCESS);

  for (pgnum = 0; pgnum < 5; ++pgnum) {
    fail_unless(mock_call_expect(&virtmem_mock_calls, &emsg, "user_map_page",
        MOCK_PTR(cpu()->kvirt), MOCK_PTR(pd), 
        MOCK_PTR((long)(4096 * pgnum) + 0x10000),
        MOCK_LONG((4096 * pgnum) + 0xC0000000), 
        MOCK_END()), emsg);
    fail_unless(mock_call_expect(&virtmem_mock_calls, &emsg, "user_set_page_flags",
        MOCK_PTR(cpu()->kvirt), MOCK_PTR(pd), 
        MOCK_PTR((long)(4096 * pgnum) + 0x10000),
        MOCK_INT( (int)VIRTMEM_PAGE_READABLE | (int)VIRTMEM_PAGE_EXECUTABLE),
        MOCK_END()), emsg);
  }

  fail_unless(mock_call_list_empty(&virtmem_mock_calls));

} END_TEST

START_TEST (check_memory_region_map_allocate) {

} END_TEST

static virtmem_error_t fake_user_get_page(struct virtmem *v,
    virtmem_md_context_t c, physaddr_t *paddr, virtaddr_t vaddr) {
  mock_call(&virtmem_mock_calls, "user_get_page", MOCK_PTR(v),
      MOCK_PTR(c), MOCK_PTR(paddr), MOCK_PTR(vaddr), MOCK_END());
  return VIRTMEM_SUCCESS;
}

static virtmem_error_t fake_user_map_page(struct virtmem *v,
    virtmem_md_context_t c, virtaddr_t vaddr, physaddr_t paddr) {
  mock_call(&virtmem_mock_calls, "user_map_page", MOCK_PTR(v),
      MOCK_PTR(c), MOCK_PTR(vaddr), MOCK_LONG(paddr), MOCK_END());
  return VIRTMEM_SUCCESS;
}

static virtmem_error_t fake_user_set_page_flags(struct virtmem *v,
    virtmem_md_context_t c, virtaddr_t vaddr, int flags) {
  mock_call(&virtmem_mock_calls, "user_set_page_flags", MOCK_PTR(v),
      MOCK_PTR(c), MOCK_PTR(vaddr), MOCK_INT(flags), MOCK_END());
  return VIRTMEM_SUCCESS;
}

static struct virtmem fake_virtmem;

void check_address_space_setup() {
  kernel()->phys = test_physmem_alloc(kernel(), 24);
  cpu()->localmem = kernel()->phys;
  as.v = (struct address_space_vfuncs) {
    .destroy = common_address_space_destroy,
    .get_region = common_address_space_get_region,
    .init_region = common_address_space_init_region,
    .clone_region = common_memory_region_clone,
    .map_region = common_memory_region_map,
  };
  mr1.v = (struct memory_region_vfuncs) {
    .set_location = common_memory_region_set_location,
    .set_flags = common_memory_region_set_flags,
  };
  cpu()->kvirt = &fake_virtmem;
  fake_virtmem = (struct virtmem) {
    .cpu = cpu(),
    .v = (struct virtmem_vfuncs) {
      .user_map_page = fake_user_map_page,
      .user_get_page = fake_user_get_page,
      .user_set_page_flags = fake_user_set_page_flags,
    },
  };

  memcpy(&mr2, &mr1, sizeof(mr1));
  memcpy(&mr3, &mr1, sizeof(mr1));

  mock_call_list_init(&virtmem_mock_calls);

}

void check_initialize_address_space_tests(TCase *t) {
  tcase_add_checked_fixture(t, check_address_space_setup, NULL);
   tcase_add_test(t, check_memory_region_compare_to_region);
   tcase_add_test(t, check_memory_region_compare_to_location);
   tcase_add_test(t, check_memory_region_available_in_address_space);
   tcase_add_test(t, check_memory_region_map_exact);
   tcase_add_test(t, check_memory_region_map_allocate);

   tcase_add_test(t, check_common_memory_region_set_flags);
   tcase_add_test(t, check_common_memory_region_set_location);

   tcase_add_test(t, check_common_address_space_init_region);
   tcase_add_test(t, check_common_address_space_get_region);
}
