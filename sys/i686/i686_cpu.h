#ifndef SYS_I686_CPU_H
#define SYS_I686_CPU_H

#include "kernel.h"
#include "thread.h"
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

struct i686_tss {
  uint16 link;
  uint16 link_h;

  uint32 esp0;
  uint16 ss0;
  uint16 ss0_h;

  uint32 esp1;
  uint16 ss1;
  uint16 ss1_h;

  uint32 esp2;
  uint16 ss2;
  uint16 ss2_h;

  uint32 cr3;
  uint32 eip;
  uint32 eflags;

  uint32 eax;
  uint32 ecx;
  uint32 edx;
  uint32 ebx;

  uint32 esp;
  uint32 ebp;

  uint32 esi;
  uint32 edi;

  uint16 es;
  uint16 es_h;

  uint16 cs;
  uint16 cs_h;

  uint16 ss;
  uint16 ss_h;

  uint16 ds;
  uint16 ds_h;

  uint16 fs;
  uint16 fs_h;

  uint16 gs;
  uint16 gs_h;

  uint16 ldt;
  uint16 ldt_h;

  uint16 trap;
  uint16 iomap;
} __attribute__((__packed__));

struct i686_cpu {
  struct cpu c;
  struct feeder_physmem feeder;
  struct i686_gdt_entry gdt[6] __attribute__((aligned(4)));
  struct i686_idt_entry idt[256] __attribute__((aligned(4)));
  struct i686_tss tss __attribute__((aligned(4)));
  struct scheduler sched;
  long stack[1024];

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

struct i686_thread {
  struct thread t;
  struct i686_context ctx;
};

struct i686_cpu *i686_cpu_alloc();

extern void i686_userspace_return(struct i686_context *);

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
