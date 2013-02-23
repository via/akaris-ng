#include "slab.h"

#define NUM_CACHES 8

static struct kmem_cache caches[NUM_CACHES];
static int next_free_cache;

static void
i686_kmem_init() {
  next_free_cache = 0;
}

static struct kmem_cache *
i686_kmem_cache_alloc() {

  return &caches[next_free_cache++];

}

struct kmem_allocator i686_kmem_allocator = {
  .av = {
    .kmem_init = i686_kmem_init,
    .kmem_cache_alloc = i686_kmem_cache_alloc,
    .kmem_cache_init = common_kmem_cache_init
  },
  .cv = {
    .reap = common_kmem_cache_reap,
    .alloc = common_kmem_cache_alloc,
    .free = common_kmem_cache_free
  }
};

