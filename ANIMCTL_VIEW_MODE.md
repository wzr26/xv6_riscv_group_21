# Testing animctl view mode auto-exit

## Feature: View Mode Now Auto-Exits When Animation Stops

### Implementation Details
- Added new syscall `get_anim_state()` that returns whether animation is enabled
- `animctl view` now continuously checks animation state
- When animation is stopped (via `animctl stop`), view loop automatically exits
- Returns to shell prompt with message: "[animctl] Animation stopped, exiting view mode"

### How to Test Manually in QEMU

1. **Start QEMU**
   ```bash
   cd /home/gkd2162/xv6-riscv
   make qemu
   ```

2. **In QEMU shell, run these commands**
   ```
   $ animctl start
   [kernel] Animation started.
   
   $ animctl view
   animctl: entering view mode (continuous frame display)
   (Run 'animctl stop' in another terminal to exit)
   
   (frames display continuously here)
   ```

3. **In another terminal/window, stop the animation**
   ```bash
   # While animctl view is running
   $ animctl stop
   [kernel] Animation stopped.
   animctl: stop requested
   ```

4. **Expected Result**
   - The `animctl view` process in the other terminal should automatically exit
   - You should see: `[animctl] Animation stopped, exiting view mode`
   - Shell prompt `$` returns
   - No need to press Ctrl+C

### Commands Summary
```
animctl start          # Start animation (sets animation_enabled=1)
animctl stop           # Stop animation (sets animation_enabled=0)
animctl speed <n>      # Set animation speed (n ticks per frame)
animctl view           # Display animated frames in real-time
                       # (auto-exits when animation stops)
```

### Technical Notes
- `get_anim_state()` is syscall #28 (SYS_get_anim_state)
- Returns 1 if animation enabled, 0 if disabled
- View loop calls this function at each iteration
- No spinlocks needed (simple atomic read of single-word flag)
- Very fast check (~10ms refresh rate)
