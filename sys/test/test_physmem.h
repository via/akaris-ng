#ifndef TEST_PHYSMEM_H
#define TEST_PHYSMEM_H

#include "physical_memory.h"


extern struct test_physmem t_physmem;

struct physmem *test_physmem_alloc(struct kernel *, int n_pages);

struct test_physmem {
  struct physmem p;
  struct physmem_page *pagelist;
};

void check_initialize_physmem_tests(TCase *);


#endif
