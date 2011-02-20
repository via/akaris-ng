
#include "physmem.h"
#include "strfuncs.h"

uint32 region_bitmap[128];
struct physmem_region testregion;

struct physmem *
test_physmem_alloc(struct kernel *kernel) {

  memset(region_bitmap, 0, 128 * sizeof(uint32));

  testregion.bitmap = region_bitmap;
  testregion.bitmap_length = 128;
  testregion.free_pages = 128 * 32;
  testregion.used_pages = 0;
  testregion.start_address = 0x80000000;

  SLIST_INIT(&test_physmem.regionlist);
  SLIST_INSERT_HEAD(&test_physmem.regionlist, &testregion, regions);

  return &test_physmem;

}

static uint32 test_physmem_page_size(struct physmem *p) {
  return 4096;
}

struct physmem test_physmem = {
  .name = "testphysmem",
  .v = {
    physmem_page_alloc,
    physmem_page_free,
    physmem_stats_get,
    test_physmem_page_size,
    physmem_region_add,
  }
};
