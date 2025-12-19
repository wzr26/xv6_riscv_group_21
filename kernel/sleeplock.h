#ifndef SLEEPLOCK_H
#define SLEEPLOCK_H

#include "types.h"
#include "spinlock.h"

// Long-term locks for processes (sleep locks)
struct sleeplock {
  uint locked;          // Is the lock held?
  struct spinlock lk;   // Spinlock protecting this sleeplock

  // Debug info
  char *name;           // Name of lock
  int pid;              // Process holding lock
};

#endif // SLEEPLOCK_H

