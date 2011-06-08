#ifndef COMMON_CPU_H
#define COMMON_CPU_H

/* Generic structures for CPU */

struct cpu_vfuncs;

struct cpu {
  struct kernel k;
  struct cpu_vfuncs v;
  struct physmem *localmem;
  unsigned char *model;


  /* Task list, run list, current process pointer */

};

struct cpu_vfuncs {
  void (*init)(struct cpu *);
  void (*schedule)(struct cpu *);

};




#endif
