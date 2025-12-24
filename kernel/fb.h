#ifndef FB_H
#define FB_H

#include "types.h"

#define FB_WIDTH  128
#define FB_HEIGHT 128

// Row-aligned memory for faster access (Week 4)
#define FB_ROW_ALIGN 128

// Initialize framebuffer
void fb_init(void);

// Clear entire framebuffer
void fb_clear(uint32 color);

// Draw a single pixel
void fb_draw_pixel(int x, int y, uint32 color);

// Draw filled rectangle
void fb_draw_rect(int x, int y, int w, int h, uint32 color);

// Return size
int fb_width(void);
int fb_height(void);

// Test pattern
void fb_test_pattern(void);

// Print a coarse ASCII preview of the framebuffer to the serial console.
// Called periodically to make animation visible on the text console.
void fb_print_ascii_if_needed(void);

// Print ASCII preview on demand (called via syscall)
void fb_print_ascii_preview(void);

// Additional drawing primitives (Week 3)
void fb_draw_line(int x0, int y0, int x1, int y1, uint32 color);
void fb_draw_box(int x, int y, int w, int h, uint32 color);
void fb_draw_box_filled(int x, int y, int w, int h, uint32 color);
void fb_draw_circle(int cx, int cy, int r, uint32 color);

// Double-buffering support (Week 5)
void fb_swap_buffers(void);
void fb_flush_region(int x, int y, int w, int h);

// Sprite rendering support (Week 5)
void fb_blit_sprite(int x, int y, const void *sprite, uint32 color_key);
int fb_rle_decompress(const uint8 *src, uint32 *dst, int max_pixels);

#endif


