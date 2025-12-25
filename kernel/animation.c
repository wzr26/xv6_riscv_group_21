// kernel/animation.c
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "fb.h"
#include "animation.h"

// Animation state
static int x = 0;
static int y = 50;
static int dx = 1;
static int w = 20, h = 12;
static int prev_x = 0;
static int prev_y = 50;
static int frame_no = 0;

// Frame pacing state
anim_frame_state_t anim_frame_state = {0, 0, 0, 0};

void
animation_init(void)
{
    fb_init();
    
    // Initialize frame pacing state
    anim_frame_state.last_tick = 0;
    anim_frame_state.target_ticks = 1;
    anim_frame_state.frame_count = 0;
    anim_frame_state.delta_time = 0;
}

static void
erase_previous(void)
{
    fb_draw_rect(prev_x, prev_y, w, h, 0x000000);
}

void
animation_update(void)
{
    // Called from timer interrupt context (interrupts disabled)
    frame_no++;
    prev_x = x;
    prev_y = y;

    x += dx;

    // Handle speed = 0 edge case
    if(anim_ticks_per_frame <= 0) {
        anim_ticks_per_frame = 1;
    }

    if (x < 0 || x + w >= fb_width()) {
        dx = -dx;
        x += dx;
    }
}

void
draw_next_frame(void)
{
    // Called from timer interrupt context (interrupts disabled)
    erase_previous();
    fb_draw_rect(x, y, w, h, 0xff2020);  // red rectangle
    fb_print_ascii_if_needed();
}

