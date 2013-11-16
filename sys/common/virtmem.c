#include "virtual_memory.h"
#include "cpu.h"
#include "physical_memory.h"
#include "strfuncs.h"
#include "math.h"


virtmem_error_t
common_virtmem_copy_user_to_kernel(struct virtmem *v, void *dst,
    virtmem_md_context_t src_ctx, const void *src, size_t len) {
  void *d;
  const void *s;
  virtaddr_t curpage, newcurpage;
  virtaddr_t mapping;
  physaddr_t src_phys;
  int pgsize;
  
  virtmem_kernel_alloc(v, &mapping, 1);
  pgsize = physmem_page_size(cpu()->localmem);
  curpage = NULL;
  for (d = dst, s = src; len > 0;) {
    newcurpage = (virtaddr_t)((unsigned long)s % pgsize);
    if (newcurpage != curpage) {
      /* Map next page */
      virtmem_user_get_page(v, src_ctx, &src_phys, (const virtaddr_t)s);
      virtmem_kernel_map_virt_to_phys(v, src_phys, mapping);
      curpage = newcurpage;
    }
    /* Copy what we can */
    size_t l = minl(len, pgsize - (unsigned long)curpage);
    memcpy(d, mapping + (s - curpage), l);
    s += l;
    d += l;
    len -= l;
  }

  virtmem_kernel_free(v, mapping);
  return VIRTMEM_SUCCESS;
}

virtmem_error_t
common_virtmem_copy_kernel_to_user(struct virtmem *v, 
    virtmem_md_context_t dst_ctx, void *dst, const void *src, size_t len) {
  

  return VIRTMEM_SUCCESS;
}
