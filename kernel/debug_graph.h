// kernel/debug_graph.h
#ifndef DEBUG_GRAPH_H
#define DEBUG_GRAPH_H

#include "types.h"
#include "spinlock.h"

/*
 * Simple kernel-side diagnostic graphing/profiling helper.
 *
 * Usage:
 *   dbg_init();                 // once at boot (e.g., in main() or animation_init())
 *   dbg_record(int value);      // called every tick/frame to record a sample
 *   dbg_dump_ascii();           // prints ASCII graph to kernel console (printf)
 *   dbg_clear();                // clear buffer
 *
 * Thread-safety: protected by internal spinlock.
 */

#define DBG_SAMPLES 128     // number of samples in circular buffer
#define DBG_ASCII_WIDTH 60  // max width of ASCII bar when dumping

void dbg_init(void);
void dbg_record(int value);    // record a sample (value can be any small int)
void dbg_dump_ascii(void);     // pretty-print collected samples to kernel console
void dbg_clear(void);          // reset buffer

#endif // DEBUG_GRAPH_H

