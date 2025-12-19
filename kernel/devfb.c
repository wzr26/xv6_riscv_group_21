// kernel/devfb.c
// Framebuffer device for xv6-riscv
//
// Provides a minimal /dev/fb interface:
//   - read()  returns raw framebuffer bytes
//   - write() writes raw framebuffer bytes
//   - supports fb_clear(), fb_draw_pixel(), etc.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "fb.h"

// Framebuffer memory (provided by fb.c)
extern uint32 *fb;           // pointer to framebuffer
extern int fb_size_bytes;    // total bytes

struct spinlock fb_lock;

// --------------------------------------------------------------
// Initialize /dev/fb
// --------------------------------------------------------------
void
fbdev_init(void)
{
    initlock(&fb_lock, "fbdev");
    fb_init();              // initialize framebuffer from fb.c
    printf("[fbdev] /dev/fb initialized (%d bytes)\n", fb_size_bytes);
}

// --------------------------------------------------------------
// Read from the framebuffer: copy raw bytes to user
// --------------------------------------------------------------
int
fbdev_read(int user_dst, uint64 dst, int n)
{
    if (n <= 0) return 0;

    // clamp reads
    if (n > fb_size_bytes)
        n = fb_size_bytes;

    acquire(&fb_lock);

    // copy framebuffer to user
    int copied = either_copyout(
        user_dst,
        dst,
        (void *)fb,
        n
    );

    release(&fb_lock);

    return (copied < 0 ? -1 : n);
}

// --------------------------------------------------------------
// Write raw bytes into the framebuffer
// --------------------------------------------------------------
int
fbdev_write(int user_src, uint64 src, int n)
{
    // Support two write formats:
    // 1) Raw framebuffer bytes: write exactly fb_size_bytes bytes -> copies to base of fb.
    // 2) Rect write: header (4 x int32: x,y,w,h) followed by w*h uint32 pixels -> copies into rectangle.
    if (n <= 0) return 0;

    acquire(&fb_lock);

    // Fast path: full-buffer raw write
    if (n == fb_size_bytes) {
        int copied = either_copyin((void *)fb, user_src, src, n);
        // print ASCII preview for immediate feedback when user writes whole buffer
        fb_print_ascii_if_needed();
        release(&fb_lock);
        return (copied < 0 ? -1 : n);
    }

    // Try rectangle header (16 bytes) + pixel data
    if (n >= 16) {
        uint32 header[4];
        // copy header from user
        if (either_copyin((void *)header, user_src, src, 16) < 0) {
            release(&fb_lock);
            return -1;
        }

        int x = (int)header[0];
        int y = (int)header[1];
        int w = (int)header[2];
        int h = (int)header[3];

        // basic validation
        if (w <= 0 || h <= 0 || x < 0 || y < 0 || x + w > FB_WIDTH || y + h > FB_HEIGHT) {
            release(&fb_lock);
            return -1;
        }

        uint64 expected = 16 + (uint64)w * (uint64)h * sizeof(uint32);
        if ((uint64)n < expected) {
            release(&fb_lock);
            return -1;
        }

        // copy each row directly into framebuffer memory
        for (int row = 0; row < h; row++) {
            uint64 user_off = src + 16 + (uint64)row * (uint64)w * sizeof(uint32);
            uint32 *dst = fb + (y + row) * FB_WIDTH + x;
            if (either_copyin((void *)dst, user_src, user_off, w * sizeof(uint32)) < 0) {
                release(&fb_lock);
                return -1;
            }
        }

        // show ASCII preview after rect write
        fb_print_ascii_if_needed();
        release(&fb_lock);
        return (int)expected;
    }

    // Fallback: if smaller than header or not matching formats, clamp to fb_size_bytes and copy to base
    if (n > fb_size_bytes) n = fb_size_bytes;
    int copied = either_copyin((void *)fb, user_src, src, n);
    // show ASCII preview after fallback write
    fb_print_ascii_if_needed();
    release(&fb_lock);
    return (copied < 0 ? -1 : n);
}

// --------------------------------------------------------------
// File ops table for /dev/fb
// --------------------------------------------------------------
extern struct devsw devsw[];

void
fbdev_register(void)
{
    devsw[FB_DEVICE].read  = fbdev_read;
    devsw[FB_DEVICE].write = fbdev_write;
    fbdev_init();
}

