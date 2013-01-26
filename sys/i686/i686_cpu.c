#include "kernel.h"
#include "physical_memory.h"
#include "i686_cpu.h"
#include "slab.h"
#include "i686_slab.h"

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

static void i686_setup_idt(struct i686_cpu *cpu) {

  cpu->idt[0] = (struct i686_segment_entry) {
    .limit_low = 0, .base_low = 0, .base_mid = 0,
    .type = 0x0, .system = 1, .dpl = 0, .present = 1,
    .limit_high = 0, .avl = 0, .reserved = 0,
    .op_size = 1, .granularity = 1, .base_high = 0
  };

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

