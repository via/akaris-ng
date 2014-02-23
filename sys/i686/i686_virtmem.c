#include "kernel.h"
#include "virtual_memory.h"
#include "cpu.h"
#include "i686_virtmem.h"

#include "types.h"
#include "strfuncs.h"
#include "assert.h"
#include "math.h"
struct i686_virtmem i686_virtmem;

static const unsigned int n_kernel_pde = 256;
static const unsigned int n_start_pde = 768;
static const unsigned long pg_size = 4096;
struct i686_pde kernel_pd[1024] __attribute__((aligned));
struct i686_pte kernel_pts[256][1024] __attribute__((aligned(4096)));
struct i686_pte *kernel_flatpt = (struct i686_pte *)kernel_pts;
extern int highstart;

static void i686_invalidate_page(virtaddr_t a) {
  __asm__("invlpg (%0)" :: "r"(a) : "memory");
}

static void i686_set_pte(struct i686_pte *p, physaddr_t addr, int flags) {

    p->phys_addr = addr >> 12;
    p->present = bittestl(flags, I686_PAGE_PRESENT);
    p->writable = bittestl(flags, I686_PAGE_WRITABLE);
    p->write_through = bittestl(flags, I686_PAGE_WRITETHROUGH);
    p->cache_disable = bittestl(flags, I686_PAGE_CACHEDISABLE);
    p->user = bittestl(flags, I686_PAGE_USER);
    p->accessed = 0;
    p->dirty = 0;
    p->zero = 0;
    p->global = bittestl(flags, I686_PAGE_GLOBAL);
    p->avail = 0;
}

static void i686_set_pde(struct i686_pde *p, physaddr_t loc, int flags) {

    p->phys_addr = loc >> 12;
    p->present = bittestl(flags, I686_PAGE_PRESENT);
    p->writable = bittestl(flags, I686_PAGE_WRITABLE);
    p->write_through = bittestl(flags, I686_PAGE_WRITETHROUGH);
    p->cache_disable = bittestl(flags, I686_PAGE_CACHEDISABLE);
    p->user = bittestl(flags, I686_PAGE_USER);
    p->accessed = 0;
    p->size = 0;
    p->zero = 0;
    p->global = bittestl(flags, I686_PAGE_GLOBAL);
    p->avail = 0;
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
    physaddr_t phys_addr = (pg_size * curpage) - (physaddr_t)&highstart;
    long phys_page = phys_addr / pg_size;
    i686_set_pte(&kernel_flatpt[phys_page], phys_addr, 
        I686_PAGE_PRESENT | I686_PAGE_WRITABLE);
    i686_invalidate_page((virtaddr_t)(curpage * pg_size));
  } 
  v->identitymap_limit = newend;
  
  return oldlimit;

}

static void
i686_set_kernel_pdes(struct i686_pde *pd) {
  unsigned int cur_pde;
  for (cur_pde = n_start_pde; cur_pde < n_kernel_pde + n_start_pde;
      ++cur_pde) {
    i686_set_pde(&pd[cur_pde], ((unsigned long)(kernel_pts[cur_pde - 
          n_start_pde]) - (unsigned long)&highstart),
          I686_PAGE_PRESENT | I686_PAGE_WRITABLE);
  }
}

static virtmem_error_t i686_get_context(struct virtmem *v VAR_UNUSED,
    virtmem_md_context_t *pd) {
  
  __asm__("movl %%cr3, %%eax\n"
          "movl %%eax, %0" :: "m"(pd));
  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_set_context(struct virtmem *v VAR_UNUSED, virtmem_md_context_t pd) {
  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "m"((struct i686_pde *)pd));
  return VIRTMEM_SUCCESS;
}

struct virtmem *
i686_virtmem_init(struct kernel *k) {

  extern int ebss;
  i686_virtmem.kernel_pde_list = &kernel_pd[n_start_pde];
  i686_virtmem.virt.cpu = k->bsp;

  i686_set_kernel_pdes(kernel_pd);

  i686_virtmem.identitymap_limit = &highstart;
  i686_brk((struct virtmem*)&i686_virtmem, &ebss);

  i686_set_context(&i686_virtmem.virt, (virtmem_md_context_t)
      ((void *)kernel_pd - (void *)&highstart));

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
      pte->writable = 1;
      *addr = pgindex * pg_size + (virtaddr_t)&highstart;
      i686_invalidate_page(*addr);
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
  uint32 n_pde;
  struct i686_pte *pte;

  uint32 pagenum = (uint32)addr / physmem_page_size(v->virt.cpu->localmem);
  n_pde = (pagenum / 1024) - n_start_pde;

  assert(n_pde < n_kernel_pde);
  pte = &kernel_flatpt[pagenum - (n_start_pde*1024)];

  return pte;
}


static virtmem_error_t
i686_kernel_virt_to_phys(struct virtmem *_v,
    physaddr_t *p, virtaddr_t addr) {

  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  physaddr_t paddr;

  const struct i686_pte *pte = i686_virt_to_pte(v, addr);

  if (!pte->present)
    return VIRTMEM_NOTPRESENT;
  paddr = pte->phys_addr << 12; /*Convert to address */
  if (p)
    *p = paddr;

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t
i686_kernel_map_virt_to_phys(struct virtmem *_v,
    physaddr_t p, virtaddr_t addr ) {

  struct i686_virtmem *v = (struct i686_virtmem *)_v;
  struct i686_pte *pte = i686_virt_to_pte(v, addr);

  assert (pte->present == 1);
  if (!pte->present)
    return VIRTMEM_NOTPRESENT;  
  
  i686_set_pte(pte, p, I686_PAGE_WRITABLE | I686_PAGE_PRESENT);
  i686_invalidate_page(addr);
  return VIRTMEM_SUCCESS;
  /* TODO: invalidate page in tlb */
}

static int i686_initialize_pde(struct i686_pde *pde) {
  physaddr_t pde_phys;
  virtaddr_t vaddr;

  if (physmem_page_alloc(cpu()->localmem, 0, &pde_phys) != PHYSMEM_SUCCESS)
    return -1;

  /* Wipe out pt before using */
  if (virtmem_kernel_alloc(cpu()->kvirt, &vaddr, 1) != VIRTMEM_SUCCESS)
    return -1;
  virtmem_kernel_map_virt_to_phys(cpu()->kvirt, pde_phys, vaddr);
  memset(vaddr, 0, physmem_page_size(cpu()->localmem));
  virtmem_kernel_free(cpu()->kvirt, vaddr);

  i686_set_pde(pde,  pde_phys, I686_PAGE_PRESENT | I686_PAGE_WRITABLE |
      I686_PAGE_USER);

  return 0;
}

static int i686_pagewalk_init(struct i686_pagewalk_context *ctx, 
    struct i686_pde *physpd) {
  virtaddr_t adr;

  ctx->pagestart = 0;

  if (virtmem_kernel_alloc(cpu()->kvirt, &adr, 1) != VIRTMEM_SUCCESS)
    return -1;
  ctx->pt = adr;

  if (virtmem_kernel_alloc(cpu()->kvirt, &adr, 1) != VIRTMEM_SUCCESS) {
    virtmem_kernel_free(cpu()->kvirt, adr);
    return -1;
  }
  ctx->pd = adr;

  virtmem_kernel_map_virt_to_phys(cpu()->kvirt, (physaddr_t)physpd, ctx->pd);
  return 0;
}

static void i686_pagewalk_done(struct i686_pagewalk_context *ctx) {
  virtmem_kernel_free(cpu()->kvirt, ctx->pt);
  virtmem_kernel_free(cpu()->kvirt, ctx->pd);
}

static struct i686_pte * 
i686_pagewalk(struct i686_pagewalk_context *ctx, virtaddr_t addr) {
  long pgsize = physmem_page_size(cpu()->localmem);
  long ptsize = pgsize * 1024;
  long pdindex = (unsigned long)addr / ptsize;
  long ptindex = ((unsigned long)addr % ptsize) / pgsize;
  
  if (ctx->pd[pdindex].present == 0) {
    /* PDE doesn't exist, create one */
    i686_initialize_pde(&ctx->pd[pdindex]);
  }

  if (pdindex * ptsize + ptindex != ctx->pagestart) {
    physaddr_t ptaddr = ctx->pd[pdindex].phys_addr * pgsize;
    virtmem_kernel_map_virt_to_phys(cpu()->kvirt, ptaddr, ctx->pt);
    ctx->pagestart = pdindex * ptsize + ptindex;
  }

  return &ctx->pt[ptindex];

}

static virtmem_error_t
i686_setup_kernelspace(virtmem_md_context_t ctx) {

  struct i686_pde *pd = (struct i686_pde *)ctx;
  struct i686_pagewalk_context pgw;

  i686_pagewalk_init(&pgw, pd);
  i686_set_kernel_pdes(pgw.pd);
  i686_pagewalk_done(&pgw);

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t 
i686_user_get_page(struct virtmem *v VAR_UNUSED, virtmem_md_context_t c,
    physaddr_t *p, const virtaddr_t vaddr) {
  struct i686_pagewalk_context ctx;
  struct i686_pte *l;

  if (i686_pagewalk_init(&ctx, c) < 0)
    return VIRTMEM_OOM;

  l = i686_pagewalk(&ctx, vaddr);
  *p = l->phys_addr * physmem_page_size(cpu()->localmem);
  i686_pagewalk_done(&ctx);

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t 
i686_user_map_page(struct virtmem *v VAR_UNUSED, virtmem_md_context_t c,
    virtaddr_t vaddr, physaddr_t p) {
  struct i686_pagewalk_context ctx;
  struct i686_pte *l;

  if (i686_pagewalk_init(&ctx, c) < 0)
    return VIRTMEM_OOM;

  l = i686_pagewalk(&ctx, vaddr);
  l->phys_addr = p / physmem_page_size(cpu()->localmem);
  i686_pagewalk_done(&ctx);

  i686_invalidate_page(vaddr);

  return VIRTMEM_SUCCESS;
}

static virtmem_error_t 
i686_user_set_page_flags(struct virtmem *v VAR_UNUSED, virtmem_md_context_t c,
    virtaddr_t vaddr, int flags) {
  struct i686_pagewalk_context ctx;
  struct i686_pte *l;

  i686_pagewalk_init(&ctx, c);
  l = i686_pagewalk(&ctx, vaddr);
  l->writable = bittestl(flags, VIRTMEM_PAGE_WRITABLE);
  l->user = bittestl(flags, VIRTMEM_PAGE_READABLE);
  l->present = 1;
  /* Currently nothing to prevent execution of nonexecute pages for i686 */
  i686_pagewalk_done(&ctx);
  i686_invalidate_page(vaddr);

  return VIRTMEM_SUCCESS;
}


struct i686_virtmem i686_virtmem = {
  .virt = {
    .v = {
      .kernel_alloc = i686_kernel_alloc,
      .kernel_free = i686_kernel_free,
      .kernel_virt_to_phys = i686_kernel_virt_to_phys,
      .kernel_map_virt_to_phys = i686_kernel_map_virt_to_phys,
      .kernel_brk = i686_brk,

      .user_get_page = i686_user_get_page,
      .user_map_page = i686_user_map_page,
      .user_set_page_flags = i686_user_set_page_flags,
      .user_setup_kernelspace = i686_setup_kernelspace,

      .get_context = i686_get_context,
      .set_context = i686_set_context,

      .copy_user_to_kernel = common_virtmem_copy_user_to_kernel,
      .copy_kernel_to_user = common_virtmem_copy_kernel_to_user

    },
  },
};
