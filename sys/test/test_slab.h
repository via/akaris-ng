#ifndef TEST_SLAB_H
#define TEST_SLAB_H

void test_kmem_init();
struct kmem_cache *test_kmem_cache_alloc();
extern struct kmem_allocator test_allocator;

void check_initialize_slab_tests(TCase *);

#endif

