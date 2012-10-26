#include <stdarg.h>
#include "types.h"
#include "multiboot.h"
#include "strfuncs.h"
#include "bootvideo.h"

#include "assert.h"
#include "mutex.h"
#include "kernel.h"
#include "i686_cpu.h"
#include "i686_virtmem.h"
#include "i686_physmem.h"


#define T_STACK_SIZE 1024

static char t_stack[T_STACK_SIZE] __attribute__((aligned(4)));

static struct kernel i686_kernel;
static struct i686_cpu bootproc;
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

  i686_kernel.debug = i686_debug;

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    i686_debug("Not booted from multiboot loader!\n");
    while (1);
  }

  i686_debug("mods_addr: %x\nmod_start: %x\n", info->mods_addr,
      0);

  i686_kernel.mutex = &i686_mutex;
  i686_kernel.bsp->kvirt = i686_virtmem_init(&i686_kernel);
  i686_kernel.phys = i686_physmem_alloc(&i686_kernel, info);

  i686_cpu_alloc(&bootproc, &i686_kernel);
  i686_kernel.bsp = (struct cpu *)&bootproc;

  i686_kernel.bsp->v.init(i686_kernel.bsp);

  i686_debug("Location GDT entry: %x\n", bootproc.gdt);
  while (1);

  struct physmem_page *p;
  virtmem_error_t e = virtmem_kernel_virt_to_phys(i686_kernel.bsp->kvirt, &p, (virtaddr_t)0x100000);
  assert(e == VIRTMEM_NOTPRESENT);




}

