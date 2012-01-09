#ifndef INCLUDE_SLAB_H
#define INCLUDE_SLAB_H

#include <queue.h>
#include <cpu.h>

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

  const char *name;
  unsigned int objsize;
  unsigned int used;
  struct kmem_cache_vfuncs v;
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
  void (*kmem_cache_init)(struct cpu *cpu, const char * name, size_t size, 
      void (*ctor)(void *), void (*dtor)(void *));
};

struct kmem_allocator {
  struct kmem_allocator_vfuncs v;
};


void *kmem_cache_alloc(struct kmem_cache *);
void kmem_cache_free(struct kmem_cache *, void *);
void kmem_cache_reap(struct kmem_cache *);






#endif
