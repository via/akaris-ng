#ifndef I686_PHYSMEM_H
#define I686_PHYSMEM_H

#include "multiboot.h"
#include "kernel.h"
#include "physical_memory.h"
#include "virtual_memory.h"

#define PAGE_SIZE 4096
#define MAX_REGIONS 16

#define PHYS_KERN_START 0x100000

struct physmem *i686_physmem_alloc(struct kernel *, const multiboot_info_t *);

struct i686physmem {
  struct physmem p;
  virtaddr_t start_address;
  virtaddr_t last_address;
};

extern struct i686physmem i686_physmem;

#endif
