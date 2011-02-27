#ifndef INCLUDE_MUTEX_H
#define INCLUDE_MUTEX_H

struct mutex;

enum mutex_status {
  MUTEX_UNLOCKED,
  MUTEX_LOCKED,
};

struct mutex_vfuncs {
  enum mutex_status (*mutex_lock)(struct mutex *, const char *name); 
  void (*mutex_unlock)(struct mutex *);
  const char * (*mutex_blocker)(struct mutex *);
};


struct mutex {
  uint32 lock;
  const char *blocker;
};

/* Arch specific implementations */
extern struct mutex_vfuncs i686_mutex;

/*Base class functions */
void mutex_unlock(struct mutex *);
const char *mutex_blocker(struct mutex*);

  

#endif
