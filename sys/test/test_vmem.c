/* test_vmem.c -- test implementation of virtual memory manager framework */

#include "virtual_memory.h"


static virtmem_error_t 
test_kernel_alloc(struct virtmem *v, virtaddr_t *a, unsigned int n_pages) {
  

  a = NULL; 
  return VIRTMEM_SUCCESS;
}


static virtmem_error_t
test_kernel_free(struct virtmem *v, virtaddr_t addr) {

  return VIRTMEM_SUCCESS;  
}

static virtmem_error_t
test_kernel_virt_to_phys(struct virtmem *v, struct physmem_page **p, 
    virtaddr_t addr) {

  *p = NULL;
  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
test_kernel_map_virt_to_phys(struct virtmem *v, struct physmem_page **p,
    virtaddr_t addr) {

  *p = NULL;
  return VIRTMEM_SUCCESS;
}

struct virtmem test_virtmem = {
  .v = {
    .kernel_alloc = test_kernel_alloc,
    .kernel_free = test_kernel_free,
    .kernel_virt_to_phys = test_kernel_virt_to_phys,
    .kernel_map_virt_to_phys = test_kernel_map_virt_to_phys,
  },
};
