#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64 sys_exit(void) {
  int n;
  if (argint(0, &n) < 0) return -1;
  exit(n);
  return 0;  // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return fork(); }

uint64 sys_wait(void) {
  uint64 p;
  int flags;
  if (argaddr(0, &p) < 0) return -1;
  if (argint(1,&flags) < 0) return -1;
  return wait(p,flags);
}

uint64 sys_sbrk(void) {
  int addr;
  int n;

  if (argint(0, &n) < 0) return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0) return -1;
  return addr;
}

uint64 sys_sleep(void) {
  int n;
  uint ticks0;

  if (argint(0, &n) < 0) return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (myproc()->killed) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;

  if (argint(0, &pid) < 0) return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_rename(void) {
  char name[16];
  int len = argstr(0, name, MAXPATH);
  if (len < 0) {
    return -1;
  }
  struct proc *p = myproc();
  memmove(p->name, name, len);
  p->name[len] = '\0';
  return 0;
}

//modify begin
//modify end
extern struct proc proc[NPROC];
uint64 sys_yield(void) {
  struct proc *p = myproc();

  printf("Save the context of the process to the memory region from address %p to %p\n",
    (uint64)&p->context,(uint64)(&p->context +1));

  printf("Current running process pid is %d and user pc is %p\n",
    p->pid, (uint64)p->trapframe->epc);

  int start = p-proc;
  int next_pid=-1;
  uint64 next_pc=0;
  for(int i=1;i<NPROC;i++){
    int idx = (start+i)%NPROC;
    struct proc *np = &proc[idx];
    acquire(&np->lock);
    if(np->state == RUNNABLE){
      next_pid = np->pid;
      next_pc = np->trapframe->epc;
      release(&np->lock);
      break;
    }
    release(&np->lock);
  }
  printf("Next runnable process pid is %d and user pc is %p\n",
    next_pid,next_pc);
  yield();
  return 0;
}

uint64 sys_seccomp_ctl(void){
  int op;
  uint64 arg;
  struct proc *p = myproc();

  argint(0,&op);
  argaddr(1,&arg);
  if(op==0){
    p->seccomp_mask = arg;
    return 0;
  }else if(op == 1){
    if(arg>0x7fffffffUL) return -1;
    p->max_children = (int) arg;
    return 0;
  }
  return -1;
}

uint64 sys_seccomp_getlog(void){
  uint64 buf_addr;
  uint64 len_addr;
  int len;
  struct proc *p = myproc();
  argaddr(0,&buf_addr);
  argaddr(1,&len_addr);

  if(copyin(p->pagetable,(char *)&len,len_addr,sizeof(len))<0)
    return -1;
  
  if(len < 0) return -1;
  if(len > p->seccomp_log_count)
    len = p->seccomp_log_count;

  if(len>0 && copyout(p->pagetable,buf_addr,(char *)p->seccomp_log,len*sizeof(uint64))<0)
    return -1;
  if(copyout(p->pagetable,len_addr,(char *)&len,sizeof(len))<0)
    return -1;

  return 0;
}