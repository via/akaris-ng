#ifndef KERNEL_H
#define KERNEL_H

struct thread;

struct kernel {
  struct physmem *phys;
  struct mutex_vfuncs *mutex;
  struct cpu *bsp;
  void (*debug)(const char *, ...);
};

struct kernel *kernel();
struct thread *spawn(const void *elf);

#endif

