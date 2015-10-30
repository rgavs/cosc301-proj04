/* clone and change the address space size mid-ship */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

int ppid;
#define PGSIZE (4096)

volatile int global = 1;
volatile int *globalptr = 0;

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
  ppid = getpid();
  void *stack = malloc(PGSIZE*2);
  assert(stack != NULL);
  if((uint)stack % PGSIZE)
    stack = stack + (PGSIZE - (uint)stack % PGSIZE);

  int clone_pid = clone(worker, 0, stack);
  assert(clone_pid > 0);
  while(global != 5);

  // check whether array allocated in thread, but globally
  // accessible has all the expected contents as seen back
  // in the main process.
  int i = 0;
  int match = 1;
  for (; i < 100; i++) {
    if (globalptr[i] != i) {
      match = 0;
    }
  }
  assert(match == 1);

  printf(1, "TEST PASSED\n");
  exit();
}

void worker(void *arg_ptr) {
  assert(global == 1);
  global = 5;
  globalptr = (int*)malloc(100*sizeof(int));
  int i = 0;
  for ( ; i < 100; i++) {
     globalptr[i] = i;
  }
  exit();
}

