#include "kernel.h"
#include "physmem.h"

#include <stdio.h>

int
main() {
  struct kernel test_kernel;
  test_kernel.phys = test_physmem_alloc(&test_kernel, 1024);

  struct feeder_physmem test_feeder;

  printf("Hosted: Startup!\n");

  feeder_physmem_create(&test_feeder, test_kernel.phys, 100, 512);

  physaddr_t addr, old;
  physmem_error_t status = PHYSMEM_SUCCESS;
  int num_pages = 0;
  while (status == PHYSMEM_SUCCESS) {
    num_pages++;
/*    status = test_kernel.phys->v.page_alloc(test_kernel.phys, 0, &addr);*/
    status = test_feeder.p.v.page_alloc(&test_feeder.p, 0, &addr);
    printf(":  Page allocated at address: %x\n", addr);
  }
  printf("Num allocated pages: %d\n", num_pages - 1);


  return 0;
}
