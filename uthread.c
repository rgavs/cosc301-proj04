#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

/*
 * This is where you'll need to implement the user-level functions
 */
#define PGSIZE (4096)    // bytes mapped by a page (from mmu.c)
#define NPROC        64  // maximum number of processes (from param.h)
uint ustacks[NPROC];

void lock_init(lock_t *lock) {
    lock->locked = 0;
}

void lock_acquire(lock_t *lock) {
    while(xchg(&lock->locked, 1) != 0)
    ;
}

void lock_release(lock_t *lock) {
  xchg(&lock->locked, 0);
}

int
thread_join(int pid) {
    pid = join(pid);
    if(pid > 0 && pid < NPROC)
        free((void *)ustacks[pid - 1]);
    return pid;
}

int thread_create(void (*start_routine)(void *), void *arg) {
    void *newstack = malloc(PGSIZE);
    if((uint)newstack % PGSIZE)
        newstack = newstack + (PGSIZE - (uint)newstack % PGSIZE);

    int clone_pid = clone(start_routine, arg, newstack);
    if(clone_pid > 0 && clone_pid < NPROC)
        ustacks[clone_pid - 1] = (uint)newstack;

    exit();
    return clone_pid;
}
