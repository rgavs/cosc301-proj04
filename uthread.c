#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

/*
 * This is where you'll need to implement the user-level functions
 */

void lock_init(lock_t *lock) {
}

void lock_acquire(lock_t *lock) {
}

void lock_release(lock_t *lock) {
}

int
thread_join(int pid)
{
    return join(pid);
}

int thread_create(void (*start_routine)(void *), void *arg) {
    return 0;
}
