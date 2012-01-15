#ifndef INCLUDE_SLAB_H
#define INCLUDE_SLAB_H

#include <queue.h>
#include <cpu.h>

#define MAX_SLAB_NAME_LEN 32

typedef enum {
  KMEM_SUCCESS,
  KMEM_ERR_USED, /* Triggered if trying to create cache with duplicate name */
  KMEM_ERR_INVALID, /* An invalid parameter was given */
  KMEM_ERR_OOM, /*Out of physical memory */
} kmem_error_t;

struct kmem_slab {
  SLIST_ENTRY(kmem_slab) slabs;
  unsigned short num_free;
  unsigned short * freelist;
};

struct kmem_cache;

struct kmem_cache_vfuncs {
  void (*reap)(struct kmem_cache *);
  void *(*alloc)(struct kmem_cache *);
  void (*free)(struct kmem_cache *, void *);
};

struct kmem_cache {

  char name[MAX_SLAB_NAME_LEN];
  unsigned int objsize;
  unsigned int used;
  struct kmem_cache_vfuncs *v;
  struct cpu *cpu;

  SLIST_HEAD(, slab) slabs_full;
  SLIST_HEAD(, slab) slabs_partial;
  SLIST_HEAD(, slab) slabs_empty;

  void (*ctor)(void *object);
  void (*dtor)(void *object);
  

};

struct kmem_allocator_vfuncs {
  void (*kmem_init)();
  struct kmem_cache *(*kmem_cache_alloc)();
  kmem_error_t (*kmem_cache_init)(struct kmem_cache *, struct cpu *cpu, 
      const char *name, size_t size, void (*ctor)(void *), 
      void (*dtor)(void *));
};

struct kmem_allocator {
  struct kmem_allocator_vfuncs av;
  struct kmem_cache_vfuncs cv;
};


void *common_kmem_cache_alloc(struct kmem_cache *);
void common_kmem_cache_free(struct kmem_cache *, void *);
void common_kmem_cache_reap(struct kmem_cache *);
kmem_error_t common_kmem_cache_init(struct kmem_cache *k, struct cpu *c, 
    const char *name, size_t size, void (*ctor)(void *), 
    void (*dtor)(void *));

void *kmem_cache_alloc(struct kmem_cache *);
void kmem_cache_free(struct kmem_cache *, void *);
void kmem_cache_reap(struct kmem_cache *);

#endif

