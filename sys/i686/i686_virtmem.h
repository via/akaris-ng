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

struct i686_virtmem {
  struct virtmem virt;
  struct i686_pde *kernel_pde_list;
};



struct virtmem *i686_virtmem_init(struct kernel *kernel);



#endif

