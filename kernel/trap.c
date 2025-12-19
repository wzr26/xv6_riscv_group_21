// kernel/trap.c
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "animation.h"
#include "fb.h"
#include "debug_graph.h"

struct spinlock tickslock;
uint ticks;

// Animation globals (declared in sysproc.c)
extern int animation_enabled;
extern int anim_ticks_per_frame;

// Internal tick counter
static int anim_tick_counter = 0;

// -----------------------------------------------------
// Animation tick: called whenever a timer interrupt fires.
// -----------------------------------------------------
void
anim_tick(void)
{
  if (!animation_enabled)
    return;

  anim_tick_counter++;

  if (anim_tick_counter < anim_ticks_per_frame)
    return;

  anim_tick_counter = 0;

  // Update physics + render frame
  animation_update();
  draw_next_frame();

  // OPTIONAL RATE-LIMITED DEBUG LOGGING
  static int dbg = 0;
  if ((dbg++ % 50) == 0)
    printf("[anim] frame updated (ticks=%d)\n", ticks);

  // OPTIONAL sample profiling (only records values)
  dbg_record(anim_tick_counter);
}

// -----------------------------------------------------
// TRAP INITIALIZATION
// -----------------------------------------------------
extern char trampoline[], uservec[];

void kernelvec();
extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

// -----------------------------------------------------
// USER TRAP — handles syscalls, faults, device interrupts
// -----------------------------------------------------
uint64
usertrap(void)
{
  int which_dev = 0;

  if ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // Switch to kernel trap handler while in kernel mode
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  p->trapframe->epc = r_sepc();

  if (r_scause() == 8) {
    // System call
    if (killed(p))
      kexit(-1);

    p->trapframe->epc += 4; // Skip ecall
    intr_on();
    syscall();

  } else if ((which_dev = devintr()) != 0) {
    // device interrupt processed
  } else if ((r_scause() == 15 || r_scause() == 13) &&
             vmfault(p->pagetable, r_stval(), (r_scause() == 13)) == 0) {
    // Lazy page allocation
  } else {
    printf("usertrap(): unexpected scause=0x%lx pid=%d\n",
           r_scause(), p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n",
           r_sepc(), r_stval());
    setkilled(p);
  }

  if (killed(p))
    kexit(-1);

  if (which_dev == 2)  // timer
    yield();

  prepare_return();
  return MAKE_SATP(p->pagetable);
}

// -----------------------------------------------------
// PREPARE RETURN TO USER MODE
// -----------------------------------------------------
void
prepare_return(void)
{
  struct proc *p = myproc();
  intr_off();

  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  p->trapframe->kernel_satp = r_satp();
  p->trapframe->kernel_sp = p->kstack + PGSIZE;
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();

  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP;
  x |= SSTATUS_SPIE;
  w_sstatus(x);

  w_sepc(p->trapframe->epc);
}

// -----------------------------------------------------
// KERNEL TRAP (when kernel faults or devices interrupt)
// -----------------------------------------------------
void
kerneltrap(void)
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if ((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");

  if (intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if ((which_dev = devintr()) == 0) {
    printf("kerneltrap: unexpected scause=0x%lx sepc=0x%lx stval=0x%lx\n",
           scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // Timer interrupt → Animation tick
  if (which_dev == 2) {
    anim_tick();
    if (myproc() != 0)
      yield();
  }

  // Restore registers
  w_sepc(sepc);
  w_sstatus(sstatus);
}

// -----------------------------------------------------
// TIMER INTERRUPT
// -----------------------------------------------------
void
clockintr(void)
{
  if (cpuid() == 0) {
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  // next timer event (100ms)
  w_stimecmp(r_time() + 1000000);
}

// -----------------------------------------------------
// PROCESS DEVICE INTERRUPTS
// -----------------------------------------------------
int
devintr(void)
{
  uint64 scause = r_scause();

  // External interrupt (PLIC)
  if (scause == 0x8000000000000009L) {
    int irq = plic_claim();

    if (irq == UART0_IRQ)
      uartintr();
    else if (irq == VIRTIO0_IRQ)
      virtio_disk_intr();
    else if (irq)
      printf("unexpected irq=%d\n", irq);

    if (irq)
      plic_complete(irq);

    return 1;
  }

  // Timer interrupt
  else if (scause == 0x8000000000000005L) {
    clockintr();
    return 2;
  }

  return 0;
}

