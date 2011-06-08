#ifndef KERNEL_H
#define KERNEL_H


struct kernel {
  struct physmem *phys;
  struct mutex_vfuncs *mutex;
  struct cpu *bsp;
  void (*debug)(const char *, ...);
};

#endif

