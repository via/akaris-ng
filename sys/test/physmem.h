#ifndef TEST_PHYSMEM_H
#define TEST_PHYSMEM_H

#include "physical_memory.h"


extern struct physmem test_physmem;

struct physmem *test_physmem_alloc(struct kernel *, int num, int n_pages);


#endif
