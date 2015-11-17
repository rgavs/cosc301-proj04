/* thread user library functions */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)
#define LOOPS 1000000

int ppid;
lock_t xlock;
int global = 0;
int done = 0;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int main(int argc, char *argv[])
{
  int num_threads = 4;
  int i;
  ppid = getpid();
  lock_init(&xlock);

  for(i = 0; i < num_threads; i++) {
    int thread_pid = thread_create(worker, 0);
    assert(thread_pid > 0);
  }

  while (done < num_threads) { sleep(1); }

  for(i = 0; i < num_threads; i++) {
    int join_pid = thread_join(-1);
    assert(join_pid > 0);
  }

  printf(1, "Without locks updating global: %d\n", global);
  printf(1, "(Should be %d if no race condition\n", num_threads * LOOPS);

  printf(1, "TEST PASSED\n");
  exit();
}


void worker(void *arg_ptr) {
  int i;

  for(i = 0; i < LOOPS; i++) {
    global += 1;
  }

  lock_acquire(&xlock);
  done += 1;
  lock_release(&xlock);

  exit();
}
