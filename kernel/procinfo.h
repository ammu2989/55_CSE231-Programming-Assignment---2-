#ifndef PROCINFO_H
#define PROCINFO_H

struct procinfo {
  int active;
  int pid;
  int ppid;
  int sz;
  char name[16];
};

#endif


