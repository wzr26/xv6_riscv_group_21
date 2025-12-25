// kernel/sysproc.c

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "animation.h"
#include "fb.h"
#include "debug_graph.h"
extern struct proc proc[NPROC];

// ====================================================
// GLOBALS for animation
// ====================================================
int animation_enabled = 0;
int anim_ticks_per_frame = 10;

// ====================================================
// syscall: start_anim()
// ====================================================
uint64
sys_start_anim(void)
{
  // Simple flag write - no lock needed
  animation_enabled = 1;
  printf("[kernel] Animation started.\n");
  return 0;
}

// ====================================================
// syscall: stop_anim()
// ====================================================
uint64
sys_stop_anim(void)
{
  // Simple flag write - no lock needed
  animation_enabled = 0;
  printf("[kernel] Animation stopped.\n");
  return 0;
}

// ====================================================
// syscall: set_speed(int)
// ====================================================
uint64
sys_set_speed(void)
{
  int speed;
  argint(0, &speed);

  // validate provided speed (ticks per frame). Keep a reasonable range.
  if (speed < 1) {
    printf("[kernel] set_speed: invalid speed %d -> must be >= 1\n", speed);
    return -1;
  }
  if (speed > 1000000) {
    printf("[kernel] set_speed: clamping huge speed %d -> 1000000\n", speed);
    speed = 1000000;
  }

  // Simple flag write - no lock needed
  anim_ticks_per_frame = speed;
  printf("[kernel] Animation speed set to %d ticks/frame.\n", anim_ticks_per_frame);
  return 0;
}

// ====================================================
// syscall: view_anim()
// ====================================================
uint64
sys_view_anim(void)
{
  // Print current framebuffer state as ASCII preview
  fb_print_ascii_preview();
  return 0;
}

// ====================================================
// syscall: fb_write(int x, int y, uint32 color)
// ====================================================
uint64
sys_fb_write(void)
{
  int x, y;
  int color;
  argint(0, &x);
  argint(1, &y);
  argint(2, &color);

  fb_draw_pixel(x, y, (uint32)color);

  return 0;
}

// ====================================================
// syscall: fb_clear(uint32 color)
// ====================================================
uint64
sys_fb_clear(void)
{
  int color;
  argint(0, &color);

  fb_clear((uint32)color);

  return 0;
}

// ====================================================
// syscall: hello()
// ====================================================
uint64
sys_hello(void)
{
  printf("Hello from xv6 kernel!\n");
  return 0;
}

// ====================================================
// syscall: kinfo()
// ====================================================
uint64
sys_kinfo(void)
{
  struct proc *p;
  struct proc *curproc = myproc();

  printf("kernel: kinfo() called by pid %d\n", curproc->pid);

  void *mem = kalloc();
  if(mem == 0){
    printf("kernel: memory allocation failed!\n");
  } else {
    printf("kernel: allocated one page at %p\n", mem);
  }

  printf("PID\tSTATE\t\tNAME\n");
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;

    printf("%d\t", p->pid);
    switch(p->state){
      case SLEEPING: printf("SLEEPING\t"); break;
      case RUNNING:  printf("RUNNING\t\t"); break;
      case RUNNABLE: printf("RUNNABLE\t"); break;
      default:       printf("OTHER\t\t"); break;
    }
    printf("%s\n", p->name);
  }

  printf("kernel: kinfo() done.\n");
  return 0;
}

// ====================================================
// REQUIRED syscall: sbrk
// (FIXES undefined reference to sys_sbrk)
// ====================================================
uint64
sys_sbrk(void)
{
  int addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;

  if (growproc(n) < 0)
    return -1;

  return addr;
}


// ====================================================
// Default xv6 syscalls
// ====================================================
uint64 sys_exit(void){ int n; argint(0,&n); kexit(n); return 0; }
uint64 sys_getpid(void){ return myproc()->pid; }
uint64 sys_fork(void){ return kfork(); }
uint64 sys_wait(void){ uint64 p; argaddr(0,&p); return kwait(p); }
uint64 sys_kill(void){ int pid; argint(0,&pid); return kkill(pid); }
uint64 sys_uptime(void){ uint xticks; acquire(&tickslock); xticks=ticks; release(&tickslock); return xticks; }

uint64 sys_pause(void){
  int n; 
  uint ticks0;

  argint(0,&n);
  if(n < 0) n = 0;

  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < (uint)n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}
uint64
sys_debuggraph(void)
{
    dbg_dump_ascii();
    return 0;
}

