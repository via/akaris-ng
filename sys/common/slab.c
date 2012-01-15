/* slab.c -- base implementation of slab management */

#include <types.h>
#include <strfuncs.h>

#include <slab.h>

void *common_kmem_cache_alloc(struct kmem_cache *cache) {
  return NULL;
}

void common_kmem_cache_free(struct kmem_cache *cache, void *obj) {

}

void common_kmem_cache_reap(struct kmem_cache *cache) {

}

kmem_error_t common_kmem_cache_init(struct kmem_cache *k, struct cpu *c, 
    const char *name, size_t size, void (*ctor)(void *), 
    void (*dtor)(void *)) {

  k->ctor = ctor;
  k->dtor = dtor;
  strlcpy(k->name, name, MAX_SLAB_NAME_LEN);

  if (size < 4)
    return KMEM_ERR_INVALID;
  k->objsize = size;

  k->used = 0;
  k->cpu = c;


  SLIST_INIT(&k->slabs_empty);
  SLIST_INIT(&k->slabs_full);
  SLIST_INIT(&k->slabs_partial);

  k->v = &c->allocator->cv; /*Set cache vfuncs to whats in allocator */

  return KMEM_SUCCESS;
}


inline void *kmem_cache_alloc(struct kmem_cache *k) {
  return k->v->alloc(k);
}

inline void kmem_cache_reap(struct kmem_cache *k) {
  k->v->reap(k);
}

inline void kmem_cache_free(struct kmem_cache *k, void *p) {
  k->v->free(k, p);
}
