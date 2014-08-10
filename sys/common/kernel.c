#include "address_space.h"
#include "cpu.h"
#include "elf.h"
#include "kernel.h"
#include "scheduler.h"
#include "thread.h"

struct thread *
spawn(const void *elf) {
  struct thread *t;
  struct address_space *as;
  struct memory_region *mr;
  struct elf_context ctx;
  struct elf_section *s;
  elf_error_t e;

  if (elf_init(&ctx, elf) != ELF_SUCCESS) {
    return NULL;
  }

  address_space_alloc(&as);
  virtmem_user_setup_kernelspace(kernel()->bsp->kvirt, as->pd);
  /* Actually load the address space */
  while ( (e = elf_iterate(&ctx)) != ELF_END) {
    if (e == ELF_UNSUPPORTED) {
      continue;
    }
    s = elf_section(&ctx);
    memory_region_alloc(&mr);
    if (address_space_init_region(as, mr, s->memaddr,
        s->elf_len) != AS_SUCCESS) {
      return NULL;
    }
    if (memory_region_set_flags(mr, s->writable, 
        s->executable) != AS_SUCCESS) {
      return NULL;
    }
    if (memory_region_map(as, mr, NULL) != AS_SUCCESS) {
      return NULL;
    }
    virtmem_copy_kernel_to_user(kernel()->bsp->kvirt, as->pd, s->memaddr, 
        s->elf_start, s->elf_len);
  }


  scheduler_thread_alloc(cpu()->sched, &t);
  thread_init(t, as);
  t->state = THREAD_RUNNABLE;
  scheduler_thread_add(cpu()->sched, t);

  return t;

}

