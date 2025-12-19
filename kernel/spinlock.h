#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

// Forward declaration to avoid circular include problems
struct cpu;

struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The CPU holding the lock
};

#endif // SPINLOCK_H

