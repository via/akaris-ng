#include "assert.h"
#include "cpu.h"
#include "address_space.h"
#include "thread.h"

static thread_id next_tid = 1;

thread_err_t common_thread_init(struct thread *t, struct address_space *as) {
  t->cpu = cpu();
  t->tid = next_tid++; /*TODO, more atomicity */
  t->state = THREAD_INIT; 
  t->space = as;
  return THREAD_SUCCESS;
}

thread_err_t common_thread_clone(struct thread *t VAR_UNUSED, 
    struct thread *src VAR_UNUSED) {
  assert(0); /* Not implemented yet */

  return THREAD_SUCCESS;
}

