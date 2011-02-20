#include "types.h"
#include "multiboot.h"
#include "strfuncs.h"
#include "bootvideo.h"

#include "kernel.h"
#include "physmem.h"

#define T_STACK_SIZE 1024

static char t_stack[T_STACK_SIZE] __attribute__((aligned(4)));

static struct kernel i686_kernel;


void
i686_kmain(unsigned long magic, multiboot_header_t *info) {


  bootvideo_cls();
  bootvideo_puts("Hello, World");

  i686_kernel.phys = (struct physmem *)i686_physmem_alloc(&i686_kernel, info);



}


void
start() {
  unsigned long magic;
  multiboot_header_t *mboot_info;

  __asm__ ("movl %0, %%esp" : : "i"(t_stack  + T_STACK_SIZE - 1));
  __asm__ ("movl %%eax, %0" : "=m"(magic));
  __asm__ ("movl %%ebx, %0" : "=m"(mboot_info));

  i686_kmain(magic, mboot_info);

  while (1);

}



