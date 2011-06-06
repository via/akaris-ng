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


static uint32 i686_physmem_page_size(const struct physmem *p) {

  return PAGE_SIZE;
}


struct physmem *
i686_physmem_alloc(struct kernel *kernel, multiboot_info_t *info) {

  extern const int ebss;
  physaddr_t k_end = ((physaddr_t)&ebss + 4096) / 4096 * 4096;
  i686_create_initial(kernel, info, k_end);


  return &i686_physmem;

}

static struct physmem_page * i686_physmem_phys_to_page(const struct physmem *_p, physaddr_t addr) {

  extern const int ebss;
  physaddr_t k_end = ((physaddr_t)&ebss + 4096) / 4096 * 4096;
  struct physmem_page *pageindex = (struct physmem_page *)k_end;
  int index = (addr / _p->v.page_size(_p));

  return &pageindex[index];
}

static physaddr_t i686_physmem_page_to_phys(const struct physmem *_p, 
    const struct physmem_page *page) {

  extern const int ebss;
  physaddr_t k_end = ((physaddr_t)&ebss + 4096) / 4096 * 4096;

  int index = ((void *)page - (void *)k_end) / sizeof(struct physmem_page);

  return index * _p->v.page_size(_p);


}

struct physmem i686_physmem = {
  .name = "i686physmem",
  .v = {
    i686_physmem_phys_to_page,
    i686_physmem_page_to_phys,
    physmem_page_alloc,
    physmem_page_free,
    physmem_stats_get,
    i686_physmem_page_size,
  },
};
