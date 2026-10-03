#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid;
  int count;

  pid = getpid();

  printf("Testing lineage for PID %d\n", pid);

  count = lineage(pid);

  printf("Lineage count: %d\n", count);

  if (count > 0)
    printf("Q3 test passed\n");
  else
    printf("Q3 test failed\n");

  exit(0);
}
