
#include "types.h"
#include "multiboot.h"
#include "strfuncs.h"

struct multiboot_header header = {
  MULTIBOOT_HEADER_MAGIC,
  MULTIBOOT_HEADER_FLAGS,
  -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS),
};__attribute__((aligned(4)))

#define T_STACK_SIZE 4096

static char t_stack[T_STACK_SIZE] __attribute__((aligned(4)));

static volatile unsigned long magic;
static volatile multiboot_header_t *mboot_info;

void
kmain() {
  char *vmem = (char *)0xB8000;
  char buf[30];
  itoa(buf, 'x',  magic);
  int i;
  for (i = 0; i < 20; ++i) {
    vmem[i * 2] = buf[i];
  }
}

void
start() {
  __asm__ ("movl %%eax, %0" : "=m"(magic));
  __asm__ ("movl %%ebx, %0" : "=m"(mboot_info));
  __asm__ ("movl %0, %%esp" : : "c"(t_stack  + T_STACK_SIZE - 1));

  kmain();

  while (1);

}
