#ifndef COMMON_ADDRESS_SPACE_H
#define COMMON_ADDRESS_SPACE_H

struct memory_region;
struct address_space;

typedef enum {
  AS_SUCCESS,
  AS_NOTFOUND,
  AS_INVALID,
  AS_OOM,
} address_space_err_t;

struct memory_region_vfuncs {
  address_space_err_t (*memory_region_set_location)
    (struct memory_region *, virtaddr_t start, size_t len);
  address_space_err_t (*memory_region_set_flags)
    (struct memory_region *, int writable, int executable);
  address_space_err_t (*memory_region_clone)
    (struct memory_region *dst, struct memory_region *src, int cow);
  address_space_err_t (*memory_region_map)(struct memory_region *);
  void (*memory_region_fault)(struct memory_region *, virtaddr_t location);

};

struct memory_region {
  virtaddr_t start;
  size_t length;

  struct memory_region_vfuncs v;

  /* flags */
  int writable : 1;
  int executable : 1;
  int copy_on_write : 1;

  /* entry for list of regions in address space*/
  LIST_ENTRY(memory_region) regions;
  /* entry for list of regions currently COW with this one */
  LIST_ENTRY(memory_region) cow_regions;

};

struct address_space_vfuncs {
  address_space_err_t (*memory_region_alloc)
    (struct address_space *, struct memory_region **);
  address_space_err_t (*memory_region_free)
    (struct address_space *, struct memory_region *);
  address_space_err_t (*address_space_free)(struct address_space *);
  address_space_err_t (*address_space_get_memory_region)
    (struct address_space, struct memory_region **, virtaddr_t loc);
};


struct address_space {
  struct address_space_vfuncs v;

  LIST_HEAD(, memory_region) regions;

};

address_space_err_t common_memory_region_set_location(
    struct memory_region *, virtaddr_t start, size_t len);
address_space_err_t common_memory_region_set_flags(
    struct memory_region *, int writable, int executable);
address_space_err_t common_memory_region_clone(
    struct memory_region *dst, struct memory_region *src, int cow);
address_space_err_t common_memory_region_map(struct memory_region *);
void common_memory_region_cow_fault(struct memory_region *, 
    int location);
void common_memory_region_fault(struct memory_region *, int location);
address_space_err_t common_memory_region_alloc(struct address_space *, 
    struct memory_region **);
address_space_err_t common_memory_region_free(struct address_space *, 
    struct memory_region *);
address_space_err_t common_address_space_free(struct address_space *);
address_space_err_t common_address_space_alloc(struct address_space **);
void common_address_space_init();

#endif

