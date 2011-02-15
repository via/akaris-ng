#ifndef I386_PHYSMEM_H
#define I386_PHYSMEM_H

#include "physical_memory.h"

#define PAGE_SIZE 4096
#define MAX_REGIONS 16

struct physmem *i686_physmem_alloc(multiboot_header_t *);

#endif
