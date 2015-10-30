/* clone with various bad arguments; all should fail */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;

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
   if((uint)stack % PGSIZE == 0) // make stack unaligned
     stack = (void*)((char*)stack - 100);

   assert(clone(worker, 0, stack) == -1);
   assert(clone(0, 0, 0) == -1);
   assert(clone(worker, 0, 0) == -1);

   printf(1, "TEST PASSED\n");
   exit();
}

void worker(void *arg_ptr) {
   printf(1, "TEST FAILED! should never get here!\n");
   exit();
}
