#include "types.h"
#include "assert.h"
#include "scheduler.h"



scheduler_err_t 
common_scheduler_reschedule(struct scheduler *s) {

  /* Move runnable threads from waiting to runnable */

  /* Prioritize runnable queue based on:
   *   messages waiting (message priority)
   *   recent cycle count
   *
   */

  s->current = LIST_FIRST(&s->runnable);

  return SCHED_SUCCESS;
}

scheduler_err_t 
common_scheduler_get_current_thread(struct scheduler *s,
    struct thread **t) {
  if (!s->current) {
    return SCHED_NOTFOUND;
  }
  *t = s->current;
  return SCHED_SUCCESS;
}


/* Replace with hash table? */
scheduler_err_t 
common_scheduler_thread_lookup_by_id(struct scheduler *s, 
    struct thread **t, thread_id tid) {

  struct thread *p;
  LIST_FOREACH(p, &s->runnable, threads)
    if (p->tid == tid) {
      *t = p;
      return SCHED_SUCCESS;
    }
  LIST_FOREACH(p, &s->waiting, threads)
    if (p->tid == tid) {
      *t = p;
      return SCHED_SUCCESS;
    }
      
  return SCHED_NOTFOUND;
}

scheduler_err_t
common_scheduler_thread_add(struct scheduler *s, struct thread *t) {
  assert(t != NULL);
  assert(t->space != NULL);

  switch (t->state) {
  case THREAD_RUNNABLE:
    LIST_INSERT_HEAD(&s->runnable, t, threads);
    break;
  case THREAD_SLEEPING:
  case THREAD_BLOCKED:
  case THREAD_STOPPED:
    LIST_INSERT_HEAD(&s->waiting, t, threads);
    break;
  case THREAD_INIT:
  default:
    return SCHED_INVALID;
  }

  return SCHED_SUCCESS;
}

