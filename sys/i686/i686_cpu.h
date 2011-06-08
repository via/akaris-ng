#ifndef SYS_I686_CPU_H
#define SYS_I686_CPU_H

#include "cpu.h"

struct i686_cpu {
  struct cpu c;

  /* CPUID, GDT */
};

void i686_cpu_init(struct cpu *);
void i686_cpu_schedule(struct cpu *);


#endif
