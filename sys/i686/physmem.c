#include "multiboot.h"
#include "types.h"
#include "bootvideo.h"
#include "kernel.h"
#include "strfuncs.h"
#include "physmem.h"

static physaddr_t i686_create_initial(struct kernel *k, multiboot_info_t *info, 
    physaddr_t kernel_end) {

  memory_map_t *entry = (memory_map_t *)info->mmap_addr;
  struct physmem_page *table = (struct physmem_page *)kernel_end;
  unsigned int total_pages = 0;
  unsigned int free_pages = 0;
  unsigned int pagesize = i686_physmem.v.page_size(&i686_physmem);

  k->debug("addr: %x  len: %d\n", info->mmap_addr, info->mmap_length);
  while (entry < (memory_map_t *)(info->mmap_addr + info->mmap_length)) {
    k->debug("Entry: start: 0x%x  len: 0x%x  type: 0x%x\n",
        entry->base_addr_low, entry->length_low, entry->type);

    if (entry->base_addr_high || entry->length_high) {
      k->debug("More than 32 bit memory areas not supported\n");
      while (1);
    }

    unsigned int startpage = ((entry->base_addr_low + (pagesize - 1)) / 
      pagesize);
    unsigned int lastpage = ((entry->base_addr_low + entry->length_low - 1)  
         / pagesize);

    if (lastpage < startpage)
      lastpage = startpage;


    if (entry->type == 1) { /* Usable */
      free_pages += (lastpage - startpage + 1);
      int curpage;

      for (curpage = startpage; curpage <= lastpage; ++curpage) {
        if (curpage * pagesize < 0x100000) { /*HACK, if <1MB, save for later*/
          LIST_INSERT_HEAD(&i686_physmem.bootlist, &table[curpage], pages);
        } else {
          LIST_INSERT_HEAD(&i686_physmem.freelist, &table[curpage], pages);
        }
      }
    } 

    total_pages += (lastpage - startpage + 1);
    i686_physmem.free_pages = free_pages;
    i686_physmem.total_pages = total_pages;

    entry++;
  }
  return kernel_end + total_pages * sizeof(struct physmem_page);

}


static uint32 i686_physmem_page_size(const struct physmem *p) {

  return PAGE_SIZE;
}

static uint32 i686_prune_memory(struct kernel *k, multiboot_info_t *info,
    physaddr_t newend) {

  unsigned int pagesize = i686_physmem.v.page_size(&i686_physmem);
  unsigned int start_kernel = 0x100000 / 0x1000;
  unsigned int end_kernel = (newend + (pagesize - 1)) / pagesize;
  unsigned int curpage;
  uint32 forcemarked = 0;

  for (curpage = start_kernel; curpage <= end_kernel; ++curpage) {
    struct physmem_page *page = i686_physmem.v.phys_to_page(&i686_physmem,
        curpage * pagesize);
    LIST_REMOVE(page, pages);
    forcemarked++;
  }

  return forcemarked;
}

struct physmem *
i686_physmem_alloc(struct kernel *kernel, multiboot_info_t *info) {

  extern const int ebss;
  physaddr_t k_end = ((physaddr_t)&ebss + 4096) / 4096 * 4096;
  uint32 pruned;

  LIST_INIT(&i686_physmem.freelist);
  LIST_INIT(&i686_physmem.bootlist);

  physaddr_t newend = i686_create_initial(kernel, info, k_end);
  
  /* Now we want to actually retroactively remove this area */
  pruned = i686_prune_memory(kernel, info, newend);
  kernel->debug("Pruned %d pages\n", pruned);




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
