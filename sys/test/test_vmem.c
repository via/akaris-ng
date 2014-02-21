/* test_vmem.c -- test implementation of virtual memory manager framework */

#include <stdlib.h>
#include "check.h"
#include "queue.h"
#include "mock.h"

#include "cpu.h"
#include "virtual_memory.h"
#include "physical_memory.h"
#include "test_vmem.h"


static mock_call_list virtmem_mock_calls;
struct virtmem_vfuncs test_virtmem_vfuncs;

void
test_kernel_vmem_create(struct virtmem **_v, struct cpu *c, int n_pages) {

  mock_call_list_init(&virtmem_mock_calls);

  struct test_virtmem *v;
  int i;
  v = (struct test_virtmem *)malloc(sizeof(struct test_virtmem));
  memcpy(&v->v.v, &test_virtmem_vfuncs, sizeof(struct virtmem_vfuncs));
  v->n_pages = n_pages;
  v->v.cpu = c;
  v->pages = calloc(n_pages, sizeof(struct test_vmem_page));
  memset(v->pages, 0, n_pages * sizeof(struct test_vmem_page));
  for (i = 0; i < n_pages; ++i)
    v->pages->virtaddr = malloc(4096);

  *_v = &v->v;
}




static virtmem_error_t 
test_kernel_alloc(struct virtmem *_v, virtaddr_t *a, unsigned int n_pages) {

  int i;
  struct test_virtmem *v = (struct test_virtmem *)_v;

  mock_call(&virtmem_mock_calls, "test_kernel_alloc", MOCK_PTR(_v),
      MOCK_PTR(a), MOCK_LONG(n_pages));

  for (i = 0; i < v->n_pages; ++i) {
    if (v->pages[i].used == 0) {
      v->pages[i].used = 1;
      a = v->pages[i].virtaddr; 
      return VIRTMEM_SUCCESS;
    }
  }
  
  return VIRTMEM_OOM;
  

}


static virtmem_error_t
test_kernel_free(struct virtmem *_v, virtaddr_t addr) {
  mock_call(&virtmem_mock_calls, "test_kernel_free", MOCK_PTR(_v),
      MOCK_PTR(addr));

  struct test_virtmem *v = (struct test_virtmem *)_v;
  int i;
  for (i = 0; i < v->n_pages; ++i) {
    if (v->pages[i].virtaddr == addr) {
      v->pages[i].physaddr = 0;
      v->pages[i].used = 0;
      return VIRTMEM_SUCCESS;  
    }
  }
  return VIRTMEM_NOTPRESENT;

}

static virtmem_error_t
test_kernel_virt_to_phys(struct virtmem *_v, physaddr_t *p, 
    virtaddr_t addr) {
  mock_call(&virtmem_mock_calls, "test_kernel_virt_to_phys", MOCK_PTR(_v),
      MOCK_PTR(addr));

  struct test_virtmem *v = (struct test_virtmem *)_v;
  int i;
  for (i = 0; i < v->n_pages; ++i) {
    if (v->pages[i].virtaddr == addr) {
      *p = v->pages[i].physaddr;
      return VIRTMEM_SUCCESS;  
    }
  }
  return VIRTMEM_NOTPRESENT;
}

static virtmem_error_t
test_kernel_map_virt_to_phys(struct virtmem *_v, physaddr_t p,
    virtaddr_t addr) {
  mock_call(&virtmem_mock_calls, "test_kernel_map_virt_to_phys", MOCK_PTR(_v),
      MOCK_LONG(p), MOCK_PTR(addr));
  struct test_virtmem *v = (struct test_virtmem *)_v;
  int i;
  for (i = 0; i < v->n_pages; ++i) {
    if (v->pages[i].virtaddr == addr) {
      v->pages[i].physaddr = p;
      return VIRTMEM_SUCCESS;  
    }
  }
  return VIRTMEM_NOTPRESENT;
}

START_TEST (check_common_virtmem_copy_user_to_kernel) {

} END_TEST

START_TEST (check_common_virtmem_copy_kernel_to_user) {

} END_TEST

/* All of this is currently unused until its done being implement */
struct virtmem_vfuncs test_virtmem_vfuncs = {
  .kernel_alloc = test_kernel_alloc,
  .kernel_free = test_kernel_free,
  .kernel_virt_to_phys = test_kernel_virt_to_phys,
  .kernel_map_virt_to_phys = test_kernel_map_virt_to_phys,
  .copy_user_to_kernel = common_virtmem_copy_user_to_kernel,
  .copy_kernel_to_user = common_virtmem_copy_kernel_to_user,
};


void check_initialize_virtmem_tests(TCase *t) {
  tcase_add_test(t, check_common_virtmem_copy_user_to_kernel);
  tcase_add_test(t, check_common_virtmem_copy_kernel_to_user);
}
