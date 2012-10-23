
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
#include "i686_cpu.h"

#define LOADER_STACKSIZE 64
static char loader_stack[LOADER_STACKSIZE] __attribute__((aligned(4)));
static struct i686_gdt_entry gdt[3] __attribute__((aligned(4)));

struct i686_pde temp_pde[1024] __attribute__((aligned(4096)));
struct i686_pte kernel_pt[1024] __attribute__((aligned(4096)));
struct i686_pte identity_pt[1024] __attribute__((aligned(4096)));

void enable_paging() {

  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "i"(temp_pde));
  __asm__("movl %cr0, %eax\n"
          "orl  $0x80000000, %eax\n"
          "movl %eax, %cr0");
}

void setup_tables() {
  temp_pde[0].phys_addr = (physaddr_t)&identity_pt / 4096;
  temp_pde[0].present = 1;
  temp_pde[0].writable = 1;
  temp_pde[768].phys_addr = (physaddr_t)&kernel_pt / 4096;
  temp_pde[768].present = 1;
  int i;

  for (i = 0; i < 1024; ++i) {
      identity_pt[i].phys_addr = i;
      identity_pt[i].present = 1;
      identity_pt[i].writable = 1;
      kernel_pt[i].phys_addr = i;
      kernel_pt[i].present = 1;
      kernel_pt[i].writable = 1;
  }
}

static void i686_cpu_set_gdt(struct i686_gdt_entry *gdt, int length) {

  volatile struct {
    uint16 limit;
    uint32 base;
  } __attribute__((packed)) gdtr;

  gdtr.base = (uint32)gdt;
  gdtr.limit = (uint16)(length * sizeof(struct i686_gdt_entry));

  __asm__("lgdt %0\n"
          "ljmp $0x08, $use_new_gdt\n" 
          "use_new_gdt:\n"
          "movw $0x10, %%ax\n"
          "movw %%ax, %%ds\n"
          "movw %%ax, %%es\n"
          "movw %%ax, %%fs\n"
          "movw %%ax, %%gs\n"
          "movw %%ax, %%ss\n": : "m"(gdtr));

}

void setup_gdt() {

  gdt[0] = (struct i686_gdt_entry) {
    .limit_low = 0, .base_low = 0, .base_mid = 0,
    .type = 0, .system = 0, .dpl = 0, .present = 0,
    .limit_high = 0, .avl = 0, .reserved = 0,
    .op_size = 0, .granularity = 0, .base_high = 0,
  };

  gdt[1] = (struct i686_gdt_entry) {
    .limit_low = 0xFFFF, .base_low = 0, .base_mid = 0,
    .type = 0xA, .system = 1, .dpl = 0, .present = 1,
    .limit_high = 0xF, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0,
  };

  gdt[2] = (struct i686_gdt_entry) {
    .limit_low = 0xFFFF, .base_low = 0, .base_mid = 0,
    .type = 0x2, .system = 1, .dpl = 0, .present = 1,
    .limit_high = 0xF, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0,
  };

  i686_cpu_set_gdt(gdt, 3);

}



void
loader_start() {
  unsigned long magic;
  multiboot_info_t *mboot_info;

  __asm__ ("movl %0, %%esp" : : "i"(loader_stack + LOADER_STACKSIZE - 4));
  __asm__ ("movl %%eax, %0" : "=m"(magic));
  __asm__ ("movl %%ebx, %0" : "=m"(mboot_info));


  setup_tables();
  setup_gdt();
  enable_paging();

  i686_kmain(magic, mboot_info);

}
