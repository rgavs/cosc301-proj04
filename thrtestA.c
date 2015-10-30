/* thread user library functions */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int num_threads = 8;
int global = 0;
lock_t xlock;

#define assert(x) if (x) {} else { \
  printf(1, "%s: %d ", __FILE__, __LINE__); \
  printf(1, "assert failed (%s)\n", # x); \
  printf(1, "TEST FAILED\n"); \
  kill(ppid); \
  exit(); \
}

void worker(void *arg_ptr);
void spawner(void *arg_ptr);

int main(int argc, char *argv[])
{
  int i;
  ppid = getpid();
  lock_init(&xlock);

  int arg = num_threads / 2;

  int thread_pid = thread_create(spawner, (void *)arg);
  printf(1, "thread_pid %d\n", thread_pid);
  assert(thread_pid > 0);

  for(i = 0; i < arg; i++) {
    int thread_pid = thread_create(worker, (void *)arg);
    printf(1, "i: %d thread_pid %d\n", i, thread_pid);
    assert(thread_pid > 0);
  }
  printf(1, "thread creation completed (wait for 10 sec)\n");
  while (global < (num_threads+1)) {}
  printf(1, "joining threads\n");

  int join_pid = 0;
  int join_count = 0;
  while (join_pid != -1) {
    join_pid = thread_join(-1);
    if (join_pid != -1) {
      join_count += 1;
    }
  }
  printf(1, "join_count %d\n", join_count);

  printf(1, "TEST PASSED\n");
  exit();
}

void spawner(void *arg_ptr) {
  int arg = (int)arg_ptr;
  int i;

  for(i = 0; i < arg; i++) {
    int thread_pid = thread_create(worker, &arg);
    assert(thread_pid > 0);
  }
  lock_acquire(&xlock);
  global += 1;
  lock_release(&xlock);

  exit();
}

void worker(void *arg_ptr) {
  int i;
  int tmp = 0;

  for(i = 0; i < 10000; i++) { tmp++; }
  sleep(10);
  lock_acquire(&xlock);
  global += 1;
  lock_release(&xlock);
  exit();
}
