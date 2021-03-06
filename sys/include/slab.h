#ifndef INCLUDE_SLAB_H
#define INCLUDE_SLAB_H

#include <queue.h>
#include <cpu.h>

#define MAX_SLAB_NAME_LEN 32
#define MIN_CACHE_SIZE 1

typedef void (*kmem_cache_ctor)(void *o);
typedef void (*kmem_cache_dtor)(void *o);

typedef enum {
  KMEM_SUCCESS,
  KMEM_ERR_USED, /* Triggered if trying to create cache with duplicate name */
  KMEM_ERR_INVALID, /* An invalid parameter was given */
  KMEM_ERR_OOM, /*Out of physical memory */
} kmem_error_t;

struct kmem_slab {
  SLIST_ENTRY(kmem_slab) slabs;
  unsigned int num_total;
  unsigned int n_pages;
  void **first_free; /*Points to the index of the first free one */
  void *freelist[0]; /*Pre-allocated list of all pages */
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

  SLIST_HEAD(, kmem_slab) slabs_full;
  SLIST_HEAD(, kmem_slab) slabs_partial;
  SLIST_HEAD(, kmem_slab) slabs_empty;

  kmem_cache_ctor ctor;
  kmem_cache_dtor dtor;
  

};

struct kmem_allocator_vfuncs {
  void (*kmem_init)();
  struct kmem_cache *(*kmem_cache_alloc)();
  kmem_error_t (*kmem_cache_init)(struct kmem_cache *, struct cpu *cpu, 
      const char *name, size_t size, kmem_cache_ctor, kmem_cache_dtor );
};

struct kmem_allocator {
  struct kmem_allocator_vfuncs av;
  struct kmem_cache_vfuncs cv;
};


void *common_kmem_cache_alloc(struct kmem_cache *);
void common_kmem_cache_free(struct kmem_cache *, void *);
void common_kmem_cache_reap(struct kmem_cache *);
kmem_error_t common_kmem_cache_init(struct kmem_cache *k, struct cpu *c, 
    const char *name, size_t size, kmem_cache_ctor, kmem_cache_dtor);


static inline void *kmem_cache_alloc(struct kmem_cache *k) {
  return k->v->alloc(k);
}

static inline void kmem_cache_reap(struct kmem_cache *k) {
  k->v->reap(k);
}

static inline void kmem_cache_free(struct kmem_cache *k, void *p) {
  k->v->free(k, p);
}

static inline void kmem_init(struct kmem_allocator *k) {
  k->av.kmem_init();
}

static inline struct kmem_cache *
kmem_alloc(struct kmem_allocator *k) {
  return k->av.kmem_cache_alloc();
}

static inline kmem_error_t kmem_cache_init( struct kmem_allocator *k, 
    struct kmem_cache *c, struct cpu *cpu, const char *name, size_t size, 
    kmem_cache_ctor ctor, kmem_cache_dtor dtor) {
  return k->av.kmem_cache_init(c, cpu, name, size, ctor, dtor);
}


#endif

