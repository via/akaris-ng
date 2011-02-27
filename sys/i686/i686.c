#include <stdarg.h>
#include "types.h"
#include "multiboot.h"
#include "strfuncs.h"
#include "bootvideo.h"

#include "kernel.h"
#include "physmem.h"


#define T_STACK_SIZE 1024

static char t_stack[T_STACK_SIZE] __attribute__((aligned(4)));

static struct kernel i686_kernel;
static char debugbuf[256];


static void i686_debug(const char *fmt, ...) {

  va_list ap;
  va_start(ap, fmt);

  k_snprintf_vaarg(debugbuf, 256, fmt, ap);
  bootvideo_puts(debugbuf);
  
  va_end(ap);

}

void
i686_kmain(unsigned long magic, multiboot_info_t *info) {


  bootvideo_cls();

  i686_kernel.phys = (struct physmem *)i686_physmem_alloc(&i686_kernel, info);
  i686_kernel.debug = i686_debug;

  i686_kernel.debug("%s: %d: %s\n", "Hello! ", 12345, "\nWorld!\n");



}


void
start() {
  unsigned long magic;
  multiboot_info_t *mboot_info;

  __asm__ ("movl %0, %%esp" : : "i"(t_stack  + T_STACK_SIZE - 1));
  __asm__ ("movl %%eax, %0" : "=m"(magic));
  __asm__ ("movl %%ebx, %0" : "=m"(mboot_info));

  i686_kmain(magic, mboot_info);

  while (1);

}



