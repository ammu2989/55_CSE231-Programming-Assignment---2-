#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int first;
  int second;

  first = getuptime();

  for (volatile int i = 0; i < 1000000; i++)
    ;

  second = getuptime();

  printf("First uptime: %d ticks\n", first);
  printf("Second uptime: %d ticks\n", second);

  if (second >= first)
    printf("Q1 test passed\n");
  else
    printf("Q1 test failed\n");

  exit(0);
}
