#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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

uint64
sys_trace(void)
{
  int n;
  // 获取追踪的mask, 从陷阱帧中获取用户程序传递的第一个参数
  // 这些这些参数是由用户程序通过寄存器传递给内核的, 0表示从a0寄存器中读取
  if(argint(0, &n) < 0)
    return -1;
  // 将mask保存在本进程的proc中
  myproc()->TraceMask = n;
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  info.freemem = get_free_physical_memory();
  info.nproc = get_proc_num();
  
  uint64 infoAddr;   // 获取用户空间参数，即要获取的info的地址
  if(argaddr(0, &infoAddr) < 0)
    return -1;

  struct proc *p = myproc();    // 获取用户空间中正在执行代码的进程，即要获取sysinfo的进程
  if (copyout(p->pagetable, infoAddr, (char *)&info, sizeof(info)) < 0)    // 拷贝到该进程页表中对应地址处
    return -1;

  return 0;
}