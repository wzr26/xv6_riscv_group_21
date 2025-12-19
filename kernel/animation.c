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
static int y = 20;
static int dx = 2;
static int w = 12, h = 6;
static int prev_x = 0;
static int prev_y = 20;
static int frame_no = 0;

// Export the old anim_state_t for legacy code
anim_state_t anim = {0, 20, 12, 6, 2, 0, 0};

// Frame pacing state (Week 4)
anim_frame_state_t anim_frame_state = {0, 0, 0, 0.0f};

// Multi-object animation array (Week 5)
anim_object_t anim_objects[MAX_ANIM_OBJECTS];
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
    anim_frame_state.delta_time = 0.0f;
    
    // Initialize multi-object array (Week 5)
    for(int i = 0; i < MAX_ANIM_OBJECTS; i++) {
        anim_objects[i].active = 0;
        anim_objects[i].x = 0;
        anim_objects[i].y = 0;
        anim_objects[i].w = 8;
        anim_objects[i].h = 8;
        anim_objects[i].dx = 0;
        anim_objects[i].dy = 0;
        anim_objects[i].color = 0xff0000;
    }
    num_active_objects = 0;
}

// Register a new animated object (Week 5)
void
anim_register_object(int ox, int oy, int ow, int oh, uint32 color)
{
    acquire(&anim_lock);
    
    if(num_active_objects < MAX_ANIM_OBJECTS) {
        int idx = num_active_objects;
        anim_objects[idx].x = ox;
        anim_objects[idx].y = oy;
        anim_objects[idx].w = ow;
        anim_objects[idx].h = oh;
        anim_objects[idx].color = color;
        anim_objects[idx].dx = 2;   // default velocity
        anim_objects[idx].dy = 1;
        anim_objects[idx].active = 1;
        num_active_objects++;
    }
    
    release(&anim_lock);
}

// Clear all objects (Week 5)
void
anim_clear_all(void)
{
    acquire(&anim_lock);
    
    for(int i = 0; i < MAX_ANIM_OBJECTS; i++) {
        anim_objects[i].active = 0;
    }
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
    acquire(&anim_lock);

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

    release(&anim_lock);
}

// Update all active objects (Week 5)
void
anim_update_all(void)
{
    acquire(&anim_lock);
    
    for(int i = 0; i < num_active_objects; i++) {
        if(!anim_objects[i].active) continue;
        
        anim_object_t *obj = &anim_objects[i];
        
        // Apply velocity
        obj->x += obj->dx;
        obj->y += obj->dy;
        
        // Boundary detection and bouncing (Week 5)
        if(obj->x < 0 || obj->x + obj->w >= fb_width()) {
            obj->dx = -obj->dx;
            obj->x += obj->dx;
        }
        if(obj->y < 0 || obj->y + obj->h >= fb_height()) {
            obj->dy = -obj->dy;
            obj->y += obj->dy;
        }
    }
    
    release(&anim_lock);
}

// Draw the current frame. Supports both legacy single-object and multi-object modes.
void
draw_next_frame(void)
{
    acquire(&anim_lock);

    // If multi-object mode is active, render all objects
    if(num_active_objects > 0) {
        // Clear entire framebuffer for multi-object rendering
        fb_clear(0x000000);
        
        // Draw all active objects
        for(int i = 0; i < num_active_objects; i++) {
            if(!anim_objects[i].active) continue;
            
            anim_object_t *obj = &anim_objects[i];
            fb_draw_rect(obj->x, obj->y, obj->w, obj->h, obj->color);
        }
    } else {
        // Legacy single-object rendering mode
        erase_previous();
        fb_draw_rect(x, y, w, h, 0xff2020);  // soft red tone
    }
    
    // Print lightweight ASCII preview (Week 4 - low frequency)
    fb_print_ascii_if_needed();

    release(&anim_lock);
}

