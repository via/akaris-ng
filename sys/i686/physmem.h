#ifndef I386_PHYSMEM_H
#define I386_PHYSMEM_H

#include "physical_memory.h"

#define PAGE_SIZE 4096
#define MAX_REGIONS 16

struct physmem_memregion {

  physaddr_t bitmap_phys;
  uint32 * bitmap;

  uint32 total_pages;
  uint32 free_pages;
  physaddr_t startpos;

  uint8 node;

};

struct i686_physmem {
  struct physmem phys;
  struct physmem_memregion regions[MAX_REGIONS];
};

struct i686_physmem *i686_physmem_alloc(multiboot_header_t *);

#endif
