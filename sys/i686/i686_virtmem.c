#include "kernel.h"
#include "virtual_memory.h"
#include "cpu.h"
#include "i686_virtmem.h"

#include "types.h"
#include "strfuncs.h"
#include "assert.h"
struct i686_virtmem i686_virtmem;

static const int n_kernel_pde = 256;

struct virtmem *
i686_virtmem_init(struct kernel *k) {
  physmem_error_t perr;
  physaddr_t paddr;
  int i;

  assert(k->phys != NULL);
  /* Allocate page for the pde list */
  perr = physmem_page_alloc(k->phys, 0, &paddr);
  assert(perr == PHYSMEM_SUCCESS);
  memset((void*)paddr, 0, physmem_page_size(k->phys));
  i686_virtmem.kernel_pde_list = (struct i686_pde *)paddr;
  for (i = 0; i < n_kernel_pde; ++i) {
    perr = physmem_page_alloc(k->phys, 0, &paddr);
    assert(perr == PHYSMEM_SUCCESS);
    memset((void*)paddr, 0, physmem_page_size(k->phys));
    i686_virtmem.kernel_pde_list[i].phys_addr = paddr >> 10; /*Page count */
    i686_virtmem.kernel_pde_list[i].global = 1;
    i686_virtmem.kernel_pde_list[i].writable = 1;
    i686_virtmem.kernel_pde_list[i].present = 1;
  }

  return &i686_virtmem.virt;

}


static virtmem_error_t
i686_kernel_alloc(struct virtmem *v, virtaddr_t *addr, unsigned int n_pages) {

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_kernel_free(struct virtmem *v, virtaddr_t addr) {

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_kernel_virt_to_phys(struct virtmem *_v,
    struct physmem_page **p, virtaddr_t addr) {

  uint32 n_pte, n_pde;
  struct i686_pte *pte;
  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  physaddr_t paddr;

  uint32 pagenum = (uint32)addr / physmem_page_size(_v->cpu->localmem);
  n_pte = pagenum % 1024;
  n_pde = pagenum / 1024;

  assert(n_pde < n_kernel_pde);
  pte = &((struct i686_pte *)(v->kernel_pde_list[n_pde].phys_addr << 10))[n_pte];
  if (!pte->present)
    return VIRTMEM_NOTPRESENT;
  paddr = pte->phys_addr << 10; /*Convert to address */
  *p = physmem_phys_to_page(_v->cpu->localmem, paddr);

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_kernel_map_virt_to_phys(struct virtmem *v,
    struct physmem_page **p, virtaddr_t addr) {
  return VIRTMEM_SUCCESS;
}

struct i686_virtmem i686_virtmem = {
  .virt = {
    .v = {
      .kernel_alloc = i686_kernel_alloc,
      .kernel_free = i686_kernel_free,
      .kernel_virt_to_phys = i686_kernel_virt_to_phys,
      .kernel_map_virt_to_phys = i686_kernel_map_virt_to_phys,
    },
  },
};
