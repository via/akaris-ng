#include "types.h"
#include "multiboot.h"
#include "strfuncs.h"

#define T_STACK_SIZE 1024

static volatile unsigned long magic;
static volatile multiboot_header_t *mboot_info;
static char t_stack[T_STACK_SIZE] __attribute__((aligned(4)));

void
start() {
  __asm__ ("movl %%eax, %0" : "=m"(magic));
  __asm__ ("movl %%ebx, %0" : "=m"(mboot_info));
  __asm__ ("movl %0, %%esp" : : "c"(t_stack  + T_STACK_SIZE - 1));


  while (1);

}
