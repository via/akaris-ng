#include "scheduler.h"



scheduler_err_t 
common_scheduler_reschedule(struct scheduler *s) {

  /* Move runnable threads from waiting to runnable */

  /* Prioritize runnable queue based on:
   *   messages waiting (message priority)
   *   recent cycle count
   *
   */


  return SCHED_SUCCESS;
}

scheduler_err_t 
common_scheduler_get_current_thread(struct scheduler *s,
    struct thread **t) {
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
