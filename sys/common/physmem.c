
#include "physical_memory.h"
#include "assert.h"

static physmem_stat_t 
physmem_block_status(uint32 *bitmap, uint32 size, uint32 block) {

  int index = block / (sizeof(uint32) * 8);
  int offset = block % (sizeof(uint32) * 8);

  ASSERT(index < size);

  return (bitmap[index] & (1 << offset)) ? PHYSMEM_USED : PHYSMEM_FREE;
}

static physmem_stat_t 
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

physmem_error_t physmem_page_alloc(uint8 node, physaddr_t address) {

  return PHYSMEM_SUCCESS;
}

physmem_error_t physmem_page_free(physaddr_t address) {

  return PHYSMEM_SUCCESS;
}

struct physmem_stats physmem_stats_get() {

  physmem_stats a;

  return a;

}

uint32 physmem_page_size() {

  ASSERT(0);
  return 0;

}

physmem_error_t (*region_add)(uint8 node, physaddr_t start, physaddr_t end) {

  return PHYSMEM_SUCCESS;
}

