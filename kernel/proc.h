#ifndef PROC_H
#define PROC_H

#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "file.h"
#include "fs.h"

// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved registers
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // current process
  struct context context;     // swtch() here to enter scheduler
  int noff;                   // push_off nesting
  int intena;                 // interrupt enabled before push_off?
};

extern struct cpu cpus[NCPU];

// Trapframe for user traps
struct trapframe {
  uint64 kernel_satp;     // kernel page table
  uint64 kernel_sp;       // process kernel stack top
  uint64 kernel_trap;     // usertrap()
  uint64 epc;             // saved user program counter
  uint64 kernel_hartid;   // saved tp for this hart

  uint64 ra;
  uint64 sp;
  uint64 gp;
  uint64 tp;

  uint64 t0;
  uint64 t1;
  uint64 t2;

  uint64 s0;
  uint64 s1;

  uint64 a0;
  uint64 a1;
  uint64 a2;
  uint64 a3;
  uint64 a4;
  uint64 a5;
  uint64 a6;
  uint64 a7;

  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;

  uint64 t3;
  uint64 t4;
  uint64 t5;
  uint64 t6;
};

enum procstate {
  UNUSED,
  USED,
  SLEEPING,
  RUNNABLE,
  RUNNING,
  ZOMBIE
};

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock required
  enum procstate state;
  void *chan;              // channel for sleep
  int killed;
  int xstate;
  int pid;

  // parent process
  struct proc *parent;

  // Private state, lock not required
  uint64 kstack;           // address of kernel stack
  uint64 sz;               // process memory size
  pagetable_t pagetable;   // user page table
  struct trapframe *trapframe;
  struct context context;
  struct file *ofile[NOFILE];
  struct inode *cwd;       // current directory
  char name[16];           // debugging
};

#endif // PROC_H

