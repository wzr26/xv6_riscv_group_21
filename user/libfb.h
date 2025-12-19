// user/libfb.h
// User-space framebuffer graphics library (Week 5)

#ifndef LIBFB_H
#define LIBFB_H

// Framebuffer dimensions
#define FB_WIDTH  128
#define FB_HEIGHT 128

// Initialize framebuffer device (open /dev/fb)
void libfb_init(void);

// Close framebuffer device
void libfb_close(void);

// Drawing primitives
void libfb_clear(unsigned int color);
void libfb_draw_pixel(int x, int y, unsigned int color);
void libfb_draw_rect(int x, int y, int w, int h, unsigned int color);
void libfb_draw_line(int x0, int y0, int x1, int y1, unsigned int color);
void libfb_draw_circle(int cx, int cy, int r, unsigned int color);
void libfb_draw_box(int x, int y, int w, int h, unsigned int color);

// Utility functions
int libfb_width(void);
int libfb_height(void);
void libfb_show_ascii_preview(void);

// Higher-level drawing helpers
void libfb_fill_color(unsigned int color);
void libfb_test_pattern(void);
void libfb_draw_gradient(int x, int y, int w, int h);

// Profiling/timing helpers
unsigned long libfb_get_ticks(void);
void libfb_profile_start(void);
void libfb_profile_end(void);

#endif // LIBFB_H
