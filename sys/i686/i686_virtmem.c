#include "kernel.h"
#include "virtual_memory.h"
#include "cpu.h"
#include "i686_virtmem.h"

#include "types.h"
#include "strfuncs.h"
#include "assert.h"
struct i686_virtmem i686_virtmem;

static const unsigned int n_kernel_pde = 256;
static const unsigned int n_start_pde = 256;
struct i686_pde kernel_pd[1024] __attribute__((aligned));
struct i686_pte kernel_pts[256][1024] __attribute__((aligned(4096)));

static void i686_set_cr3(struct i686_pde *cr3) {
  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "m"(cr3));
}


struct virtmem *
i686_virtmem_init(struct kernel *k) {
  physmem_error_t perr;
  physaddr_t paddr;
  unsigned int i;

  assert(k->phys != NULL);
  /* Allocate page for the pde list */
  memset((void*)kernel_pd, 0, physmem_page_size(k->phys));
  i686_virtmem.kernel_pde_list = kernel_pd;
  for (i = n_start_pde; i < n_start_pde + n_kernel_pde; ++i) {
    memset(kernel_pts[i - n_start_pde], 0, physmem_page_size(k->phys));
    i686_virtmem.kernel_pde_list[i].phys_addr = (physaddr_t)kernel_pts[i - n_start_pde] >> 10; /*Page count */
    i686_virtmem.kernel_pde_list[i].global = 1;
    i686_virtmem.kernel_pde_list[i].writable = 1;
    i686_virtmem.kernel_pde_list[i].present = 1;
  }

  /* identity-map the first 4mb, just like the loader */
  for (i = 0; i < 1024; ++i) {
    kernel_pts[0][i].phys_addr = i;
    kernel_pts[0][i].global = 1;
    kernel_pts[0][i].writable = 1;
    kernel_pts[0][i].present = 1;
  }

  i686_virtmem.virt.cpu = k->bsp;

  i686_set_cr3(kernel_pd);

  return &i686_virtmem.virt;

}


static virtmem_error_t
i686_kernel_alloc(struct virtmem *v VAR_UNUSED, virtaddr_t *addr VAR_UNUSED, 
                  unsigned int n_pages VAR_UNUSED) {

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_kernel_free(struct virtmem *v VAR_UNUSED, virtaddr_t addr VAR_UNUSED) {

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
i686_kernel_map_virt_to_phys(struct virtmem *v VAR_UNUSED,
    struct physmem_page **p VAR_UNUSED, virtaddr_t addr VAR_UNUSED) {
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
