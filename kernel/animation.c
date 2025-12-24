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

// Legacy single-object state (backward compatibility)
static int x = 0;
static int y = 50;
static int dx = 3;
static int w = 20, h = 12;
static int prev_x = 0;
static int prev_y = 20;
static int frame_no = 0;

// Export the old anim_state_t for legacy code
anim_state_t anim = {0, 20, 12, 6, 2, 0, 0};

// Frame pacing state (Week 4)
anim_frame_state_t anim_frame_state = {0, 0, 0, 0};

// Multi-object animation array (Week 5) - dynamically allocated pointers
anim_object_t *anim_objects = 0;
static int num_active_objects = 0;

// Lock protecting animation state
struct spinlock anim_lock;

void
animation_init(void)
{
    initlock(&anim_lock, "anim");
    fb_init();
    
    // Initialize frame pacing state (Week 4)
    anim_frame_state.last_tick = 0;
    anim_frame_state.target_ticks = 3;  // default 3 ticks per frame
    anim_frame_state.frame_count = 0;
    anim_frame_state.delta_time = 0;
    
    num_active_objects = 0;
}

// Register a new animated object (Week 5)
void
anim_register_object(int ox, int oy, int ow, int oh, uint32 color)
{
    // Not yet implemented - multi-object mode requires dynamic allocation
    // Placeholder for future Week 6 implementation
    (void)ox;
    (void)oy;
    (void)ow;
    (void)oh;
    (void)color;
}

// Clear all objects (Week 5)
void
anim_clear_all(void)
{
    acquire(&anim_lock);
    num_active_objects = 0;
    release(&anim_lock);
}

static void
erase_previous(void)
{
    fb_draw_rect(prev_x, prev_y, w, h, 0x000000);  // black
}

// Update single object (legacy, Week 1-3)
void
animation_update(void)
{
    // Note: Called from timer interrupt context (interrupts disabled)
    // No lock needed - interrupt-safe by design
    frame_no++;
    prev_x = x;
    prev_y = y;

    x += dx;

    // Handle speed = 0 edge case (Week 4 improvement)
    if(anim_ticks_per_frame <= 0) {
        // prevent division by zero; use default
        anim_ticks_per_frame = 3;
    }

    if (x < 0 || x + w >= fb_width()) {
        dx = -dx;
        x += dx;
    }
}

// Update all active objects (Week 5)
void
anim_update_all(void)
{
    // Placeholder - not yet implemented
    // Future Week 6 will add full multi-object support
}

// Draw the current frame. Supports both legacy single-object and multi-object modes.
void
draw_next_frame(void)
{
    // Note: Called from timer interrupt context (interrupts disabled)
    // No lock needed - interrupt-safe by design
    
    // Legacy single-object rendering mode
    erase_previous();
    fb_draw_rect(x, y, w, h, 0xff2020);  // soft red tone
    
    // Print lightweight ASCII preview (Week 4 - low frequency)
    fb_print_ascii_if_needed();
}

