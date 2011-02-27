
#include "physmem.h"
#include "strfuncs.h"

#include <stdlib.h>


struct physmem *
test_physmem_alloc(struct kernel *kernel, int num, int n_pages) {

  SLIST_INIT(&test_physmem.regionlist);

  while (num--) {
    uint32 *bitmap = (uint32 *)malloc(n_pages / 8);
    struct physmem_region *newregion = (struct physmem_region *)malloc(
        sizeof(struct physmem_region));
    memset(bitmap, 0, n_pages / 8);

    newregion->bitmap = bitmap;
    newregion->bitmap_length = (n_pages / (8 * sizeof(uint32)));
    newregion->free_pages = n_pages;
    newregion->used_pages = 0;
    newregion->start_address = 0x01000000 * (num + 1);

    SLIST_INSERT_HEAD(&test_physmem.regionlist, newregion, regions);

  }

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
