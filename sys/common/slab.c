/* slab.c -- base implementation of slab management */

#include <types.h>
#include <strfuncs.h>

#include "assert.h"
#include "queue.h"
#include "slab.h"
#include "virtual_memory.h"

static void
kmem_initialize_slab(struct kmem_cache *cache, struct kmem_slab *s) {

  assert(cache != NULL);
  assert(s != NULL);
  assert(cache->cpu != NULL);
  assert(cache->cpu->localmem != NULL);

  assert(s->n_pages != 0);
  assert(cache->objsize != 0);


  int n_elements;
  int i;
  void *cur_obj;
  unsigned long size = s->n_pages * physmem_page_size(cache->cpu->localmem);

  /* Formula for number of elements that can fit, accounting for the free list
   * and slab manager struct is:
   * (page size * size_of_slab - sizeof(manager struct) - sizeof(void*)  /
   * (object size + size of a void* (free list)
   */
  n_elements = 
    (size - sizeof(struct kmem_slab) - sizeof(void*)) / 
      (cache->objsize + sizeof(void *));
  s->num_total = n_elements;
  assert(n_elements > 0);

  /* Iterate over every object through n_elements, setting cur_obj to it along
   * the way */
  for (i = 0, cur_obj = (void *)s + size - (cache->objsize * n_elements); 
      i < n_elements;
      ++i, cur_obj += cache->objsize) {
    s->freelist[i] = cur_obj;
    /* Set this freelist entry to the object */
  }
  s->freelist[i] = NULL;
  s->first_free = &s->freelist[0];


}

void *common_kmem_cache_alloc(struct kmem_cache *cache) {
  assert(cache != NULL);
  assert(cache->cpu != NULL);
  assert(cache->cpu->kvirt != NULL);

  struct kmem_slab *s = NULL;
  void *obj = NULL;
  virtmem_error_t verr;
  physmem_error_t perr;

  if (!SLIST_EMPTY(&cache->slabs_partial)) {
    s = SLIST_FIRST(&cache->slabs_partial);
  } else if (!SLIST_EMPTY(&cache->slabs_empty)) {
    s = SLIST_FIRST(&cache->slabs_empty);
    SLIST_REMOVE_HEAD(&cache->slabs_empty, slabs);
    SLIST_INSERT_HEAD(&cache->slabs_partial, s, slabs);
  } else {
    /*We have no slabs in either empty or partial, get some */
    virtaddr_t vaddr;
    physaddr_t paddr;
    verr = virtmem_kernel_alloc(cache->cpu->kvirt, &vaddr, 1);
    if (verr != VIRTMEM_SUCCESS) {
      /* Possibly attempt reaping? But now just fail */
      return NULL;
    }
    perr = physmem_page_alloc(cache->cpu->localmem, 0, &paddr);
    if (perr != PHYSMEM_SUCCESS) {
      /* Possibly attempt reaping? But now just fail */
      return NULL;
    }
    virtmem_kernel_map_virt_to_phys(cache->cpu->kvirt, paddr, vaddr);
    s = (struct kmem_slab *)vaddr;
    s->n_pages = 1;
    kmem_initialize_slab(cache, s);
    SLIST_INSERT_HEAD(&cache->slabs_partial, s, slabs);
  }



  obj = *s->first_free;
  assert(obj != NULL);
  s->first_free++;
  if (*s->first_free == NULL) { /*This was the last object */
    /* Move to full */
    SLIST_REMOVE_HEAD(&cache->slabs_partial, slabs);
    SLIST_INSERT_HEAD(&cache->slabs_full, s, slabs);
  }
  
  if (cache->ctor != NULL) {
    cache->ctor(obj);
  }
  cache->used++;

  return obj;

}

void common_kmem_cache_free(struct kmem_cache *cache, void *obj) {
  assert(cache != NULL);
  assert(obj != NULL);

  struct kmem_slab *s;

  if (!SLIST_EMPTY(&cache->slabs_partial)) {
    s = SLIST_FIRST(&cache->slabs_partial);
  } else if (!SLIST_EMPTY(&cache->slabs_full)) {
    s = SLIST_FIRST(&cache->slabs_full);
    SLIST_REMOVE_HEAD(&cache->slabs_full, slabs);
    SLIST_INSERT_HEAD(&cache->slabs_partial, s, slabs);
  } else {
    /* This means the cache thinks there's nothing allocated but its been free'd
     * too anyway */
    assert(0);
  }

  if (cache->dtor != NULL) {
    cache->dtor(obj);
  }

  s->first_free--;
  *s->first_free = obj;
  cache->used--;

  /* Was that the last one? */
  if (s->first_free == (void **)&s->freelist) {
    /* If so, push it to empty */
    SLIST_REMOVE_HEAD(&cache->slabs_partial, slabs);
    SLIST_INSERT_HEAD(&cache->slabs_empty, s, slabs);
  }

}

void common_kmem_cache_reap(struct kmem_cache *cache VAR_UNUSED) {

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


