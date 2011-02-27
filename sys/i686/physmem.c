#include "multiboot.h"
#include "types.h"
#include "bootvideo.h"
#include "kernel.h"
#include "strfuncs.h"
#include "physmem.h"

static physaddr_t i686_create_initial(multiboot_info_t *info, 
    physaddr_t kernel_end) {

  memory_map_t *entry = (memory_map_t *)info->mmap_addr;
  while (entry < (memory_map_t *)(info->mmap_addr + info->mmap_length)) {

    entry++;
  }
  return kernel_end;

}


static uint32 i686_physmem_page_size(struct physmem *p) {

  return PAGE_SIZE;
}

static physmem_error_t
i686_physmem_region_add(struct physmem *_phys, uint8 node,
    physaddr_t start, physaddr_t stop) {

  /*TODO: Not implemented */
  return PHYSMEM_ERR_INITCOND;
}

struct physmem *
i686_physmem_alloc(struct kernel *kernel, multiboot_info_t *info) {

  extern const int ebss;
  physaddr_t k_end = ((physaddr_t)&ebss + 4096) / 4096 * 4096;
  i686_create_initial(info, k_end);


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
