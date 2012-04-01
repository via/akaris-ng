/* slab.c -- base implementation of slab management */

#include <types.h>
#include <strfuncs.h>

#include "assert.h"
#include "queue.h"
#include "slab.h"

static void
kmem_initialize_slab(struct kmem_cache *cache, struct kmem_slab *s) {

}

void *common_kmem_cache_alloc(struct kmem_cache *cache) {
  assert(cache != NULL);
  assert(cache->ctor != NULL);
  assert(cache->cpu != NULL);
  assert(cache->cpu->kvirt != NULL);

  struct kmem_slab *s = NULL;
  void *obj = NULL;

  if (!SLIST_EMPTY(&cache->slabs_partial)) {
    s = SLIST_FIRST(&cache->slabs_partial);
  } else if (!SLIST_EMPTY(&cache->slabs_empty)) {
    s = SLIST_FIRST(&cache->slabs_empty);
    SLIST_REMOVE_HEAD(&cache->slabs_empty, slabs);
    SLIST_INSERT_HEAD(&cache->slabs_partial, s, slabs);
  } else {
    /*We have no slabs in either empty or partial, get some */
    s = (struct kmem_slab *)kernel_alloc(cache->cpu->kvirt, 1);
    if (s == NULL) {
      /* Possibly attempt reaping? But now just fail */
      return NULL;
    }
    kmem_initialize_slab(cache, s);
  }



  obj = *s->first_free;
  assert(obj != NULL);
  s->first_free++;
  if (*s->first_free == NULL) { /*This was the last object */
    /* Move to full */
    SLIST_REMOVE_HEAD(&cache->slabs_partial, slabs);
    SLIST_INSERT_HEAD(&cache->slabs_full, s, slabs);
  }
  
  cache->ctor(obj);
  cache->used++;

  return obj;

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

  if (size < MIN_CACHE_SIZE)
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


