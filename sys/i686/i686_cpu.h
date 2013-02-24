#ifndef SYS_I686_CPU_H
#define SYS_I686_CPU_H

#include "kernel.h"
#include "physical_memory.h"
#include "cpu.h"

struct i686_gdt_entry {
  uint16 limit_low;
  uint16 base_low;
  uint8 base_mid;
  unsigned int type : 4;
  unsigned int system : 1;
  unsigned int dpl : 2;
  unsigned int present : 1;
  unsigned int limit_high : 4;
  unsigned int avl : 1;
  unsigned int reserved : 1;
  unsigned int op_size : 1;
  unsigned int granularity : 1;
  uint8 base_high;
} __attribute__((packed));


struct i686_idt_entry {
  uint16 offset_low;
  uint16 segment;
  unsigned int reserved : 5;
  unsigned int zeroes : 3;
  unsigned int type : 5;
  unsigned int dpl : 2;
  unsigned int present : 1;
  uint16 offset_high;
} __attribute__((packed));
  
typedef enum {
  E_DE = 0,
  E_PF = 14,
} i686_exception;

struct i686_cpu {
  struct cpu c;
  struct feeder_physmem feeder;
  struct i686_gdt_entry gdt[5] __attribute__((aligned(4)));
  struct i686_idt_entry idt[256] __attribute__((aligned(4)));


  /* CPUID, GDT */
};



struct i686_cpu *i686_cpu_alloc(struct kernel *);

#define I686_INT_HANDLER_WERR(INTNUM, DEST) __asm__( \
    "i686_int_handler_"#INTNUM":\n" \
    "  pushl $"#INTNUM"\n" \
    "  jmp "#DEST);

#define I686_INT_HANDLER_NOERR(INTNUM, DEST) __asm__( \
    "i686_int_handler_"#INTNUM":\n" \
    "  pushl $0\n" \
    "  pushl $"#INTNUM"\n" \
    "  jmp "#DEST);

void i686_int_handler_14(void);


#endif
