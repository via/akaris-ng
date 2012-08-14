
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
#include "physmem.h"
#include "virtual_memory.h"
#include "i686_virtmem.h"

#define LOADER_STACKSIZE 64
static char loader_stack[LOADER_STACKSIZE] __attribute__((aligned(4)));

struct i686_pde temp_pde[1024] __attribute__((aligned(4096)));
struct i686_pte kernel_pt[1024] __attribute__((aligned(4096)));
struct i686_pte identity_pt[1024] __attribute__((aligned(4096)));

void loader_start(unsigned long magic, multiboot_info_t *info) {
  while(1);
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
