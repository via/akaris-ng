#include <stdlib.h>

#include "slab.h"

void test_kmem_init() {
}

struct kmem_cache *test_kmem_cache_alloc() {
  struct kmem_cache *k = malloc(sizeof(struct kmem_cache));
  return k;
}

struct kmem_allocator test_allocator = {
  .av = {
    .kmem_init = test_kmem_init,
    .kmem_cache_alloc = test_kmem_cache_alloc,
    .kmem_cache_init = common_kmem_cache_init,
  },
  .cv = {
    .reap = common_kmem_cache_reap,
    .alloc = common_kmem_cache_alloc,
    .free = common_kmem_cache_free,
  },
};
