/* test_vmem.c -- test implementation of virtual memory manager framework */

#include <stdlib.h>

#include "strfuncs.h"
#include "cpu.h"
#include "virtual_memory.h"
#include "physical_memory.h"
#include "test_vmem.h"


struct virtmem_vfuncs test_virtmem_vfuncs;

void
test_kernel_vmem_create(struct virtmem **_v, struct cpu *c, int n_pages) {

  struct test_virtmem *v;
  v = (struct test_virtmem *)malloc(sizeof(struct test_virtmem));
  memcpy(&v->v.v, &test_virtmem_vfuncs, sizeof(struct virtmem_vfuncs));
  v->n_pages = n_pages;
  v->v.cpu = c;
  v->pages = calloc(n_pages, sizeof(struct test_vmem_page));
  memset(v->pages, 0, n_pages * sizeof(struct test_vmem_page));

  *_v = &v->v;
}




static virtmem_error_t 
test_kernel_alloc(struct virtmem *_v, virtaddr_t *a VAR_UNUSED, unsigned int n_pages) {

  int i;
  struct test_virtmem *v = (struct test_virtmem *)_v;

  for (i = 0; i < v->n_pages; ++i) {
    if (v->pages[i].physaddr == NULL) {
      break;
    }
  }
  
  

  a = NULL; 
  return VIRTMEM_SUCCESS;
}


static virtmem_error_t
test_kernel_free(struct virtmem *v VAR_UNUSED, virtaddr_t addr VAR_UNUSED) {

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

/* All of this is currently unused until its done being implement */
struct virtmem_vfuncs test_virtmem_vfuncs = {
  .kernel_alloc = test_kernel_alloc,
  .kernel_free = test_kernel_free,
  .kernel_virt_to_phys = test_kernel_virt_to_phys,
  .kernel_map_virt_to_phys = test_kernel_map_virt_to_phys,
};

