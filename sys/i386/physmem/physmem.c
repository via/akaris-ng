/* physmem.c -- low level physical page allocation for x86 (i386) 
 * 
 * Implements the functions in physical_memory.h
 */

#include "physical_memory.h"
#include "physmem.h"
#include "assert.h"

#ifdef HOSTED
#include <stdio.h>
#endif

/* Static variables for initial page allocation */
static physmem_memregion_t phys_regions[MAX_PHYS_REGIONS];
static uint32 init_bitmap[PAGE_SIZE / sizeof (uint32)];

uint32 
physmem_page_size () {
  return PAGE_SIZE;
}

physmem_error_t 
physmem_initialize () {

  uint32 curregion;
  
  for (curregion = 0; curregion < MAX_PHYS_REGIONS; ++curregion) {
    phys_regions[curregion].bitmap_phys = 0;
    phys_regions[curregion].bitmap = init_bitmap;
    phys_regions[curregion].total_pages = phys_regions[curregion].free_pages = 0;
    phys_regions[curregion].node = 0;
  }

#ifdef HOSTED
  printf ("Physmem Initialized\n");
#endif
  
  return PHYSMEM_SUCCESS;
}

physmem_error_t
physmem_add_memory_region (uint8 node, physaddr_t startaddress, size_t length) {

  /*We want to loop over and over in region lengths of 128M*/
  size_t remaining_mem = length;

  while (remaining_mem > 0) {

    /* If this is the first region being added, it *must* be less than the initial
     * bitmap can store */

    if (phys_regions[0].total_pages == 0) {
      if (length < PAGE_SIZE * PAGE_SIZE * 8) {
        return PHYSMEM_ERR_INITCOND;
      }

      phys_regions[0].startpos = startaddress;
      phys_regions[0].total_pages = phys_regions[0].free_pages = remaining_mem / PAGE_SIZE;
      phys_regions[0].node = 0;
    } else {
      /* Not the first region, carry on as per normal */
      
      /*TODO: Check for existing overlap */
      uint32 curregion = 0;
      physaddr_t new_physbitmap;
      uint32 *new_bitmap;
      physmem_error_t p_error;

      while (phys_regions[curregion].total_pages != 0) {
        ++curregion;
        ASSERT (curregion != PHYS_MAX_REGIONS);
      }

      p_error = physmem_allocate_page (0, &new_physbitmap);
      if (p_error != PHYSMEM_SUCCESS) {
        /*Not enough free memory to add more memory...*/
        return PHYSMEM_ERR_OOM;
      }
      
      /* Get space in virtmem for the new bitmap */
      new_bitmap = virtmem_get_kernelspace_page ();
      virtmem_map_kernel_to_physical (new_physbitmap);

      phys_regions[curregion].total_pages = phys_regions[curregion].free_pages = 
        ((remaining_mem > (PAGE_SIZE * PAGE_SIZE * 8)) ? (PAGE_SIZE * PAGE_SIZE * 8) :
        remaining_mem) / PAGE_SIZE;

      phys_regions[curregion].startpos = length - remaining_mem + startaddress;
      phys_regions[curregion].node = node;

    }

      remaining_mem -= (PAGE_SIZE * PAGE_SIZE * 8);
    }

    return PHYSMEM_SUCCESS;
  }

    
