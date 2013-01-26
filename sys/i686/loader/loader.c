
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

static struct i686_gdt_entry gdt[3] __attribute__((aligned(4)));
extern int highstart;
#define T_STACK_SIZE 1024
char t_stack[T_STACK_SIZE] __attribute__((aligned(4))) = {0};

extern void i686_kmain(unsigned long, multiboot_info_t *);


void enable_paging(struct i686_pde *cr3) {

  __asm__("movl %0, %%eax\n"
          "movl %%eax, %%cr3" :: "m"(cr3));
  __asm__("movl %cr0, %eax\n"
          "orl  $0x80000000, %eax\n"
          "movl %eax, %cr0");
}

void setup_tables() {
  struct i686_pde *kpd = (struct i686_pde *)((physaddr_t)kernel_pd - (physaddr_t)&highstart);
  struct i686_pte (*kpts)[1024] = (struct i686_pte (*)[1024])((physaddr_t)kernel_pts - (physaddr_t)&highstart);
  int i, j;

  /* Quick and dirty clearing of all pagetables */
  for (i = 0; i < 256; ++i) {
    for (j = 0; j < 1024; ++j) {
      kpts[i][j] = (struct i686_pte){0};
    }
  }

  kpd[0].phys_addr = (physaddr_t)kpts[0] >> 12;
  kpd[0].present = 1;
  kpd[0].writable = 1;
  kpd[768].phys_addr = (physaddr_t)kpts[0] >> 12;
  kpd[768].present = 1;
  kpd[768].writable = 1;

  for (i = 0; i < 1024; ++i) {
      kpts[0][i].phys_addr = i;
      kpts[0][i].present = 1;
      kpts[0][i].writable = 1;
  }

  enable_paging(kpd);

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

/* The following is a giant hack due to the lack of gcc supporting an
 * i686 'naked' function attribute that would allow me to have
 * loader_main as entry point */


__asm(".global loader_start\n"
      "loader_start:\n"
      "movl $t_stack + 1024 - 4, %esp\n"
      "jmp loader_main\n");

void
loader_main(void) {
  unsigned long magic;
  multiboot_info_t *mboot_info;
  
  __asm__("movl %%eax, %0" : "=m"(magic));
  __asm__("movl %%ebx, %0" : "=m"(mboot_info));

  setup_gdt();
  setup_tables();

  /* Switch stack to one in highmemory */
  __asm__("addl %0, %%esp\n": : "i"(&highstart) : "%esp");

  i686_kmain(magic, mboot_info);

}
