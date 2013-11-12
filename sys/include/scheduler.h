#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "queue.h"
#include "thread.h"

struct scheduler;

typedef enum {
  SCHED_SUCCESS,
  SCHED_OOM,
} scheduler_err_t;


struct scheduler_vfuncs {
  scheduler_err_t (*reschedule)(struct scheduler *);
  scheduler_err_t (*resume_thread)(struct scheduler *);

  scheduler_err_t (*get_current_thread)(struct scheduler *, struct thread **);
  scheduler_err_t (*set_current_thread)(struct scheduler *, struct thread *);

  scheduler_err_t (*thread_alloc)(struct scheduler *, struct thread **);
  scheduler_err_t (*thread_add)(struct scheduler, struct thread *);
  scheduler_err_t (*thread_destroy)(struct scheduler, struct thread *);
  scheduler_err_t (*thread_lookup_by_id)(struct scheduler *, struct thread **, 
      thread_id);

};

struct scheduler {
  LIST_HEAD(, thread) runnable;
  LIST_HEAD(, thread) blocked;
  LIST_HEAD(, thread) other;

};

#endif

