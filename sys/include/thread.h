#ifndef _THREAD_H
#define _THREAD_H

#include "queue.h"


typedef unsigned long thread_id;
struct cpu;
struct thread;
struct address_space;

typedef enum {
  THREAD_INIT,
  THREAD_RUNNABLE,
  THREAD_SLEEPING,
  THREAD_BLOCKED,
  THREAD_STOPPED,
} thread_state;

typedef enum {
  THREAD_SUCCESS,
} thread_err_t;

struct thread_vfuncs {
  thread_err_t (*init)(struct thread *, struct address_space *);
  thread_err_t (*clone)(struct thread *, struct thread *);
};

struct thread {
  struct thread_vfuncs v;

  LIST_ENTRY(thread) threads;
  struct address_space *space;
  struct cpu *cpu;
  thread_state state;
  thread_id tid;

  /* Blocked on what? */

  /* Sleeping till when? */

};

thread_err_t common_thread_init(struct thread *, struct address_space *);
thread_err_t common_thread_clone(struct thread *, struct thread *);

inline static thread_err_t
thread_init(struct thread *t, struct address_space *s) {
  return t->v.init(t, s);
}

inline static thread_err_t
thread_clone(struct thread *t, struct thread *s) {
  return t->v.clone(t, s);
}


#endif

