#include "types.h"
#include "mutex.h"

void mutex_unlock(struct mutex *mutex) {
  
  mutex->lock = 0;
  mutex->blocker = "";

}

const char *mutex_blocker(const struct mutex *mutex) {
  return mutex->blocker;
}

void mutex_init(struct mutex *mutex) {
  mutex->lock = 0;
  mutex->blocker = "";
}
