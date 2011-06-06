#include "kernel.h"
#include "physmem.h"

#include <stdio.h>

int
main() {
  struct kernel test_kernel;
  test_kernel.phys = test_physmem_alloc(&test_kernel, 1024);
  printf("Hosted: Startup!\n");

  physaddr_t addr, old;
  physmem_error_t status = PHYSMEM_SUCCESS;
  int num_pages = 0;
  while (status == PHYSMEM_SUCCESS) {
    num_pages++;
    status = test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &addr);
    printf(":  Page allocated at address: %x\n", addr);
  }
  printf("Num allocated pages: %d\n", num_pages - 1);


  return 0;
}
