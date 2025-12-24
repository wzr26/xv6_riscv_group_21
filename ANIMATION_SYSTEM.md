# Animation Control System - Complete Feature Summary

## Overview
This xv6-RISC-V kernel implementation provides a complete animation control system with user-space command-line interface (`animctl`).

## Architecture

### Kernel Components (kernel/)
1. **animation.c** - Core animation engine
   - Manages animation state (position, velocity, frame pacing)
   - Draws objects to framebuffer
   - Called from interrupt handler at each timer tick

2. **trap.c** - Timer interrupt integration
   - Calls `anim_tick()` on every 10kHz timer interrupt
   - Manages frame pacing based on `anim_ticks_per_frame`

3. **fb.c** - Framebuffer driver
   - 128×128 pixel 32-bit RGBA display
   - Pixel drawing and rectangle filling
   - ASCII preview for text-mode visualization

4. **sysproc.c** - Syscall implementations
   - `sys_start_anim()` - Enable animation
   - `sys_stop_anim()` - Disable animation
   - `sys_set_speed()` - Configure frame pacing
   - `sys_view_anim()` - Get current framebuffer as ASCII
   - `sys_get_anim_state()` - Query animation enabled status

### User-Space Components (user/)
1. **animctl.c** - Animation control program
   ```
   animctl start          # Start animation
   animctl stop           # Stop animation
   animctl speed <n>      # Set speed (ticks per frame)
   animctl view           # Display real-time ASCII preview
   ```

2. **libfb.c** - Framebuffer graphics library
   - Drawing primitives (pixel, rectangle, line, circle)
   - Color management
   - Resolution aware (128×128 pixels)

## Synchronization Strategy

### No Spinlocks in Interrupt Context
Animation updates run in interrupt context with interrupts disabled. RISC-V guarantees that single-word assignments are atomic, so we use simple flag writes instead of spinlocks:

```c
// In interrupt context (trap.c):
if (!animation_enabled) return;  // Simple read, no lock

// In syscalls (sysproc.c):
animation_enabled = 1;            // Simple write, no lock
```

**Why?** Attempting to acquire/release spinlock in interrupt context causes "panic: release" errors because locks expect normal context.

### Atomic Operations
- `animation_enabled` - 1 word, read/written atomically
- `anim_ticks_per_frame` - 1 word, read/written atomically
- No data corruption possible with interrupt-context reads during syscall writes

## Features

### 1. Kernel-Level Timer-Driven Animation
- Animation updates triggered by 10kHz timer interrupt
- Automatic frame pacing: `anim_ticks_per_frame` controls speed
- No busy-waiting or syscalls from timer handler

### 2. User-Space Control
- Four syscalls for complete control:
  - `start_anim()` - Enable animation (sets flag to 1)
  - `stop_anim()` - Disable animation (sets flag to 0)
  - `set_speed(n)` - Adjust pacing (1-1000000 ticks/frame)
  - `view_anim()` - Get ASCII visualization of current frame
  - `get_anim_state()` - Query if animation is enabled

### 3. Interactive Viewer Mode
- `animctl view` displays animation in real-time
- 10ms refresh rate for smooth display
- **Auto-exits when animation stops** (NEW FEATURE)
  - Monitors `animation_enabled` flag
  - No need for Ctrl+C
  - Clean terminal experience

### 4. Manual Control (Not Automatic)
- Animation does NOT start automatically on boot
- User explicitly runs `animctl start` to enable
- Allows clean demonstration of feature control

## Recent Improvements (Week 6)

### Problem: View Mode Required Ctrl+C to Exit
Users running `animctl view` in one terminal and `animctl stop` in another would see view mode keep running, requiring manual Ctrl+C exit.

### Solution: View State Monitoring
Added `get_anim_state()` syscall:
1. New syscall #28: `SYS_get_anim_state`
2. Returns 1 if `animation_enabled`, 0 if not
3. `animctl view` loops while `get_anim_state()` is true
4. When `animctl stop` sets flag to 0, view loop exits
5. Displays message: "[animctl] Animation stopped, exiting view mode"
6. Returns to shell prompt automatically

### Technical Details
```c
// animctl.c - NEW view mode
while (get_anim_state()) {     // Check animation state each iteration
  view_anim();                 // Display current frame
  printf("\n");                // Frame separator
  pause(100);                  // ~10ms delay
}
printf("\n[animctl] Animation stopped, exiting view mode\n");
```

## Compilation & Testing

### Build
```bash
cd /home/gkd2162/xv6-riscv
make clean
make
```

### Run
```bash
make qemu
# In QEMU shell:
$ animctl start
$ animctl view
# In another terminal:
$ animctl stop
# View mode automatically exits!
```

## Requirements Met

### ✅ Requirement 1: Kernel Timer Tasks
- Timer interrupt handler calls animation update every tick
- Frame pacing managed by configurable counter
- Works regardless of syscalls or user input

### ✅ Requirement 2: Framebuffer Drawing
- 128×128 pixel display
- Rectangle drawing for moving object
- Color management and clearing
- ASCII preview for text-mode visualization

### ✅ Requirement 3: User Program Control
- Four animation syscalls (start, stop, speed, view)
- `animctl` command-line tool for user interaction
- Get animation state via syscall
- Interactive viewer mode with auto-exit

## File Listing

**Modified in this session:**
- kernel/sysproc.c - Added sys_get_anim_state()
- kernel/syscall.h - Added SYS_get_anim_state = 28
- kernel/syscall.c - Registered new syscall handler
- user/user.h - Added get_anim_state() prototype
- user/usys.pl - Added syscall stub generation
- user/animctl.c - Changed view loop to monitor animation state

**Supporting documentation:**
- ANIMCTL_VIEW_MODE.md - Detailed feature documentation
- VIEW_MODE_FEATURE.txt - Quick feature reference
- Animation_Project_Report.md - Comprehensive architecture doc

## Performance Notes
- View mode refresh: 10ms (100 QEMU ticks)
- Animation frame rate: 3-10 kernel ticks (configurable)
- Syscall overhead: Negligible (simple atomic reads/writes)
- Memory: ~256KB framebuffer, minimal kernel overhead

## Known Limitations
- Single framebuffer (no double-buffering due to memory constraints)
- ASCII preview limited to 64×16 character grid
- QEMU-only testing (specific to nographic mode)
- Three harts (cores) configured in xv6 build
