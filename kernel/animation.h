#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "types.h"

// Max number of animated objects (Week 5)
#define MAX_ANIM_OBJECTS 32

// Animation object structure for multi-object rendering (Week 5)
typedef struct anim_object {
  int x, y;              // position
  int w, h;              // size
  int dx, dy;            // velocity
  uint32 color;          // color
  int active;            // 0 = inactive, 1 = active
} anim_object_t;

// Global frame pacing state (Week 4)
typedef struct anim_frame_state {
  uint64 last_tick;      // timestamp of last frame update
  int target_ticks;      // desired ticks per frame
  int frame_count;       // total frames rendered
  float delta_time;      // time since last update (in units)
} anim_frame_state_t;

// Old single-object state (for backward compatibility, Week 1-3)
typedef struct anim_state {
  int x, y;
  int w, h;
  int dx, dy;
  int frame_count;
} anim_state_t;

// External declarations
extern int animation_enabled;        // 0 = OFF, 1 = ON
extern int anim_ticks_per_frame;     // frame pacing target
extern struct spinlock anim_lock;

extern anim_state_t anim;            // legacy single object
extern anim_frame_state_t anim_frame_state;  // frame timing state (Week 4)
extern anim_object_t anim_objects[MAX_ANIM_OBJECTS];  // multi-object array (Week 5)

// Initialization
void animation_init(void);

// Frame update
void animation_update(void);
void draw_next_frame(void);

// Multi-object API (Week 5)
void anim_register_object(int x, int y, int w, int h, uint32 color);
void anim_update_all(void);
void anim_clear_all(void);

#endif // _ANIMATION_H_

