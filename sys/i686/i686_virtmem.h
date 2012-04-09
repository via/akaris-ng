#ifndef I686_VIRTMEM_H
#define I686_VIRTMEM_H

struct i686_pde {
  physaddr_t phys_addr : 20;
  int avail : 3;
  int global : 1;
  int size : 1;
  int zero : 1;
  int accessed : 1;
  int cache_disable : 1;
  int write_through : 1;
  int user : 1;
  int writable : 1;
  int present : 1;
} __attribute__((__packed__));

struct i686_pte {
  physaddr_t phys_addr : 20;
  int avail : 3;
  int global : 1;
  int zero : 1;
  int dirty : 1;
  int accessed : 1;
  int cache_disable : 1;
  int write_through : 1;
  int user : 1;
  int writable : 1;
  int present : 1;
} __attribute__((__packed__));

struct i686_virtmem {
  struct virtmem virt;
  struct i686_pde *kernel_pde_list;
};



struct virtmem *i686_virtmem_init(struct kernel *kernel);



#endif

