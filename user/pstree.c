#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"

#define MAX_PROCESSES 64

struct procinfo processes[MAX_PROCESSES];

int show_pid = 0;
int show_memory = 0;

void
print_process(struct procinfo *p)
{
  printf("%s", p->name);

  if(show_pid)
    printf("(%d)", p->pid);

  if(show_memory)
    printf(" [%dB]", p->sz);
}

void
print_children(int parent_pid, int depth)
{
  int i;
  int j;

  for(i = 0; i < MAX_PROCESSES; i++) {
    if(!processes[i].active)
      continue;

    if(processes[i].ppid != parent_pid)
      continue;

    for(j = 0; j < depth; j++)
      printf("    ");

    printf("|- ");
    print_process(&processes[i]);
    printf("\n");

    print_children(processes[i].pid, depth + 1);
  }
}

int
main(int argc, char *argv[])
{
  int i;
  int root_found = 0;

  /*
   * Parse command-line arguments.
   *
   * Supported:
   *   pstree
   *   pstree -p
   *   pstree -m
   *   pstree -pm
   *   pstree -mp
   *   pstree -p -m
   */
  for(i = 1; i < argc; i++) {
    char *arg = argv[i];

    if(arg[0] != '-') {
      printf("Usage: pstree [-p] [-m]\n");
      exit(1);
    }

    for(int j = 1; arg[j] != '\0'; j++) {
      if(arg[j] == 'p') {
        show_pid = 1;
      } else if(arg[j] == 'm') {
        show_memory = 1;
      } else {
        printf("pstree: unknown option %c\n", arg[j]);
        printf("Usage: pstree [-p] [-m]\n");
        exit(1);
      }
    }
  }

  /*
   * Read every process-table entry from the kernel.
   */
  for(i = 0; i < MAX_PROCESSES; i++) {
    if(getprocinfo(i, &processes[i]) < 0) {
      processes[i].active = 0;
    }
  }

  /*
   * Find and print init, whose PID is 1.
   */
  for(i = 0; i < MAX_PROCESSES; i++) {
    if(processes[i].active && processes[i].pid == 1) {
      print_process(&processes[i]);
      printf("\n");

      print_children(processes[i].pid, 1);

      root_found = 1;
      break;
    }
  }

  if(!root_found)
    printf("pstree: init process not found\n");

  exit(0);
}
