#ifndef COMMON_PHYSICAL_MEMORY_H
#define COMMON_PHYSICAL_MEMORY_H

#include "types.h"
#include "sysconf.h"
#include "queue.h"

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
struct kernel;

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

struct physmem_region {
  uint32 *bitmap;
  uint32 free_pages, used_pages, bitmap_length;
  physaddr_t start_address;

  SLIST_ENTRY(physmem_region) regions;
};

struct physmem_vfuncs {

  /* Function API */
  physmem_error_t (*page_alloc)(struct physmem *, uint8 node, 
      physaddr_t *address);
  physmem_error_t (*page_free)(struct physmem *, physaddr_t address);
  struct physmem_stats (*stats_get)(struct physmem *);
  uint32 (*page_size)(struct physmem *);

  physmem_error_t (*region_add)(struct physmem *, uint8 node, 
      physaddr_t start, physaddr_t end);

};

struct physmem {
  struct kernel *parent;
  const char *name;
  unsigned int num_nodes;
  struct physmem_vfuncs v;
  SLIST_HEAD(, physmem_region) regionlist;
};



/* Base implementations */
physmem_error_t physmem_page_alloc(struct physmem *, uint8, physaddr_t *);
physmem_error_t physmem_page_free(struct physmem *, physaddr_t);
struct physmem_stats physmem_stats_get(struct physmem * );
uint32 physmem_page_size(struct physmem *);
physmem_error_t physmem_region_add(struct physmem *, uint8, 
    physaddr_t, physaddr_t);


#endif
