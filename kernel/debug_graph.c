// kernel/debug_graph.c
#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "debug_graph.h"

/*
 * Simple circular sampler + ASCII dumper for kernel debug/profiling.
 */

struct dbg_sample {
  uint64 when;   // time tick when recorded (if available)
  int value;     // recorded metric
};

static struct dbg_sample samples[DBG_SAMPLES];
static int head = 0;        // next write index (circular)
static int count = 0;       // number of valid samples (<= DBG_SAMPLES)
static struct spinlock dbg_lock;
static int dbg_inited = 0;

extern uint ticks; // defined in trap.c

void
dbg_init(void)
{
  if(dbg_inited) return;
  initlock(&dbg_lock, "dbg_graph");
  acquire(&dbg_lock);
  head = 0;
  count = 0;
  for(int i = 0; i < DBG_SAMPLES; i++){
    samples[i].when = 0;
    samples[i].value = 0;
  }
  dbg_inited = 1;
  release(&dbg_lock);
}

void
dbg_clear(void)
{
  acquire(&dbg_lock);
  head = 0;
  count = 0;
  for(int i = 0; i < DBG_SAMPLES; i++){
    samples[i].when = 0;
    samples[i].value = 0;
  }
  release(&dbg_lock);
}

/* Cheap recorder used in timer context */
void
dbg_record(int value)
{
  if(!dbg_inited) dbg_init();

  acquire(&dbg_lock);
  samples[head].value = value;
  samples[head].when = ticks; // use kernel ticks if available
  head = (head + 1) % DBG_SAMPLES;
  if(count < DBG_SAMPLES) count++;
  release(&dbg_lock);
}

/* Print collected samples as a simple ASCII graph. */
void
dbg_dump_ascii(void)
{
  if(!dbg_inited) dbg_init();

  acquire(&dbg_lock);

  if(count == 0){
    printf("[dbg] no samples\n");
    release(&dbg_lock);
    return;
  }

  int maxv = 1;
  int idx_start = (head - count + DBG_SAMPLES) % DBG_SAMPLES;
  for(int i = 0; i < count; i++){
    int idx = (idx_start + i) % DBG_SAMPLES;
    int v = samples[idx].value;
    if(v > maxv) maxv = v;
  }

  printf("[dbg] samples=%d max=%d\n", count, maxv);
  for(int i = 0; i < count; i++){
    int idx = (idx_start + i) % DBG_SAMPLES;
    int v = samples[idx].value;
    int len = (v * DBG_ASCII_WIDTH) / maxv;
    if(len < 0) len = 0;
    if(len > DBG_ASCII_WIDTH) len = DBG_ASCII_WIDTH;

    if(samples[idx].when)
      printf("%5d: %4d |", (int)samples[idx].when, v);
    else
      printf("       %4d |", v);

    for (int b = 0; b < len; b++)
    consputc('*');
consputc('\n');

  }

  release(&dbg_lock);
}

