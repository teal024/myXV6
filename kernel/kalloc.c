// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);     // 将页面全都添加到空闲列表中, PHYSTOP表示上限
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start); // PGROUNDUP用来表示向上进位到某个页面
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;    // 将起始地址转换为指针，将一个地址转化为指向run结构体的指针

  // 举个例子
// 0x1000: [struct run {next=0x2000}]
// 0x2000: [struct run {next=0x3000}]
// 0x3000: [struct run {next=0x4000}]
// 0x4000: [struct run {next=NULL}] //这是链表的最后一个元素，所以next指针为NULL

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;    // update the freelist pointer to point to the next free memory page
  release(&kmem.lock);
  // then we fill the original freelist
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

uint64
get_free_physical_memory()   // 查看的是物理内存有多少页
{
  struct run* fl = kmem.freelist;
  uint64 pages = 0;
  acquire(&kmem.lock);    // 在这一个瞬间获取，其他进程不要修改
  while(fl)
  {
    pages++;
    fl = fl->next;
  }
  release(&kmem.lock);
  return pages * PGSIZE;
}