#ifndef I386_PHYSMEM_H
#define I386_PHYSMEM_H

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

struct physmem_i686 {
  struct physmem phys;
  physmem_memregion regions[MAX_REGIONS];
};

#endif
