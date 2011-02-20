#include "multiboot.h"
#include "kernel.h"
#include "physmem.h"

static uint32 i686_physmem_page_size(struct physmem *p) {

  return PAGE_SIZE;
}

static physmem_error_t
i686_physmem_region_add(struct physmem *_phys, uint8 node,
    physaddr_t start, physaddr_t stop) {

  struct i686_physmem * phys __attribute__((unused)) = (struct i686_physmem *)_phys;

  return PHYSMEM_SUCCESS;
}

struct physmem *
i686_physmem_alloc(struct kernel *kernel, multiboot_header_t *info) {

  return &i686_physmem;

}

struct physmem i686_physmem = {
  .name = "i686physmem",
  .v = {
    physmem_page_alloc,
    physmem_page_free,
    physmem_stats_get,
    i686_physmem_page_size,
    i686_physmem_region_add,
  },
};
