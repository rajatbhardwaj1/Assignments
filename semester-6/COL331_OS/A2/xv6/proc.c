#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

float lielay[65] = {100000, 1.0000000000, 0.8284271247, 0.7797631497, 0.7568284600, 0.7434917750, 0.7347722899, 0.7286265957, 0.7240618613, 0.7205376500, 0.7177346254, 0.7154519838, 0.7135571323, 0.7119589943, 0.7105929411, 0.7094118423, 0.7083805188, 0.7074721811, 0.7066660686, 0.7059458445, 0.7052984768, 0.7047134431, 0.7041821542, 0.7036975293, 0.7032536794, 0.7028456664, 0.7024693184, 0.7021210871, 0.7017979350, 0.7014972472, 0.7012167599, 0.7009545036, 0.7007087569, 0.7004780084, 0.7002609257, 0.7000563297, 0.6998631733, 0.6996805230, 0.6995075444, 0.6993434892, 0.6991876841, 0.6990395222, 0.6988984545, 0.6987639833, 0.6986356566, 0.6985130627, 0.6983958257, 0.6982836021, 0.6981760770, 0.6980729614, 0.6979739895, 0.6978789165, 0.6977875163, 0.6976995803, 0.6976149151, 0.6975333417, 0.6974546935, 0.6973788162, 0.6973055657, 0.6972348081, 0.6971664181, 0.6971002789, 0.6970362812, 0.6969743227, 0.6969143073};

struct
{
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

// for(int i=0;i<NPROC;i++){
// p_state.sched_policy[i]=-1;}

static void wakeup1(void *chan);

void pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int cpuid()
{
  return mycpu() - cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu *
mycpu(void)
{
  int apicid, i;

  if (readeflags() & FL_IF)
    panic("mycpu called with interrupts enabled\n");

  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i)
  {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc *
myproc(void)
{
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

// PAGEBREAK: 32
//  Look in the process table for an UNUSED proc.
//  If found, change state to EMBRYO and initialize
//  state required to run in the kernel.
//  Otherwise return 0.
static struct proc *
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  // added
  //  p->elapsed_time = 0;
  //  p->sched_policy = -1 ;
  //  p->execution_time = 1000000000 ;

  release(&ptable.lock);

  // Allocate kernel stack.
  if ((p->kstack = kalloc()) == 0)
  {
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe *)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint *)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context *)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

// PAGEBREAK: 32
//  Set up first user process.
void userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();

  initproc = p;
  if ((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0; // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if (n > 0)
  {
    if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  else if (n < 0)
  {
    if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if ((np = allocproc()) == 0)
  {
    return -1;
  }

  // Copy process state from proc.
  if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0)
  {
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for (i = 0; i < NOFILE; i++)
    if (curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if (curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for (fd = 0; fd < NOFILE; fd++)
  {
    if (curproc->ofile[fd])
    {
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->parent == curproc)
    {
      p->parent = initproc;
      if (p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();

  acquire(&ptable.lock);
  for (;;)
  {
    // Scan through table looking for exited children.
    havekids = 0;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->parent != curproc)
        continue;
      havekids = 1;
      if (p->state == ZOMBIE)
      {
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if (!havekids || curproc->killed)
    {
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock); // DOC: wait-sleep
  }
}

void init_procs()
{
  struct proc *p;
  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    p->elapsed_time = 0;
    // p->execution_time = 1000 ;
    p->sched_policy = -1;
  }
  release(&ptable.lock);
}
// PAGEBREAK: 42
//  Per-CPU process scheduler.
//  Each CPU calls scheduler() after setting itself up.
//  Scheduler never returns.  It loops, doing:
//   - choose a process to run
//   - swtch to start running that process
//   - eventually that process transfers control
//       via swtch back to the scheduler.

// void
// scheduler(void)
// {
//   struct proc *p;
//   struct cpu *c = mycpu();
//   c->proc = 0;

//   for(;;){
//     // Enable interrupts on this processor.
//     sti();

//     // Loop over process table looking for process to run.
//     acquire(&ptable.lock);
//     for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
//       if(p->state != RUNNABLE)
//         continue;

//       // Switch to chosen process.  It is the process's job
//       // to release ptable.lock and then reacquire it
//       // before jumping back to us.
//       c->proc = p;
//       switchuvm(p);
//       p->state = RUNNING;

//       swtch(&(c->scheduler), p->context);
//       switchkvm();

//       // Process is done running for now.
//       // It should have changed its p->state before coming back.
//       c->proc = 0;
//     }
//     release(&ptable.lock);

//   }
// }

void scheduler(void)
{

  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  for (;;)
  {

    sti();
    int policy = -1;
    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->state == RUNNABLE && (p->sched_policy == 0 || p->sched_policy == 1))
      {
        policy = p->sched_policy;
        break;
      }
    }
    release(&ptable.lock);

    if (policy == 0)
    {
      int earliest_dl = 0, earliest_dl_pid = 0;
      struct proc *hp_process = myproc();
      acquire(&ptable.lock);

      for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if (p->state == RUNNABLE)
        {
          earliest_dl = p->deadline + p->arrival_time;

          earliest_dl_pid = p->pid;
          hp_process = p;
          break;
        }
      }
      release(&ptable.lock);
      acquire(&ptable.lock);

      for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {

        if (p->state != RUNNABLE)
          continue;

        if (p->deadline <= earliest_dl)
        {
          if ((p->deadline + p->arrival_time < earliest_dl) || (p->deadline + p->arrival_time == earliest_dl && p->pid < earliest_dl_pid))
          {
            earliest_dl = p->deadline + p->arrival_time;
            earliest_dl_pid = p->pid;
            hp_process = p;
          }
        }
      }

      c->proc = hp_process;
      switchuvm(hp_process);
      hp_process->state = RUNNING;

      swtch(&(c->scheduler), hp_process->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
      release(&ptable.lock);
    }

    else if (policy == 1)
    {

      int min_w = 0, min_w_pid = 0;

      struct proc *hp_process = myproc();
      acquire(&ptable.lock);

      for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if (p->state == RUNNABLE)
        {
          min_w = p->weight;
          min_w_pid = p->pid;
          hp_process = p;
          break;
        }
      }

      for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {

        if (p->state != RUNNABLE)
          continue;

        if (p->weight <= min_w)
        {
          if (p->weight < min_w || (p->weight == min_w && p->pid < min_w_pid))
          {
            min_w = p->weight;
            min_w_pid = p->pid;
            hp_process = p;
          }
        }
      }

      c->proc = hp_process;
      switchuvm(hp_process);
      hp_process->state = RUNNING;

      swtch(&(c->scheduler), hp_process->context);
      switchkvm();

      c->proc = 0;
      release(&ptable.lock);
    }

    else
    {

      acquire(&ptable.lock);
      for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      {
        if (p->state != RUNNABLE)
          continue;

        // Switch to chosen process.  It is the process's job
        // to release ptable.lock and then reacquire it
        // before jumping back to us.
        c->proc = p;
        switchuvm(p);
        p->state = RUNNING;

        swtch(&(c->scheduler), p->context);
        switchkvm();

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
      }
      release(&ptable.lock);
    }
  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void)
{
  int intena;
  struct proc *p = myproc();

  if (!holding(&ptable.lock))
    panic("sched ptable.lock");
  if (mycpu()->ncli != 1)
    panic("sched locks");
  if (p->state == RUNNING)
    panic("sched running");
  if (readeflags() & FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void yield(void)
{
  acquire(&ptable.lock); // DOC: yieldlock
  myproc()->state = RUNNABLE;

  if (myproc()->sched_policy >= 0)
    myproc()->elapsed_time++;

  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first)
  {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();

  if (p == 0)
    panic("sleep");

  if (lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if (lk != &ptable.lock)
  {                        // DOC: sleeplock0
    acquire(&ptable.lock); // DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if (lk != &ptable.lock)
  { // DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

// PAGEBREAK!
//  Wake up all processes sleeping on chan.
//  The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {
      p->killed = 1;
      // Wake process from sleep if necessary.
      if (p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

// PAGEBREAK: 36
//  Print a process listing to console.  For debugging.
//  Runs when user types ^P on console.
//  No lock to avoid wedging a stuck machine further.
void procdump(void)
{
  static char *states[] = {
      [UNUSED] "unused",
      [EMBRYO] "embryo",
      [SLEEPING] "sleep ",
      [RUNNABLE] "runble",
      [RUNNING] "run   ",
      [ZOMBIE] "zombie"};
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->state == UNUSED)
      continue;
    if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if (p->state == SLEEPING)
    {
      getcallerpcs((uint *)p->context->ebp + 2, pc);
      for (i = 0; i < 10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int setexectime(int pid, int exec_time)
{
  struct proc *p;
  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {
      p->execution_time = exec_time;
      break;
    }
  }

  release(&ptable.lock);

  if (p->sched_policy != -1)
    return -22;

  return 0;
}

int setdeadline(int pid, int deadline)
{
  struct proc *p;
  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {
      p->deadline = deadline;

      break;
    }
  }

  release(&ptable.lock);

  if (p->sched_policy != -1) // check
    return -22;

  return 0;
}

int setrate(int pid, int rate)
{
  struct proc *p;
  acquire(&ptable.lock);

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {

      p->rate = rate;

      int w = (30 - rate) * 3 + 28;
      w = w / 29;
      if (w < 1)
        w = 1;
      p->weight = w;

      break;
    }
  }

  release(&ptable.lock);
  if (p->sched_policy != -1)
    return -22;

  return 0;
}

int setschedpolicy(int pid, int policy)
{

  struct proc *p;
  int check = 0;

  acquire(&ptable.lock);

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {
      p->sched_policy = policy;

      p->arrival_time = ticks;
      break;
    }
  }

  release(&ptable.lock);
  check = schedulability_check(policy);

  if (check == -22)
  {
    p->sched_policy = -1;
    kill(pid);
    return -22;
  }

  return 0;
}

int schedulability_check(int policy)
{
  // edf check
  if (policy == 0)
  {
    float sum = 0;

    struct proc *p;
    acquire(&ptable.lock);

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->sched_policy == 0)
        sum += ((float)p->execution_time) / p->deadline;
    }
    release(&ptable.lock);

    if (sum > 1)
      return -22;

    return 0;
  }

  // rm check
  else
  {
    float sum = 0;
    int num_runnable = 0;
    struct proc *p;

    acquire(&ptable.lock);

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->sched_policy == 1)
      {
        num_runnable += 1;
        sum += (float)p->execution_time * p->rate;
      }
    }
    release(&ptable.lock);

    if (sum > 100 * lielay[num_runnable])
      return -22;

    return 0;
  }
}