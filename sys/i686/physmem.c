#include "multiboot.h"
#include "types.h"
#include "bootvideo.h"
#include "kernel.h"
#include "strfuncs.h"
#include "physmem.h"

static physaddr_t i686_create_initial(struct kernel *k, multiboot_info_t *info, 
    physaddr_t kernel_end) {

  memory_map_t *entry = (memory_map_t *)info->mmap_addr;
  k->debug("addr: %x  len: %d\n", info->mmap_addr, info->mmap_length);
  while (entry < (memory_map_t *)(info->mmap_addr + info->mmap_length)) {
    k->debug("Entry: start: 0x%x  len: 0x%x  type: 0x%x\n",
        entry->base_addr_low, entry->length_low, entry->type);


    entry++;
  }
  return kernel_end;

}


static uint32 i686_physmem_page_size(struct physmem *p) {

  return PAGE_SIZE;
}


struct physmem *
i686_physmem_alloc(struct kernel *kernel, multiboot_info_t *info) {

  extern const int ebss;
  physaddr_t k_end = ((physaddr_t)&ebss + 4096) / 4096 * 4096;
  i686_create_initial(kernel, info, k_end);


  return &i686_physmem;

}

struct physmem i686_physmem = {
  .name = "i686physmem",
  .v = {
    physmem_phys_to_page,
    physmem_page_to_phys,
    physmem_page_alloc,
    physmem_page_free,
    physmem_stats_get,
    i686_physmem_page_size,
    i686_physmem_region_add,
  },
};
