#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

// Q2: count processes whose state is not UNUSED
uint64
sys_activecount(void)
{
  struct proc *p;
  int count = 0;

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->state != UNUSED)
      count++;
    release(&p->lock);
  }
  return count;
}

// Q4: size in bytes of process `pid`, or -1 if absent/UNUSED
uint64
sys_getprocsize(void)
{
  int pid;
  struct proc *p;

  argint(0, &pid);

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid && p->state != UNUSED){
      int sz = p->sz;          // read BEFORE releasing the lock
      release(&p->lock);
      return sz;
    }
    release(&p->lock);
  }
  return -1;
}

// Q5: caller's children that are neither UNUSED nor ZOMBIE
uint64
sys_familyheadcount(void)
{
  struct proc *me = myproc();
  struct proc *p;
  int count = 0;

  acquire(&wait_lock);                // protects p->parent
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->parent == me){
      acquire(&p->lock);              // protects p->state
      if(p->state != UNUSED && p->state != ZOMBIE)
        count++;
      release(&p->lock);
    }
  }
  release(&wait_lock);
  return count;
}
