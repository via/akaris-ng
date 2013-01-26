#ifndef SYS_I686_CPU_H
#define SYS_I686_CPU_H

#include "kernel.h"
#include "physical_memory.h"
#include "cpu.h"

struct i686_gdt_entry {
  uint16 limit_low;
  uint16 base_low;
  uint8 base_mid;
  unsigned int type : 4;
  unsigned int system : 1;
  unsigned int dpl : 2;
  unsigned int present : 1;
  unsigned int limit_high : 4;
  unsigned int avl : 1;
  unsigned int reserved : 1;
  unsigned int op_size : 1;
  unsigned int granularity : 1;
  uint8 base_high;
} __attribute__((packed));

struct i686_cpu {
  struct cpu c;
  struct feeder_physmem feeder;
  struct i686_gdt_entry gdt[5] __attribute__((aligned(4)));
  struct i686_gdt_entry idt[256] __attribute__((aligned(4)));


  /* CPUID, GDT */
};



struct i686_cpu *i686_cpu_alloc(struct kernel *);


#endif
