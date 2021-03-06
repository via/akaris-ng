#include <stdarg.h>
#include "types.h"
#include "multiboot.h"
#include "strfuncs.h"
#include "bootvideo.h"
#include "i686_tty.h"

#include "assert.h"
#include "mutex.h"
#include "kernel.h"
#include "address_space.h"
#include "i686_cpu.h"
#include "i686_virtmem.h"
#include "i686_physmem.h"
#include "slab.h"
#include "i686_address_space.h"


static struct kernel i686_kernel;
static char debugbuf[256];
static int use_serial = 0;


static void i686_debug(const char *fmt, ...) {

  va_list ap;
  va_start(ap, fmt);

  k_snprintf_vaarg(debugbuf, 256, fmt, ap);
  if (use_serial)
    i686_tty_puts(0, debugbuf);
  else              
    bootvideo_puts(debugbuf);
  
  va_end(ap);

}

static void parse_cmdline(long cmdline) {
  const char *c = (const char *)cmdline;
  if (strstr(c, "use_serial") != NULL)
    use_serial = 1;
}

struct kernel *kernel() {
  return (struct kernel *)&i686_kernel;
}


void
i686_kmain(unsigned long magic, multiboot_info_t *info) {

  bootvideo_cls();

  parse_cmdline(info->cmdline);

  if (use_serial)
    i686_tty_init(0, 9600);

  i686_kernel.debug = i686_debug;

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    i686_debug("Not booted from multiboot loader!\n");
    while (1);
  }
 
  i686_debug("mods_addr: %x\nmod_start: %x\n", info->mods_addr,
      0);

  i686_kernel.mutex = &i686_mutex;
  i686_kernel.bsp = (struct cpu *)i686_cpu_alloc();
  i686_kernel.bsp->kvirt = i686_virtmem_init(&i686_kernel);
  i686_kernel.phys = i686_physmem_alloc(&i686_kernel, info);

  kmem_init(i686_kernel.bsp->allocator);
  i686_kernel.bsp->v.init(i686_kernel.bsp);

  i686_debug("Location GDT entry: %x\n", ((struct i686_cpu *)i686_kernel.bsp)->gdt);

  virtaddr_t a;
  physaddr_t p;
  virtmem_error_t e1 = virtmem_kernel_alloc(i686_kernel.bsp->kvirt, &a, 1);
  assert(e1 == VIRTMEM_SUCCESS);
  physmem_error_t e2 = physmem_page_alloc(i686_kernel.bsp->localmem, 0, &p);
  assert(e2 == PHYSMEM_SUCCESS);
  virtmem_kernel_map_virt_to_phys(i686_kernel.bsp->kvirt, p, a);
  i686_debug("Allocated address: %x(->%x)\n", a, p);

  char *s = (char *)a;

  strcpy(s, "This shows the validity of this memory");
  i686_debug("%x contains: %s\n", a, s);

  struct kmem_cache *s1 = kmem_alloc(i686_kernel.bsp->allocator);
  kmem_cache_init(i686_kernel.bsp->allocator,
      s1, i686_kernel.bsp, "test", 128, NULL, NULL);

  char *t1 = kmem_cache_alloc(s1);
  i686_debug("cache at %x provided us with %x\n", s1, t1);
  strcpy(t1, "This shows the validity of the slab allocation");
  i686_debug("%x contains: %s\n", t1, t1);

  i686_address_space_init();
  struct address_space *as;
  struct memory_region *mr;
  address_space_alloc(&as);
  memory_region_alloc(&mr);

  e1 = virtmem_kernel_alloc(i686_kernel.bsp->kvirt, &a, 1);
  virtmem_kernel_map_virt_to_phys(i686_kernel.bsp->kvirt, (physaddr_t)as->pd, a);
  
  address_space_init_region(as, mr, (virtaddr_t)0x1000000, 0x2000);
  memory_region_set_flags(mr, 1, 1);
  memory_region_map(as, mr, NULL);

  const char *teststr = "This is a test string to be copied to userspace.";
  char testcpybuf[128];
  char opcodes[] = {0xeb, 0xfe};
  virtmem_copy_kernel_to_user(i686_kernel.bsp->kvirt, as->pd, (void *)0x1000ffc, 
      (const void *)teststr, strlen(teststr) + 1);
  virtmem_copy_user_to_kernel(i686_kernel.bsp->kvirt, (void *)&testcpybuf, 
      as->pd, (const void *)0x1000ffc, strlen(teststr) + 1);
  i686_debug("testcpybuf contains '%s'\n", testcpybuf);
  virtmem_copy_kernel_to_user(i686_kernel.bsp->kvirt, as->pd, (void *)0x1000000, 
      (const void *)opcodes, 2);


  struct thread *thr1;
  scheduler_thread_alloc(cpu()->sched, &thr1);
  thread_init(thr1, as);
  thr1->state = THREAD_RUNNABLE;
  scheduler_thread_add(cpu()->sched, thr1);
  scheduler_reschedule(cpu()->sched);
  virtmem_user_setup_kernelspace(i686_kernel.bsp->kvirt, as->pd);
  virtmem_set_context(i686_kernel.bsp->kvirt, as->pd);
  scheduler_resume(cpu()->sched);
  while (1);
}

