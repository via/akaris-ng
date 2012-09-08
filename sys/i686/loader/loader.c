
/* bootloader for i686
 *
 * Steps performed:
 *  - Set up paging for memory at the kernel start address and identity mapping
 *    the boot structures (from grub, memory map, modules lst)
 *  - Create the following at the 2MB physical memory mark:
 *      Kernel, info structure from loader, 
 *      grub modules (with pointers), and physical page list
 *  - Jump to kernel entry point (passing pointer to info structure)
 */

#include "multiboot.h"
#include "i686_physmem.h"
#include "virtual_memory.h"
#include "i686_virtmem.h"

#define LOADER_STACKSIZE 64
static char loader_stack[LOADER_STACKSIZE] __attribute__((aligned(4)));

struct i686_pde temp_pde[1024] __attribute__((aligned(4096)));
struct i686_pte kernel_pt[1024] __attribute__((aligned(4096)));
struct i686_pte identity_pt[1024] __attribute__((aligned(4096)));

void * __stack_chk_guard = 0;

void __stack_chk_guard_setup()
{
  unsigned char * p;
  p = (unsigned char *) &__stack_chk_guard;
  /* 32bit code, obviously */
  p[0] = 0;
  p[1] = 0;
  p[2] = '\n';
  p[3] = 133; /* <- this should probably be randomized */
}

void __attribute__((noreturn)) __stack_chk_fail()
{ 
  /* put your panic function or similar in here */
  unsigned char * vid = (unsigned char *)0xB8000;
  vid[1] = 7;
  for(;;)
    vid[0]++;
}

void enable_paging() {

  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "i"(temp_pde));
  __asm__("movl %cr0, %eax\n"
          "orl  $0x80000000, %eax\n"
          "movl %eax, %cr0");
          while (1);
}

void setup_tables() {
  temp_pde[0].phys_addr = (physaddr_t)&identity_pt / 4096;
  temp_pde[0].present = 1;
  temp_pde[768].phys_addr = (physaddr_t)&kernel_pt / 4096;
  temp_pde[768].present = 1;

  identity_pt[256].phys_addr = 256;
  identity_pt[256].present = 1;
  identity_pt[257].phys_addr = 257;
  identity_pt[257].present = 1;
  identity_pt[258].phys_addr = 258;
  identity_pt[258].present = 1;
  identity_pt[259].phys_addr = 259;
  identity_pt[259].present = 1;
  identity_pt[260].phys_addr = 260;
  identity_pt[260].present = 1;
}


void loader_start(unsigned long magic, multiboot_info_t *info) {

  setup_tables();
  enable_paging();
while (1);


}


void
start() {
  unsigned long magic;
  multiboot_info_t *mboot_info;

  __asm__ ("movl %0, %%esp" : : "i"(loader_stack + LOADER_STACKSIZE - 4));
  __asm__ ("movl %%eax, %0" : "=m"(magic));
  __asm__ ("movl %%ebx, %0" : "=m"(mboot_info));

  loader_start(magic, mboot_info);

  while (1);

}
