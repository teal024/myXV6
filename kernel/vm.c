void
usertrap(void)
{
  ...
  } else {
    uint64 cause = r_scause();
    if(cause == 13 || cause == 15){
      // page fault
      uint64 stval = r_stval();
      if(stval < p->sz){
        // need lazy allocation
        char *mem = kalloc();
        if(mem){
          memset(mem, 0, PGSIZE);
          if(mappages(p->pagetable, PGROUNDDOWN(stval), PGSIZE, (uint64)mem, PTE_W|PTE_X|PTE_R|PTE_U) != 0){
            kfree(mem);
            uvmunmap(p->pagetable, PGROUNDDOWN(stval), 1, 1);
          } else goto brk;
        }
      }
    }

    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }
brk:
