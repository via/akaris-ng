#ifndef INCLUDE_SLAB_H
#define INCLUDE_SLAB_H

#include <queue.h>

struct slab {


};

struct slab_cache_vfuncs {

  void (*ctor)(void *slab);
  void (*dtor)(void *slab);
  unsigned int (*reap)();
};

struct slab_cache {

  const char *name;

  SLIST_HEAD(, slab) slabs_full;
  SLIST_HEAD(, slab) slabs_partial;
  SLIST_HEAD(, slab) slabs_empty;

  

};

struct slab_allocator_vfuncs {

  unsigned int (*reap)();
  struct slab_cache * (*create_allocator)(const char *, unsigned int);

};

struct slab_allocator {
  struct slab_allocator_vfuncs v;


};

#endif
