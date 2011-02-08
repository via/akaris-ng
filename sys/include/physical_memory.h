#ifndef COMMON_PHYSICAL_MEMORY_H
#define COMMON_PHYSICAL_MEMORY_H

#include "types.h"
#include "sysconf.h"

/* Defines the outward facing interface for any arch-specific physical memory
 * functionality.  This api is implemented by the arch-specific stuff
 */

/* Types */
#if PHYS_BITS == 32
typedef uint32 physaddr_t;
#elif PHYS_BITS == 64
typedef uint64 physaddr_t;
#endif

typedef enum {
  PHYSMEM_SUCCESS,
  PHYSMEM_ERR_MEMUSED,
  PHYSMEM_ERR_INITCOND,
  PHYSMEM_ERR_OOM,
} physmem_error_t;

typedef enum {
  PHYSMEM_USED = 1,
  PHYSMEM_FREE = 0,
} physmem_stat_t;


/* Function API */
physmem_error_t physmem_initialize ();
physmem_error_t physmem_add_memory_region (uint8 node, physaddr_t startaddress, size_t length);
physmem_error_t physmem_change_node (uint8 newnode, physaddr_t region_start, size_t length);
physmem_error_t physmem_change_status (physmem_stat_t mode, physaddr_t start, size_t length);
physmem_error_t physmem_delete_region (physaddr_t region_start, size_t length);

physmem_error_t physmem_allocate_page (uint8 node, physaddr_t *address);
physmem_error_t physmem_deallocate_page (physaddr_t address);



/* Helpers */
uint32 physmem_page_size ();



#endif
