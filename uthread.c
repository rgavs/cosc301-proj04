#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "proc.c"

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
thread_join(int pid) {
    if(proc->thread == 1)
        return -1;
    struct proc *p;
    int havekids;

    acquire(&ptable.lock);
    for(;;){
        // Scan through table looking child
        havekids = 0;
        for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
            if(p->pid == pid){
                if(p->parent != proc || p->thread != 0)
                    return -1;
                else
                    continue;
            }
            havekids = 1;
            if(p->state == ZOMBIE){
              // Found one.
              pid = p->pid;
              kfree(p->kstack);
              p->kstack = 0;
              freevm(p->pgdir);
              p->state = UNUSED;
              p->pid = 0;
              p->parent = 0;
              p->name[0] = 0;
              p->killed = 0;
              release(&ptable.lock);
              return pid;
          }
      }
      /*/ No point waiting if we don't have any children.
      if(!havekids || proc->killed){
        release(&ptable.lock);
        return -1;
    }*/

      // Wait for children to exit.  (See wakeup1 call in proc_exit.)
      sleep(proc, &ptable.lock);  //DOC: wait-sleep
    }
    return 0;
}

int thread_create(void (*start_routine)(void *), void *arg) {
    return 0;
}
