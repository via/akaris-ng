#include "types.h"
#include "mutex.h"

static enum mutex_status i686_mutex_lock(struct mutex *mutex, const char *name) {

  uint32 oldval;
  __asm__("xchg %0, %1;" : "=m"(mutex->lock), "=r"(oldval) :
      "m"(mutex->lock), "1"(MUTEX_LOCKED));
  if (oldval == MUTEX_UNLOCKED) {
    mutex->blocker = name;
  }

  return (enum mutex_status)oldval;
}

struct mutex_vfuncs i686_mutex = {
  i686_mutex_lock,
  mutex_unlock,
  mutex_blocker,
  mutex_init,
};
