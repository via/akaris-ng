/* slab.c -- base implementation of slab management */

#include <slab.h>

static struct kmem_slab *kmem_cache_new_slab(unsigned int objsize) {
  return NULL;

}

void *kmem_cache_alloc(struct kmem_cache *cache) {

  return NULL;
}

void kmem_cache_free(struct kmem_cache *cache, void *obj) {

}

void kmem_cache_reap(struct kmem_cache *cache) {

}

