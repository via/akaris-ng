
#include "assert.h"
#include "queue.h"
#include "physical_memory.h"


physmem_stat_t 
physmem_block_status(uint32 *bitmap, uint32 size, uint32 block) {

  int index = block / (sizeof(uint32) * 8);
  int offset = block % (sizeof(uint32) * 8);

  ASSERT(index < size);

  return (bitmap[index] & (1 << offset)) ? PHYSMEM_USED : PHYSMEM_FREE;
}

physmem_stat_t 
physmem_block_set(uint32 *bitmap, uint32 size,
    uint32 block, physmem_stat_t value) {

  int index = block / (sizeof(uint32) * 8);
  int offset = block % (sizeof(uint32) * 8);

  ASSERT(index < size);

  physmem_stat_t old =  (bitmap[index] & (1 << offset)) ? PHYSMEM_USED : PHYSMEM_FREE;
  if (value == PHYSMEM_FREE) {
    bitmap[index] &= ~(1 << offset);
  } else {
    bitmap[index] |= (1 << offset);
  }

  return old;

}

uint32
physmem_block_find_free(uint32 *bitmap, uint32 size) {
  
  int index;
  for (index = 0; index < size; ++index) {
    if (bitmap[index] != 0xFFFFFFFF) {
      int offset;
      for (offset = 0; offset < 32; ++offset) {
        if (physmem_block_status(bitmap, size, index * 32 + offset)
            == PHYSMEM_FREE) {
          physmem_block_set(bitmap, size, index * 32 + offset, PHYSMEM_USED);
          return index * 32 + offset;
        }
      }
    }
  }
  if (index == size) {
    while (1); /* Should not be here ever */
  }
  return 0;

}

physmem_error_t physmem_page_alloc(struct physmem *_phys, uint8 node, physaddr_t *address) {

  struct physmem_region *region;
  physaddr_t allocated = 0;

  SLIST_FOREACH(region, &_phys->regionlist, regions) {
    if (region->free_pages) {
      /* TODO: when mutexes work, use it to lock this */
      int newblock = physmem_block_find_free(region->bitmap, region->bitmap_length);
      region->free_pages--;
      region->used_pages++;
      physmem_block_set(region->bitmap, region->bitmap_length, newblock, PHYSMEM_USED);
      allocated = region->start_address + newblock * _phys->v.page_size(_phys);
      if (allocated == 0) {
        return PHYSMEM_ERR_INITCOND;
      }
      break;

    }
  }

  if (region == NULL) { /* Could not find a region with free pages */
    *address = NULL;
    return PHYSMEM_ERR_OOM;
  }

  *address = allocated;

  return PHYSMEM_SUCCESS;
}

physmem_error_t physmem_page_free(struct physmem *_phys, physaddr_t address) {
  struct physmem_region *region;

  SLIST_FOREACH(region, &_phys->regionlist, regions) {
    if ( (address >= region->start_address) &&
        (address <= region->start_address + 
         (region->free_pages + region->used_pages) * _phys->v.page_size(_phys))) {
        /* This page is in this region */
    }
  }
  return PHYSMEM_SUCCESS;
}

struct physmem_stats physmem_stats_get(struct physmem *_phys) {

  struct physmem_stats a;

  return a;

}

uint32 physmem_page_size(struct physmem *_phys) {

  ASSERT(0);
  return 0;

}

physmem_error_t physmem_region_add(struct physmem *_phys, uint8 node, 
    physaddr_t start, physaddr_t end) {

  return PHYSMEM_SUCCESS;
}

