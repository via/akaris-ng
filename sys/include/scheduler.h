#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "queue.h"
#include "thread.h"

struct scheduler;

typedef enum {
  SCHED_SUCCESS,
  SCHED_OOM,
  SCHED_INVALID,
  SCHED_NOTFOUND,
} scheduler_err_t;


struct scheduler_vfuncs {
  scheduler_err_t (*reschedule)(struct scheduler *);
  void (*resume)(struct scheduler *);

  scheduler_err_t (*get_current_thread)(struct scheduler *, struct thread **);

  scheduler_err_t (*thread_alloc)(struct scheduler *, struct thread **);
  scheduler_err_t (*thread_add)(struct scheduler *, struct thread *);
  scheduler_err_t (*thread_destroy)(struct scheduler *, struct thread *);
  scheduler_err_t (*thread_lookup_by_id)(struct scheduler *, struct thread **, 
      thread_id);

};

struct scheduler {
  struct scheduler_vfuncs v;

  LIST_HEAD(, thread) runnable;
  LIST_HEAD(, thread) waiting;

  struct thread *current;

};

inline static scheduler_err_t scheduler_reschedule(struct scheduler *s) {
  return s->v.reschedule(s);
}

inline static void scheduler_resume(struct scheduler *s) {
  return s->v.resume(s);
}

inline static scheduler_err_t scheduler_get_current_thread(struct scheduler *s,
    struct thread **t) {
  return s->v.get_current_thread(s, t);
}

inline static scheduler_err_t scheduler_thread_alloc(struct scheduler *s,
    struct thread **t) {
  return s->v.thread_alloc(s, t);
}

inline static scheduler_err_t scheduler_thread_add(struct scheduler *s,
    struct thread *t) {
  return s->v.thread_add(s, t);
}

inline static scheduler_err_t scheduler_thread_destroy(struct scheduler *s,
    struct thread *t) {
  return s->v.thread_destroy(s, t);
}

inline static scheduler_err_t scheduler_thread_lookup_by_id(struct scheduler *s,
    struct thread **t, thread_id tid) {
  return s->v.thread_lookup_by_id(s, t, tid);
}

#endif

