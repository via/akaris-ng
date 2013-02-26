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
  E_DB = 1,
  E_NMI = 2,
  E_BP = 3,
  E_OF = 4,
  E_BR = 5,
  E_UD = 6,
  E_NM = 7,
  E_DF = 8,
  E_CO = 9,
  E_TS = 10,
  E_NP = 11,
  E_SS = 12,
  E_GP = 13,
  E_PF = 14,
  E_RS = 15,
  E_MF = 16,
  E_AC = 17,
  E_MC = 18,
  E_XM = 19,
  E_VE = 20,
} i686_exception;

struct i686_cpu {
  struct cpu c;
  struct feeder_physmem feeder;
  struct i686_gdt_entry gdt[5] __attribute__((aligned(4)));
  struct i686_idt_entry idt[256] __attribute__((aligned(4)));


  /* CPUID, GDT */
};


struct i686_context {
  uint32 gs, fs, es, ds;
  uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32 int_no, err_code;
  uint32 eip, cs, eflags, useresp, ss;
} __attribute__((__packed__));

struct i686_pagefault_error {
  unsigned int present : 1;
  unsigned int write : 1;
  unsigned int usermode : 1;
  unsigned int reserved : 1;
  unsigned int io : 1;
  unsigned int _reserved : 27;
} __attribute__((__packed__));


struct i686_cpu *i686_cpu_alloc(struct kernel *);

#define I686_INT_HANDLER_WERR(INTNUM, DEST) \
    extern void i686_int_handler_ ## INTNUM (); \
    __asm__("i686_int_handler_"#INTNUM":\n" \
    "  pushl $"#INTNUM"\n" \
    "  jmp "#DEST);

#define I686_INT_HANDLER_NOERR(INTNUM, DEST) \
    extern void i686_int_handler_ ## INTNUM (); \
    __asm__("i686_int_handler_"#INTNUM":\n" \
    "  pushl $0\n" \
    "  pushl $"#INTNUM"\n" \
    "  jmp "#DEST);



#endif
