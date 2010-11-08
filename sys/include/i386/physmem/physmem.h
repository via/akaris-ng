#ifndef I386_PHYSMEM_H
#define I386_PHYSMEM_H

#include <common/physical_memory.h>

#define PAGE_SIZE 4096

typedef struct physmem_memregion {

  physaddr_t bitmap_phys;
  uint32 * bitmap;

  uint32 total_pages;
  uint32 free_pages;
  physaddr_t startpos;

  uint8 node;

} physmem_memregion_t;





#endif
