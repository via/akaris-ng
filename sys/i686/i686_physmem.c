#include "multiboot.h"
#include "types.h"
#include "bootvideo.h"
#include "kernel.h"
#include "strfuncs.h"
#include "i686_physmem.h"

#include "cpu.h"
#include "virtual_memory.h"

extern const int highstart;

static void i686_create_initial(struct kernel *k, multiboot_info_t *info) {

  memory_map_t *entry = (memory_map_t *)info->mmap_addr;
  struct physmem_page *table = (struct physmem_page *)virtmem_brk(k->bsp->kvirt, 0);
  i686_physmem.start_address = (virtaddr_t)table;
  unsigned int total_pages = 0;
  unsigned int free_pages = 0;
  unsigned int pagesize = physmem_page_size(&i686_physmem.p);
  virtaddr_t position;

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
    unsigned int lastpage = (entry->base_addr_low + entry->length_low 
                  + (pagesize - 1)) / pagesize;

    if (lastpage < startpage)
      lastpage = startpage;


    if (entry->type == 1) { /* Usable */
      unsigned int curpage;

      position = (virtaddr_t)&table[lastpage + 1];
      virtmem_brk(k->bsp->kvirt, position);

      for (curpage = startpage; curpage <= lastpage; ++curpage) {
        if (curpage * pagesize < 0x100000) { /*HACK, if <1MB, save for later*/
          LIST_INSERT_HEAD(&i686_physmem.p.bootlist, &table[curpage], pages);
        } else {
          LIST_INSERT_HEAD(&i686_physmem.p.freelist, &table[curpage], pages);
          free_pages++;
        }
      }
    } 

    total_pages += (lastpage - startpage + 1);
    i686_physmem.p.free_pages = free_pages;
    i686_physmem.p.total_pages = total_pages;
    i686_physmem.last_address = position;

    entry++;
  }

}


static uint32 i686_physmem_page_size(const struct physmem *p VAR_UNUSED) {

  return PAGE_SIZE;
}

static uint32 i686_prune_memory(struct kernel *k,
    multiboot_info_t *info VAR_UNUSED) {    
  
  unsigned long  pagesize = physmem_page_size(&i686_physmem.p);
  unsigned long start_kernel = (long)&highstart / pagesize;
  unsigned long end_kernel = ((long)virtmem_brk(k->bsp->kvirt, 0) + (pagesize - 1)) / pagesize;
  k->debug("start: %x  end: %x\n", start_kernel, end_kernel);
  unsigned long curpage;
  uint32 forcemarked = 0;

  for (curpage = start_kernel; curpage <= end_kernel; ++curpage) {
    unsigned long physpage = curpage - ((unsigned long)&highstart/pagesize);
    struct physmem_page *page = physmem_phys_to_page(&i686_physmem.p,
        physpage * pagesize);
    LIST_REMOVE(page, pages);
    forcemarked++;
  }

  i686_physmem.p.free_pages -= forcemarked;
  return forcemarked;
}

struct physmem *
i686_physmem_alloc(struct kernel *kernel, multiboot_info_t *info) {

  extern const int ebss;
  uint32 pruned;

  LIST_INIT(&i686_physmem.p.freelist);
  LIST_INIT(&i686_physmem.p.bootlist);

  i686_create_initial(kernel, info);
  
  /* Now we want to actually retroactively remove this area */
  pruned = i686_prune_memory(kernel, info);
  kernel->debug("Pruned %d pages\n", pruned);

  i686_physmem.p.parent = kernel;

  return &i686_physmem.p;

}

static struct physmem_page * i686_physmem_phys_to_page(const struct physmem *_p, physaddr_t addr) {

  const struct i686physmem *p = (const struct i686physmem *)_p;

  struct physmem_page *pageindex = (struct physmem_page *)p->start_address;
  int index = (addr / physmem_page_size(_p));

  return &pageindex[index];
}

static physaddr_t i686_physmem_page_to_phys(const struct physmem *_p, 
    const struct physmem_page *page) {

  const struct i686physmem *p = (const struct i686physmem *)_p;

  int index = ((void *)page - (void *)p->start_address) / sizeof(struct physmem_page);

  return index * physmem_page_size(_p);


}

struct i686physmem i686_physmem = {
  .p = {
    .name = "i686physmem",
    .v = {
      i686_physmem_phys_to_page,
      i686_physmem_page_to_phys,
      common_physmem_page_alloc,
      common_physmem_page_free,
      common_physmem_stats_get,
      i686_physmem_page_size,
    },
  },
};
