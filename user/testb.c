#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  int me = getpid();
  printf("active = %d\n", activecount());
  printf("size of me = %d\n", getprocsize(me));
  printf("size of pid 9999 = %d\n", getprocsize(9999));
  for(int i = 0; i < 3; i++){
    if(fork() == 0){
      for(volatile int j = 0; j < 10000000; j++);
      exit(0);
    }
  }
  printf("children before = %d\n", familyheadcount(me));
  for(int i = 0; i < 3; i++) wait(0);
  printf("children after  = %d\n", familyheadcount(me));
  exit(0);
}
