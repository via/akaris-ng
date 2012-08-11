/* Implementation of a buddy allocator.  
 *
 * To use, fill out a buddy_allocator struct to the desired granularity and
 * size.  Calling buddy_sizerequired() will return the size of the block of
 * memory required to initialize the allocator.  Then use buddy_init() on the
 * struct and the address of the block of memory.
 *
 * buddy_alloc() and buddy_free() can be used to allocate/free blocks of any
 * size thats a multiple of the granularity.  Block coalescing is done on free.
 */


#include <types.h>

#include <buddy.h>


/* Number of actual bytes a given level of the map takes up.  */
static unsigned long
buddy_levelsize(struct buddy_allocator *b, unsigned int level) {
  return (((1 << (b->levels - level)) * b->granularity) /
    b->page_size) / 8; /*Convert first to actual memory size,
                         and then to pages and then bits per byte */
}

/* Byte location into the map where the start of a given level is */
static unsigned long
buddy_levelstart(struct buddy_allocator *b, unsigned int level) {

  unsigned int curlevel;
  unsigned long curtotal = 0;
  for (curlevel = 0; curlevel < level; ++level) {
    curtotal += buddy_levelsize(b, curlevel);
  }
  return curtotal;
}


void *
buddy_alloc(struct buddy_allocator *b, long blocks) {

  return NULL;
}

void
buddy_free(struct buddy_allocator *b, long blocks, void *addr) {

}




long
buddy_sizerequired(struct buddy_allocator *b) {
  return buddy_levelstart(b, b->levels);
}

