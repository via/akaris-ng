#include "types.h"
#include "mutex.h"

static enum mutex_status i686_mutex_lock(struct mutex *mutex, const char *name) {

  return MUTEX_UNLOCKED;

}

struct mutex_vfuncs i686_mutex = {
  i686_mutex_lock,
  mutex_unlock,
  mutex_blocker
};
