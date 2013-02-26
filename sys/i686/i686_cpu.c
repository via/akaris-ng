#include "kernel.h"
#include "physical_memory.h"
#include "strfuncs.h"
#include "i686_cpu.h"
#include "slab.h"
#include "i686_slab.h"

/* Define entry points for the exception handlers */

I686_INT_HANDLER_NOERR(0, i686_int_handler)
I686_INT_HANDLER_NOERR(1, i686_int_handler)
I686_INT_HANDLER_NOERR(2, i686_int_handler)
I686_INT_HANDLER_NOERR(3, i686_int_handler)
I686_INT_HANDLER_NOERR(4, i686_int_handler)
I686_INT_HANDLER_NOERR(5, i686_int_handler)
I686_INT_HANDLER_NOERR(6, i686_int_handler)
I686_INT_HANDLER_NOERR(7, i686_int_handler)
I686_INT_HANDLER_WERR(8, i686_int_handler)
I686_INT_HANDLER_NOERR(9, i686_int_handler)
I686_INT_HANDLER_WERR(10, i686_int_handler)
I686_INT_HANDLER_WERR(11, i686_int_handler)
I686_INT_HANDLER_WERR(12, i686_int_handler)
I686_INT_HANDLER_WERR(13, i686_int_handler)
I686_INT_HANDLER_WERR(14, i686_int_handler)
I686_INT_HANDLER_NOERR(15, i686_int_handler)
I686_INT_HANDLER_NOERR(16, i686_int_handler)
I686_INT_HANDLER_WERR(17, i686_int_handler)
I686_INT_HANDLER_NOERR(18, i686_int_handler)
I686_INT_HANDLER_NOERR(19, i686_int_handler)
I686_INT_HANDLER_NOERR(20, i686_int_handler)

static void i686_cpu_set_gdt(struct i686_gdt_entry *gdt, int length) {

  volatile struct {
    uint16 limit;
    uint32 base; } __attribute__((packed)) gdtr;

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

static void i686_cpu_set_idt(struct i686_idt_entry *idt, int length) {

  volatile struct {
    uint16 limit;
    uint32 base;
  } __attribute__((packed)) idtr;

  idtr.base = (uint32)idt;
  idtr.limit = (uint16)(length * sizeof(struct i686_idt_entry));

  __asm__("lidt %0\n" : : "m"(idtr));

}

static void i686_setup_gdt(struct i686_cpu *cpu) {

  cpu->gdt[0] = (struct i686_gdt_entry) {
    .limit_low = 0, .base_low = 0, .base_mid = 0,
    .type = 0, .system = 0, .dpl = 0, .present = 0,
    .limit_high = 0, .avl = 0, .reserved = 0,
    .op_size = 0, .granularity = 0, .base_high = 0,
  };

  cpu->gdt[1] = (struct i686_gdt_entry) {
    .limit_low = 0xFFFF, .base_low = 0, .base_mid = 0,
    .type = 0xA, .system = 1, .dpl = 0, .present = 1,
    .limit_high = 0xF, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0,
  };

  cpu->gdt[2] = (struct i686_gdt_entry) {
    .limit_low = 0xFFFF, .base_low = 0, .base_mid = 0,
    .type = 0x2, .system = 1, .dpl = 0, .present = 1,
    .limit_high = 0xF, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0,
  };

  cpu->gdt[3] = (struct i686_gdt_entry) {
    .limit_low = 0xFFFF, .base_low = 0, .base_mid = 0,
    .type = 0xA, .system = 1, .dpl = 3, .present = 1,
    .limit_high = 0xF, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0,
  };

  cpu->gdt[4] = (struct i686_gdt_entry) {
    .limit_low = 0xFFFF, .base_low = 0, .base_mid = 0,
    .type = 0x2, .system = 1, .dpl = 3, .present = 1,
    .limit_high = 0xF, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0,
  };

  i686_cpu_set_gdt(cpu->gdt, 5);
}

static void 
i686_set_idt_entry(struct i686_idt_entry *e, void *entry) {
  *e = (struct i686_idt_entry) {
    .offset_low = (unsigned int)entry & 0xFFFF,
    .offset_high = (unsigned int)entry >> 16,
    .zeroes = 0,
    .type = 0xE,
    .segment = 0x8,
    .dpl = 0,
    .present = 1,
  };
}

static void i686_setup_idt(struct i686_cpu *cpu) {

  memset(cpu->idt, 0, sizeof(cpu->idt));

  i686_set_idt_entry(&cpu->idt[E_DE], i686_int_handler_0);
  i686_set_idt_entry(&cpu->idt[E_DB], i686_int_handler_1);
  i686_set_idt_entry(&cpu->idt[E_NMI], i686_int_handler_2);
  i686_set_idt_entry(&cpu->idt[E_BP], i686_int_handler_3);
  i686_set_idt_entry(&cpu->idt[E_OF], i686_int_handler_4);
  i686_set_idt_entry(&cpu->idt[E_BR], i686_int_handler_5);
  i686_set_idt_entry(&cpu->idt[E_UD], i686_int_handler_6);
  i686_set_idt_entry(&cpu->idt[E_NM], i686_int_handler_7);
  i686_set_idt_entry(&cpu->idt[E_DF], i686_int_handler_8);
  i686_set_idt_entry(&cpu->idt[E_CO], i686_int_handler_9);
  i686_set_idt_entry(&cpu->idt[E_TS], i686_int_handler_10);
  i686_set_idt_entry(&cpu->idt[E_NP], i686_int_handler_11);
  i686_set_idt_entry(&cpu->idt[E_SS], i686_int_handler_12);
  i686_set_idt_entry(&cpu->idt[E_GP], i686_int_handler_13);
  i686_set_idt_entry(&cpu->idt[E_PF], i686_int_handler_14);
  i686_set_idt_entry(&cpu->idt[E_RS], i686_int_handler_15);
  i686_set_idt_entry(&cpu->idt[E_MF], i686_int_handler_16);
  i686_set_idt_entry(&cpu->idt[E_AC], i686_int_handler_17);
  i686_set_idt_entry(&cpu->idt[E_MC], i686_int_handler_18);
  i686_set_idt_entry(&cpu->idt[E_XM], i686_int_handler_19);
  i686_set_idt_entry(&cpu->idt[E_VE], i686_int_handler_20);

  i686_cpu_set_idt(cpu->idt, 256);

}

static void i686_cpu_init(struct cpu *_cpu) {

  struct i686_cpu *cpu = (struct i686_cpu *)_cpu;
  
  feeder_physmem_create(&cpu->feeder, _cpu->k->phys, 1024, 1024);
  cpu->c.localmem = (struct physmem *)&cpu->feeder;

  i686_setup_gdt(cpu);
  i686_setup_idt(cpu);


}


static void i686_cpu_schedule(struct cpu *_cpu VAR_UNUSED) {


}


static struct i686_cpu i686cpu = {
  .c = {
    .model = "Lolwut i686",
    .v = {
      .init = i686_cpu_init,
      .schedule = i686_cpu_schedule,
    },
  },
};

struct i686_cpu *
i686_cpu_alloc(struct kernel *k) {
  
  i686cpu.c.k = k;
  i686cpu.c.allocator = &i686_kmem_allocator;
  return &i686cpu;

}

static physaddr_t i686_pagefault_getaddr() {
  physaddr_t a;
  __asm__("movl %%cr3, %%eax\n"
          "movl %%eax, %0" : "=r"(a)::"eax");
  return a;
}

static void i686_pagefault_decode(struct i686_pagefault_error *e, int code) {
  *((int *)e) = code;
}
 

static void i686_pagefault(struct i686_context *c) {
  struct i686_pagefault_error e;

  physaddr_t addr = i686_pagefault_getaddr();
  i686_pagefault_decode(&e, c->err_code);

  kernel()->debug("Page fault: P: %d W: %d  U: %d  R: %d,  IO: %d\n",
      e.present, e.write, e.usermode, e.reserved, e.io);
  kernel()->debug("            Address: %x\n", addr);
  while (1);
}

static struct i686_context *
i686_int_entry(struct i686_context *c) {
  if (c->int_no == E_PF) {
    i686_pagefault(c);
  }
  while (1);
  return c;
}


__asm__(
    "i686_int_handler: \n" 
    "  pusha    \n"
    "  push %ds \n"
    "  push %es \n"
    "  push %fs \n"
    "  push %gs \n"
    "  movw $0x10, %ax  \n"
    "  movw %ax, %ds    \n"
    "  movw %ax, %es    \n"
    "  movw %ax, %fs    \n"
    "  movw %ax, %gs    \n"
    "  movl %esp, %eax  \n"
    "  pushl %eax       \n"
    "  jmp i686_int_entry \n"
    "  jmp 0x0");

