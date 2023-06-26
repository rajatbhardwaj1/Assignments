#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int random(int lower, int upper){
  int num = (((ticks*ticks)-ticks) % (upper - lower + 1)) + lower;
  return num;
}

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[MAXARG+1],stackbase;
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();
  // int instr;

  begin_op();

  int aslr_flag = 0;
  char c[2] = {0};

  if ((ip = namei("aslr_flag")) == 0) {
    cprintf("unable to open aslr_flag file, default to no randomize\n");
  } else {
    ilock(ip);
    if (readi(ip, (char*)&c, 0, sizeof(char)) != sizeof(char)) {
      cprintf("unable to read aslr_flag flag, default to no randomize\n");
    } else {
      aslr_flag = (c[0] == '1')? 1 : 0;
    }
    iunlockput(ip);
  }

  if((ip = namei(path)) == 0){
    end_op();
    cprintf("exec: fail\n");
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;


  sz = 0;

  int offset = (aslr_flag)? random(0, 1000) * 16 + 1 : 0;

  // Load program into memory.
  sz = allocuvm(pgdir,0,offset);
  
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph)){
      goto bad;

    }
    if(ph.type != ELF_PROG_LOAD)
      continue;

    if(ph.memsz < ph.filesz){
      goto bad;
    }

    if(ph.vaddr + ph.memsz < ph.vaddr){
      goto bad;

    }

    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz+offset)) == 0){
      goto bad;
    }

    // if(ph.vaddr % PGSIZE != 0)
    //   goto bad;
    if(loaduvm(pgdir, (int)ph.vaddr+offset, ip, ph.off, ph.filesz) < 0){
      goto bad;
    }


  }

  iunlockput(ip);
  end_op();
  ip = 0;

  curproc = myproc();

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.

  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)sz - 2*PGSIZE);
  sp = sz;
  stackbase = sp - PGSIZE;

  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    // sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    if(sp<stackbase)
      goto bad;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  sp -= (argc+1) * sizeof(int);
  sp -= sp % 16;
  if(sp<stackbase)
    goto bad;
  if(copyout(pgdir, sp, (char*)ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry+offset;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}