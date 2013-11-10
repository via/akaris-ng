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
extern int highstart;

static void i686_set_cr3(struct i686_pde *cr3) {
  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "m"(cr3));
}

/* Makes at the space from PHYS_KERN_START to limit identity mapped
 * Does nothing if the limit is lowered -- eventually can shrink,
 * but regardless returns the old limit, so pass 0 in to get current limit */
static virtaddr_t i686_brk(struct virtmem *_v, virtaddr_t newend) {

  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  virtaddr_t oldlimit = v->identitymap_limit;

  kernel()->debug("Called with %x, old end is %x\n", newend, v->identitymap_limit);

  long old_limit_page = ((long)v->identitymap_limit + (pg_size - 1)) / pg_size;
  long new_limit_page = ((long)newend + (pg_size - 1)) / pg_size;

  if (newend < v->identitymap_limit)
    return oldlimit;

  long curpage;
  kernel()->debug("Setting limit: %x -> %x\n", old_limit_page, new_limit_page);

  for (curpage = old_limit_page + 1; curpage <= new_limit_page; ++curpage) {
    long phys_page = curpage - ((physaddr_t)&highstart / pg_size);
    kernel_flatpt[phys_page].phys_addr = phys_page;
    kernel_flatpt[phys_page].present = 1;
    kernel_flatpt[phys_page].writable = 1;
  } 
  v->identitymap_limit = newend;
  
  return oldlimit;

}


struct virtmem *
i686_virtmem_init(struct kernel *k) {

  extern int ebss;
  i686_virtmem.kernel_pde_list = kernel_pd;
  i686_virtmem.virt.cpu = k->bsp;

  unsigned int cur_pde;
  for (cur_pde = n_start_pde; cur_pde < n_kernel_pde + n_start_pde;
      ++cur_pde) {
    kernel_pd[cur_pde].phys_addr = ((unsigned long)(kernel_pts[cur_pde - n_start_pde]) - (unsigned long)&highstart) >> 12;
    kernel_pd[cur_pde].present = 1;
    kernel_pd[cur_pde].writable = 1;
  }


  i686_virtmem.identitymap_limit = &highstart;
  i686_brk((struct virtmem*)&i686_virtmem, &ebss);

  i686_set_cr3((struct i686_pde *)((void *)kernel_pd - (void *)&highstart));

  /* thanks to loader, our basic paging is already set up */

  return &i686_virtmem.virt;

}

/* Finds an unused kernelspace virtual address (page). 
 * Once this is used brk is unusable and can result in unpredictable behaviour.
 */
static virtmem_error_t
i686_kernel_alloc(struct virtmem *v VAR_UNUSED, virtaddr_t *addr, 
                  unsigned int n_pages) {
  /* For now we can just scan the kernel_pt list for an unused page */
  /* TODO: this won't be doable by multipple processors without locking.
   * really a better solution is needed, this is temporary */

  unsigned long pgindex;
  struct i686_pte *pte;

  assert (n_pages == 1); /* No buddy allocator yet */

  for (pgindex = 0; pgindex < 1024 * n_kernel_pde; ++pgindex) {
    pte = &kernel_flatpt[pgindex];
    if (pte->present == 0) {
      pte->present = 1;
      *addr = pgindex * pg_size + (virtaddr_t)&highstart;
      return VIRTMEM_SUCCESS;
    }
  }

      /* Unused, so we can use it*/


  return VIRTMEM_OOM;
}

static virtmem_error_t
i686_kernel_free(struct virtmem *v VAR_UNUSED, virtaddr_t addr VAR_UNUSED) {

                
  return VIRTMEM_SUCCESS;
}

static struct i686_pte *
i686_virt_to_pte(struct i686_virtmem *v, virtaddr_t addr) {
  uint32 n_pte, n_pde;
  struct i686_pte *pte;

  uint32 pagenum = (uint32)addr / physmem_page_size(v->virt.cpu->localmem);
  n_pte = pagenum % 1024;
  n_pde = (pagenum / 1024) - n_start_pde;

  assert(n_pde < n_kernel_pde);
  pte = &((struct i686_pte *)(v->kernel_pde_list[n_pde].phys_addr << 12))[n_pte];

  return pte;
}


static virtmem_error_t
i686_kernel_virt_to_phys(struct virtmem *_v,
    struct physmem_page **p, virtaddr_t addr) {

  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  physaddr_t paddr;

  const struct i686_pte *pte = i686_virt_to_pte(v, addr);

  if (!pte->present)
    return VIRTMEM_NOTPRESENT;
  paddr = pte->phys_addr << 12; /*Convert to address */
  if (p)
    *p = physmem_phys_to_page(kernel()->phys, paddr);

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_kernel_map_virt_to_phys(struct virtmem *_v,
    physaddr_t p, virtaddr_t addr ) {

  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  struct i686_pte *pte = i686_virt_to_pte(v, addr);

  assert (pte->present = 1);
  if (!pte->present)
    return VIRTMEM_NOTPRESENT;  
  
  pte->phys_addr = p >> 12;
  return VIRTMEM_SUCCESS;
  /* TODO: invalidate page in tlb */
}

static void i686_pagewalk_init(struct i686_pagewalk_context *ctx) {
  ctx->pagestart = (virtaddr_t)0;

  virtaddr_t adr;
  virtmem_kernel_alloc(cpu()->kvirt, &adr, 1);
  ctx->pt = adr;
}


static void i686_pagewalk_done(struct i686_pagewalk_context *ctx) {
  virtmem_kernel_free(cpu()->kvirt, ctx->pt);
}

static struct i686_pte * 
i686_pagewalk(struct i686_pagewalk_context *ctx, struct i686_pde *physpd, virtaddr_t addr) {
  long ptsize = physmem_page_size(cpu()->localmem) * 1024;
  long offset = addr % ptsize;
  if (addr - offset != ctx->pagestart) {
    virtmem_kernel_map_virt_to_phys(cpu()->kvirt, (physaddr_t)physpd, ctx->pt);
    ctx->pagestart = addr - offset;
  }

  return ctx->pt[offset / physmem_page_size(cpu()->localmem)];

}

struct i686_virtmem i686_virtmem = {
  .virt = {
    .v = {
      .kernel_alloc = i686_kernel_alloc,
      .kernel_free = i686_kernel_free,
      .kernel_virt_to_phys = i686_kernel_virt_to_phys,
      .kernel_map_virt_to_phys = i686_kernel_map_virt_to_phys,
      .kernel_brk = i686_brk,

      .user_get_page = NULL,
      .user_map_page = NULL,
      .user_set_page_flags = NULL,
    },
  },
};
