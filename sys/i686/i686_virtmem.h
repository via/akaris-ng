#ifndef I686_VIRTMEM_H
#define I686_VIRTMEM_H

struct i686_pde {
  unsigned int present : 1;
  unsigned int writable : 1;
  unsigned int user : 1;
  unsigned int write_through : 1;
  unsigned int cache_disable : 1;
  unsigned int accessed : 1;
  unsigned int zero : 1;
  unsigned int size : 1;
  unsigned int global : 1;
  unsigned int avail : 3;
  physaddr_t phys_addr : 20;
} __attribute__((__packed__));

struct i686_pte {
  unsigned int present : 1;
  unsigned int writable : 1;
  unsigned int user : 1;
  unsigned int write_through : 1;
  unsigned int cache_disable : 1;
  unsigned int accessed : 1;
  unsigned int dirty : 1;
  unsigned int zero : 1;
  unsigned int global : 1;
  unsigned int avail : 3;
  physaddr_t phys_addr : 20;
} __attribute__((__packed__));

struct i686_pagewalk_context {
  virtaddr_t pagestart;
  struct i686_pte *pt;
  struct i686_pde *pd;
};

typedef enum {
  I686_PAGE_PRESENT = 1,
  I686_PAGE_WRITABLE = 2,
  I686_PAGE_WRITETHROUGH = 4,
  I686_PAGE_CACHEDISABLE = 8,
  I686_PAGE_GLOBAL = 16,
  I686_PAGE_USER   = 32,
} i686_page_flags;


struct i686_virtmem {
  struct virtmem virt;
  struct i686_pde *kernel_pde_list;
  virtaddr_t identitymap_limit;
};

extern struct i686_pde kernel_pd[1024] __attribute__((aligned));
extern struct i686_pte kernel_pts[256][1024] __attribute__((aligned(4096)));

struct virtmem *i686_virtmem_init(struct kernel *kernel);

#endif

