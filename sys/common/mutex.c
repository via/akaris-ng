#include "types.h"
#include "mutex.h"

void mutex_unlock(struct mutex *mutex) {
  
  mutex->lock = 0;

}

const char *mutex_blocker(struct mutex *mutex) {
  return mutex->blocker;
}

