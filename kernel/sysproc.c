#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 ip;
  int n;
  int bitmask;
  // Set an upper limit on the number of pages that can be scanned.
  const int upper_limit = 32;

  argaddr(0, &ip);
  if(ip == -1)
    return -1;

  argint(1, &n);
  if(n == -1)
    return -1;

  argint(2, &bitmask);
  if(bitmask == -1)
    return -1;

  if(n > upper_limit || n < 0)
    return -1;
  
  int res = 0;
  struct proc *p = myproc();

  for(int i = 0; i < n; i++){
    int va = ip + i * PGSIZE;
    int t = vmpgaccess(p->pagetable, va);
    res = res | t << i;
  }

  if(copyout(p->pagetable, bitmask, (char*)&res, sizeof(res)) == -1)
    return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
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
