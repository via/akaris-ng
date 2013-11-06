#ifndef COMMON_CPU_H
#define COMMON_CPU_H

#include "kernel.h"
#include "physical_memory.h"
#include "virtual_memory.h"
#include "slab.h"
/* Generic structures for CPU */

struct cpu_vfuncs;
struct cpu;

struct cpu_vfuncs {
  void (*init)(struct cpu *);
  void (*schedule)(struct cpu *);

};

struct cpu {
  struct cpu_vfuncs v;
  struct physmem *localmem;
  struct kmem_allocator *allocator;
  struct virtmem *kvirt; 
  char *model;


  /* Task list, run list, current process pointer */

};

/* Fetch currently executing cpu */
struct cpu *cpu();




#endif
