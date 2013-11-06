#include <check.h>
#include <stdlib.h>

#include "slab.h"
#include "test_physmem.h"

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

static const int ctor_marker = 0x11;
static const int dtor_marker = 0x22;

static virtmem_error_t fake_alloc(struct virtmem *m VAR_UNUSED, 
    virtaddr_t *a, unsigned int pages) {
  *a = malloc(pages * 4096);
  if (*a == NULL)
    return VIRTMEM_OOM;
  return VIRTMEM_SUCCESS;
}

static virtmem_error_t  fake_alloc_null(struct virtmem *m VAR_UNUSED, 
    virtaddr_t *a, unsigned int pages VAR_UNUSED) {
  *a = NULL;
  return VIRTMEM_OOM;
}

static virtmem_error_t fake_vmem_map_virt_to_phys(struct virtmem *v VAR_UNUSED,
    physaddr_t p VAR_UNUSED, virtaddr_t addr VAR_UNUSED) {
  return VIRTMEM_SUCCESS;
}

static struct virtmem test_kvirt = {
  .v = {
    .kernel_alloc = fake_alloc,
    .kernel_map_virt_to_phys = fake_vmem_map_virt_to_phys,
  },
};

static void check_kmem_cache_ctor(void *_obj) {
  int *obj = (int *)_obj;
  *obj = ctor_marker;
  
}

static void check_kmem_cache_dtor(void *_obj) {
  /* This is a double pointer so that the obj can be set to a int* and the
   * marker can be set to an external int for verification
   */

  int **obj = (int **)_obj;
  **obj = dtor_marker;
}

static void check_kmem_cache_setup() {
  const unsigned int n_pages = 24;
  kernel()->phys = test_physmem_alloc(kernel(), n_pages);
  cpu()->kvirt = &test_kvirt;
  cpu()->localmem = kernel()->phys;
  cpu()->allocator = &test_allocator;
}

/* I know this is huge, need to learn how to use fixtures to more easily split
 * apart the source
 */
START_TEST (check_kmem_cache_init) {

  const unsigned int slab_size = 32;
  kmem_error_t err;

  struct kmem_cache *k = test_kmem_cache_alloc();


  err = common_kmem_cache_init(k, cpu(), "test-slab-32", slab_size,
      check_kmem_cache_ctor, check_kmem_cache_dtor);

  fail_unless(err == KMEM_SUCCESS);
  fail_unless(k->ctor == check_kmem_cache_ctor);
  fail_unless(k->dtor == check_kmem_cache_dtor);
  fail_unless(strcmp(k->name, "test-slab-32") == 0);
  fail_unless(k->objsize == slab_size);
  fail_unless(k->used == 0);
  fail_unless(k->cpu == cpu());
  fail_unless(SLIST_EMPTY(&k->slabs_empty));
  fail_unless(SLIST_EMPTY(&k->slabs_full));
  fail_unless(SLIST_EMPTY(&k->slabs_partial));
  fail_unless(k->v == &test_allocator.cv);

  /*Now test case of size < 0 */
  err = common_kmem_cache_init(k, cpu(), "test-slab-0", 0,
      check_kmem_cache_ctor, check_kmem_cache_dtor);
  fail_unless(err == KMEM_ERR_INVALID);

  free(k);

} END_TEST

START_TEST (check_kmem_cache_alloc_ctor_dtor) {

  int *ptr;
  int dtor_tester;
  
  struct kmem_cache *k = test_allocator.av.kmem_cache_alloc();
  test_allocator.av.kmem_cache_init(k, cpu(), "test-slab-32", 32,
      check_kmem_cache_ctor, check_kmem_cache_dtor);

  ptr = (int *)kmem_cache_alloc(k);
  fail_if(ptr == NULL);
  fail_unless(*ptr == ctor_marker);

  *((int **)(ptr)) = &dtor_tester;

  kmem_cache_free(k, ptr);
  fail_unless(dtor_tester == dtor_marker);

} END_TEST

START_TEST (check_kmem_cache_alloc) {

  void *ptr;
  struct kmem_cache *k = test_allocator.av.kmem_cache_alloc();
  test_allocator.av.kmem_cache_init(k, cpu(), "test-slab-32", 32,
      NULL, NULL);

  ptr = kmem_cache_alloc(k);

  fail_unless(SLIST_EMPTY(&k->slabs_empty));
  fail_unless(SLIST_EMPTY(&k->slabs_full));
  fail_unless(!SLIST_EMPTY(&k->slabs_partial));
  struct kmem_slab *slab = SLIST_FIRST(&k->slabs_partial);
  fail_if(ptr <= *(slab->freelist + slab->num_total));
  fail_unless(slab->first_free == &slab->freelist[1]);
  fail_unless(ptr == slab->freelist[0]);
  fail_unless(k->used == 1);
  fail_if(ptr <= (void *)slab);
  fail_if(ptr >= (void *)slab + 4096);

  /* Test what happens if allocations fail */
  cpu()->kvirt->v.kernel_alloc = fake_alloc_null;
  while ((ptr = kmem_cache_alloc(k)) != NULL) {
    fail_if(ptr <= (void *)slab);
    fail_if(ptr >= (void *)slab + 4096);
  }

} END_TEST

void check_initialize_slab_tests(TCase *t) {
  tcase_add_checked_fixture(t, check_kmem_cache_setup, NULL);
  tcase_add_test(t, check_kmem_cache_init);
  tcase_add_test(t, check_kmem_cache_alloc_ctor_dtor);
  tcase_add_test(t, check_kmem_cache_alloc);
}
