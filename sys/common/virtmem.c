#include "virtual_memory.h"
#include "cpu.h"
#include "physical_memory.h"
#include "strfuncs.h"
#include "math.h"

/*
 * Copies from a pagesize window of userspace to kernelspace.
 * Thus will copy at most either a pagesize or the length provided, whichever is
 * lowest 
 */
static int
copy_partial_page_from_user(void *dst, const void *src, size_t len) {
  int pgsize = physmem_page_size(cpu()->localmem);
  int copylen = minl(len, pgsize - ((const unsigned long)src % pgsize));
  memcpy(dst, src, copylen);
  return copylen;
}

/*
 * Copies from kernelspace to a page-size window of userspace.
 */
static int
copy_partial_page_to_user(void *dst, const void *src, size_t len) {
  int pgsize = physmem_page_size(cpu()->localmem);
  int copylen = minl(len, pgsize - ((const unsigned long)dst % pgsize));
  memcpy(dst, src, copylen);
  return copylen;
}

/* Copy a region from userspace in a non-current context to kernelspace */
static virtmem_error_t
copy_alt_user_to_kernel(struct virtmem *v, void *dst,
    virtmem_md_context_t src_ctx, const void *src, size_t len) {
  virtaddr_t mapping;
  physaddr_t src_phys;
  virtmem_kernel_alloc(v, &mapping, 1);

  while (len > 0) {
    int amt_copied;
    virtmem_user_get_page(v, src_ctx, &src_phys, (const virtaddr_t)src);
    virtmem_kernel_map_virt_to_phys(v, src_phys, mapping);
    amt_copied = copy_partial_page_from_user(dst, src, len);

    len -= amt_copied;
    src += amt_copied;
    dst += amt_copied;
  }
    
  virtmem_kernel_free(v, mapping);
  return VIRTMEM_SUCCESS;
}

/* Copy a region from kernelspace to a non-current-context userspace */
static virtmem_error_t
copy_alt_user_from_kernel(struct virtmem *v, virtmem_md_context_t dst_ctx, 
    void *dst, const void *src, size_t len) {
  virtaddr_t mapping;
  physaddr_t dst_phys;
  virtmem_kernel_alloc(v, &mapping, 1);

  while (len > 0) {
    int amt_copied;
    virtmem_user_get_page(v, dst_ctx, &dst_phys, (const virtaddr_t)dst);
    virtmem_kernel_map_virt_to_phys(v, dst_phys, mapping);
    amt_copied = copy_partial_page_from_user(dst, src, len);

    len -= amt_copied;
    src += amt_copied;
    dst += amt_copied;
  }
    
  virtmem_kernel_free(v, mapping);
  return VIRTMEM_SUCCESS;
}


virtmem_error_t
common_virtmem_copy_user_to_kernel(struct virtmem *v, void *dst,
    virtmem_md_context_t src_ctx, const void *src, size_t len) {

  /* Determine if this is the current context for this processor*/

  /* If so, memcpy. Otherwise copy_alt_user_to_kernel */
  return copy_alt_user_to_kernel(v, dst, src_ctx, src, len);
}

virtmem_error_t
common_virtmem_copy_kernel_to_user(struct virtmem *v, 
    virtmem_md_context_t dst_ctx, void *dst, const void *src, size_t len) {

  /* Determine if this is the current context for this processor*/

  /* If so, memcpy. Otherwise copy_alt_user_from_kernel */
  return copy_alt_user_from_kernel(v, dst_ctx, dst, src, len);
  

  return VIRTMEM_SUCCESS;
}
