#include "kernel.h"
#include "virtual_memory.h"
#include "cpu.h"
#include "i686_virtmem.h"

#include "types.h"
#include "strfuncs.h"
#include "assert.h"
struct i686_virtmem i686_virtmem;

static const unsigned int n_kernel_pde = 256;
static const unsigned int n_start_pde = 768;
static const unsigned long pg_size = 4096;
struct i686_pde kernel_pd[1024] __attribute__((aligned));
struct i686_pte kernel_pts[256][1024] __attribute__((aligned(4096)));
struct i686_pte *kernel_flatpt = (struct i686_pte *)kernel_pts;
extern const int highstart;

static void i686_set_cr3(struct i686_pde *cr3) {
  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "m"(cr3));
}

/* Makes at the space from PHYS_KERN_START to limit identity mapped
 * Does nothing if the limit is lowered -- eventually can shrink */
static virtaddr_t i686_sbrk(struct virtmem *_v, offset_t amt) {

  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  virtaddr_t oldlimit;

  _v->cpu->k->debug("Called with %x, old end is %x\n", amt, v->identitymap_limit);

  /* Round up */
  amt = (amt + 3) / 4 * 4;

  long old_limit_page = ((long)v->identitymap_limit + (pg_size - 1)) / pg_size;
  long new_limit_page = ((long)v->identitymap_limit + amt + (pg_size - 1)) / pg_size;
  long curpage;
  _v->cpu->k->debug("Setting limit: %x -> %x\n", old_limit_page, new_limit_page);

  for (curpage = old_limit_page + 1; curpage <= new_limit_page; ++curpage) {
    long phys_page = curpage - ((physaddr_t)&highstart / pg_size);
    kernel_flatpt[curpage].phys_addr = phys_page;
    kernel_flatpt[curpage].present = 1;
    kernel_flatpt[curpage].writable = 1;
  } 
  oldlimit = v->identitymap_limit;
  v->identitymap_limit += amt;
  
  return oldlimit;

}


struct virtmem *
i686_virtmem_init(struct kernel *k) {

  extern const int ebss;
  i686_virtmem.kernel_pde_list = kernel_pd;
  i686_virtmem.virt.cpu = k->bsp;

  unsigned int cur_pde;
  for (cur_pde = n_start_pde; cur_pde < n_kernel_pde + n_start_pde;
      ++cur_pde) {
    kernel_pd[cur_pde].phys_addr = ((unsigned long)(kernel_pts[cur_pde - n_start_pde]) - (unsigned long)&highstart) >> 12;
    kernel_pd[cur_pde].present = 1;
    kernel_pd[cur_pde].writable = 1;
  }


  i686_virtmem.identitymap_limit = (virtaddr_t)((((long)&ebss) + 3) / 4 * 4);

  /* thanks to loader, our basic paging is already set up */

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
      .kernel_sbrk = i686_sbrk,
    },
  },
};
