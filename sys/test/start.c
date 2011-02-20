#include "kernel.h"
#include "physmem.h"

#include <stdio.h>

int
main() {
  struct kernel test_kernel;
  test_kernel.phys = test_physmem_alloc(&test_kernel);
  printf("Hosted: Startup!\n");

  physaddr_t addr;
  test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &addr);
  printf("Allocated: %x\n", addr);
  test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &addr);
  printf("Allocated: %x\n", addr);

  return 0;
}
