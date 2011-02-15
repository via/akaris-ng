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

struct physmem_stats;
struct physmem;
struct physmem_vfuncs;

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

struct physmem_stats {
  unsigned int kernel_pages;
  unsigned int free_pages;
};

struct physmem {
  const char *name;
  unsigned int nodes;
  struct physmem_vfuncs *v;
};

struct physmem_vfuncs {

  /* Function API */
  physmem_error_t (*physmem_page_alloc)(uint8 node, physaddr_t *address);
  physmem_error_t (*physmem_page_free)(physaddr_t address);
  struct physmem_stats (*physmem_stats_get)();
  uint32 (*physmem_page_size)();

};


/* Helpers */



#endif
