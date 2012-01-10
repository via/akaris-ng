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
struct physmem_page;
struct physmem_vfuncs;
struct kernel;

typedef enum {
  PHYSMEM_SUCCESS,
  PHYSMEM_ERR_MEMUSED,
  PHYSMEM_ERR_INITCOND,
  PHYSMEM_ERR_OOM,
  PHYSMEM_ERR_DOUBLEFREE,
} physmem_error_t;

typedef enum {
  PHYSMEM_USED = 1,
  PHYSMEM_FREE = 0,
} physmem_stat_t;

struct physmem_stats {
  unsigned int kernel_pages;
  unsigned int free_pages;
};


struct physmem_page {
  LIST_ENTRY(physmem_page) pages;
  /* Entry for vm area */
};

struct physmem_vfuncs {

  /* Function API */
  struct physmem_page * (*phys_to_page)(const struct physmem *,
      physaddr_t address);
  physaddr_t (* page_to_phys)(const struct physmem *, 
      const struct physmem_page *);

  physmem_error_t (*page_alloc)(struct physmem *, uint8 node, 
      physaddr_t *address);
  physmem_error_t (*page_free)(struct physmem *, physaddr_t address);

  struct physmem_stats (*stats_get)(const struct physmem *);
  uint32 (*page_size)(const struct physmem *);


};

struct physmem {
  struct kernel *parent;
  const char *name;
  struct physmem_vfuncs v;
  LIST_HEAD(, physmem_page) freelist;
  LIST_HEAD(, physmem_page) bootlist;
  unsigned long total_pages, free_pages;
};



/* Base implementations */

struct physmem_page * common_physmem_phys_to_page(physaddr_t address);
physaddr_t common_physmem_page_to_phys(const struct physmem_page *);

physmem_error_t common_physmem_page_alloc(struct physmem *, uint8, physaddr_t *);
physmem_error_t common_physmem_page_free(struct physmem *, physaddr_t);
struct physmem_stats common_physmem_stats_get(const struct physmem * );
uint32 common_physmem_page_size(const struct physmem *);


/* Feeder implementation */

struct feeder_physmem {
  struct physmem p;
  struct physmem *source;

  unsigned int pages_to_keep;
  unsigned int min_free_source_pages;
};

void feeder_physmem_create(struct feeder_physmem *new,
                                       struct physmem *source,
                                       unsigned int pages_to_keep,
                                       unsigned int min_source_pages);



#endif
