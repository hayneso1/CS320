#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pstat.h"
#include "syscall.h"
#include "param.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
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
  if (growproc(n) < 0)
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
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
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

uint64
sys_getfilenum(void)
{
  int pid;
  argint(0, &pid);

  return getfilenum(pid);
}

// sysproc.c
uint64
sys_settickets(int n)
{
  return settickets(n);
}

uint64
sys_getpinfo(struct pstat *ps)
{
  return getpinfo(ps);
}

int sys_pgaccess(void)
{
  int start_va;    // Input: Starting virtual address of the memory range to check for page access.
  int num_pages;   // Input: Number of pages to check for access.
  int bitmap_addr; // Input: Address of the bitmap to store access information.

  int success = 1;
  // Check if the arguments can be retrieved from user space.
  argint(0, &start_va);
  argint(1, &num_pages);
  argint(2, &bitmap_addr);

  // If any of the argint calls failed, set the success flag to 0.
  if (start_va < 0 || num_pages < 0 || bitmap_addr < 0)
  {
    success = 0;
  }

  if (success == 0)
  {
    return -1; // Return an error code to indicate an error.
  }

  int bitmap = 0; // Allocate a temporary buffer for the bitmap.

  for (int i = 0; i < num_pages; i++)
  {
    pte_t *pte = walk(myproc()->pagetable, start_va + i * PGSIZE, 0);
    if (!pte || !(*pte & PTE_A))
    {
      // Page not accessed or not present, update the bitmap accordingly.
    }
    else
    {
      // Page has been accessed, update the bitmap accordingly.
      bitmap = bitmap | (1 << i);
      // Clear the access bit in the page table.
      *pte &= ~PTE_A;
    }
  }
  // Copy the bitmap to the user address.
  either_copyout(1, (uint64)bitmap_addr, &bitmap, sizeof(int));

  return 0; // Return 0 to indicate success.
}
