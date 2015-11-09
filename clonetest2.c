/* clone twice, verify everything looks reasonable */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

int ppid;
#define PGSIZE (4096)

volatile int global = 0;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

void allocstacks(void **start, void **stack, int threads) {
  // uint size = (unsigned int)sbrk(0);
  // printf(1, "heap size before malloc %d\n", size);
  *stack = malloc(PGSIZE*(threads+1));
  *start = *stack;
  assert(*stack != NULL);
  if((uint)(*stack) % PGSIZE)
    *stack = (void*)((char*)(*stack) + (PGSIZE - (uint)(*stack) % PGSIZE));
  // size = (unsigned int)sbrk(0);
  // printf(1, "heap size after malloc %d %x\n", size, *start);
}

int
main(int argc, char *argv[])
{
  ppid = getpid();
  int numthreads = 2;
  if (argc > 1) {
    numthreads = atoi(argv[1]);
  }
  printf(1, "Starting %d threads\n", numthreads);

  void *start, *stackoff;
  allocstacks(&start, &stackoff, numthreads);

  int *pids = malloc(sizeof(int)*numthreads);

  int tid;
  for (tid=0; tid < numthreads; tid++) {
    void *xstack = stackoff + PGSIZE*tid;
    int clone_pid = clone(worker, (void*)tid, xstack);
    assert(clone_pid > 0);
    pids[tid] = clone_pid;
  }

  sleep(1);
  while (global < numthreads) {}
  printf(1, "Threads done; now joining all\n");

  // join threads by id
  for (tid=0; tid < numthreads; tid++) {
    int jointhr = join(pids[tid]);
    assert(jointhr == pids[tid]);
  }
  int jointhr = join(-1);
  assert(jointhr == -1); // should return -1; no more threads to join

  printf(1, "TEST PASSED (mainthread)\n");

  free(start);
  free(pids);

  exit();
}

void worker(void *arg_ptr) {
  int tid = (int)(arg_ptr);
  printf(1, "Thread %d started\n", tid);
  while (global != tid) {}
  global += 1;
  printf(1, "Thread %d incremented the global counter\n", tid);
  sleep(1);
  exit();
}
