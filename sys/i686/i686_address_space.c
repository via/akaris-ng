#include "cpu.h"
#include "physical_memory.h"
#include "address_space.h"


static struct address_space_vfuncs i686_as_vfuncs = {
  .destroy = common_address_space_destroy,
  .get_region = common_address_space_get_region,
  .init_region = common_address_space_init_region,
  .clone_region = common_memory_region_clone,
  .map_region = common_memory_region_map,
};
static struct memory_region_vfuncs i686_mr_vfuncs = {
  .set_location = common_memory_region_set_location,
  .set_flags    = common_memory_region_set_flags,
  .fault = NULL,
};

static void i686_address_space_ctor(void *_as) {
  struct address_space *as = (struct address_space *)_as;
  physaddr_t p;

  as->v = i686_as_vfuncs;

  /* Allocate page for page directory */
  physmem_page_alloc(cpu()->localmem, 0, &p);
  as->pd = (virtmem_md_context_t)p;
}

static void i686_address_space_dtor(void *_as) {
  struct address_space *as = (struct address_space *)_as;
  physmem_page_free(cpu()->localmem, (physaddr_t)as->pd);
}

static void i686_memory_region_ctor(void *_mr) {
  struct memory_region *mr = (struct memory_region *)_mr;
  mr->v = i686_mr_vfuncs;
}

void i686_address_space_init() {
  address_space_init(i686_address_space_ctor, i686_memory_region_ctor,
                     i686_address_space_dtor, NULL);
}


