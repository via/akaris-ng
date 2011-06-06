#ifndef I686_PHYSMEM_H
#define I686_PHYSMEM_H

#include "physical_memory.h"

#define PAGE_SIZE 4096
#define MAX_REGIONS 16

extern struct i686physmem i686_physmem;

struct physmem *i686_physmem_alloc(struct kernel *, multiboot_info_t *);

struct i686physmem {
  struct physmem p;
  physaddr_t start_address;
  physaddr_t last_address;
};

#endif
