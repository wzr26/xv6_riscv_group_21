#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "types.h"

// Frame pacing state
typedef struct anim_frame_state {
  uint64 last_tick;
  int target_ticks;
  int frame_count;
  int delta_time;
} anim_frame_state_t;

// External declarations
extern int animation_enabled;
extern int anim_ticks_per_frame;
extern anim_frame_state_t anim_frame_state;

// Initialization
void animation_init(void);

// Frame update
void animation_update(void);
void draw_next_frame(void);

#endif // _ANIMATION_H_

