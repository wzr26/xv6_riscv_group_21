#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"         // <-- MUST be included before defs.h
#include "defs.h"
#include "fb.h"
#include "animation.h"

// Local framebuffer stored inside kernel as a flat buffer (single buffer for stability)
static uint32 fb_mem[FB_WIDTH * FB_HEIGHT];

// Exported pointer and size so /dev/fb can reference the framebuffer
uint32 *fb = fb_mem;
int fb_size_bytes = FB_WIDTH * FB_HEIGHT * sizeof(uint32);

void 
fb_init(void) 
{
    fb_clear(0x000000);
}

// Swap buffers (placeholder for Week 5 - future optimization)
void 
fb_swap_buffers(void)
{
    // Currently a no-op since we're using single buffer for stability
    // Can be optimized with double-buffering in future kernel versions
}

void 
fb_clear(uint32 color) 
{
    // Optimized clear with unrolled loops (Week 4)
    int size = FB_WIDTH * FB_HEIGHT;
    
    // Unroll loop for faster clearing
    for(int i = 0; i < size; i += 4) {
        fb_mem[i]     = color;
        fb_mem[i+1]   = color;
        fb_mem[i+2]   = color;
        fb_mem[i+3]   = color;
    }
    // Handle remainder
    for(int i = (size / 4) * 4; i < size; i++) {
        fb_mem[i] = color;
    }
}

static inline int 
in_bounds(int x, int y) 
{
    return !(x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT);
}

void 
fb_draw_pixel(int x, int y, uint32 color) 
{
    if(!in_bounds(x, y)) return;
    fb_mem[y * FB_WIDTH + x] = color;
}

void 
fb_draw_rect(int x, int y, int w, int h, uint32 color) 
{
    for(int yy = y; yy < y + h; yy++) {
        for(int xx = x; xx < x + w; xx++) {
            fb_draw_pixel(xx, yy, color);
        }
    }
}

// Flush a specific region to display (Week 4)
void 
fb_flush_region(int x, int y, int w, int h)
{
    // For now, this is a no-op for stability
    // Can be optimized for partial updates in future versions
    (void)x;
    (void)y;
    (void)w;
    (void)h;
}

int 
fb_width(void)  
{ 
    return FB_WIDTH; 
}

int 
fb_height(void) 
{ 
    return FB_HEIGHT; 
}

void 
fb_test_pattern(void) 
{
    for(int y = 0; y < FB_HEIGHT; y++) {
        for(int x = 0; x < FB_WIDTH; x++) {
            fb_mem[y * FB_WIDTH + x] = (x * 5) ^ (y * 7);
        }
    }
}

// Bresenham's line algorithm
void 
fb_draw_line(int x0, int y0, int x1, int y1, uint32 color) 
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = dx >= 0 ? 1 : -1;
    int sy = dy >= 0 ? 1 : -1;
    dx = dx >= 0 ? dx : -dx;
    dy = dy >= 0 ? dy : -dy;

    if (dx > dy) {
        int err = dx/2;
        int y = y0;
        for (int x = x0; x != x1; x += sx) {
            fb_draw_pixel(x, y, color);
            err -= dy;
            if (err < 0) { y += sy; err += dx; }
        }
        fb_draw_pixel(x1, y1, color);
    } else {
        int err = dy/2;
        int x = x0;
        for (int y = y0; y != y1; y += sy) {
            fb_draw_pixel(x, y, color);
            err -= dx;
            if (err < 0) { x += sx; err += dy; }
        }
        fb_draw_pixel(x1, y1, color);
    }
}

void 
fb_draw_box(int x, int y, int w, int h, uint32 color) 
{
    fb_draw_line(x, y, x + w - 1, y, color);
    fb_draw_line(x, y, x, y + h - 1, color);
    fb_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
    fb_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
}

void 
fb_draw_box_filled(int x, int y, int w, int h, uint32 color) 
{
    for (int yy = y; yy < y + h; yy++) {
        for (int xx = x; xx < x + w; xx++) {
            fb_draw_pixel(xx, yy, color);
        }
    }
}

// Midpoint circle algorithm
void 
fb_draw_circle(int cx, int cy, int r, uint32 color) 
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        fb_draw_pixel(cx + x, cy + y, color);
        fb_draw_pixel(cx + y, cy + x, color);
        fb_draw_pixel(cx - y, cy + x, color);
        fb_draw_pixel(cx - x, cy + y, color);
        fb_draw_pixel(cx - x, cy - y, color);
        fb_draw_pixel(cx - y, cy - x, color);
        fb_draw_pixel(cx + y, cy - x, color);
        fb_draw_pixel(cx + x, cy - y, color);

        y += 1;
        if (err <= 0) {
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

// Sprite blitting support (Week 5)
// Simple blitting with color key transparency
void 
fb_blit_sprite(int x, int y, const void *sprite, uint32 color_key)
{
    // Basic implementation: sprite is expected to be uint32* array
    // with width/height encoded in first two uint32s
    // For now, this is a placeholder for future sprite format
    (void)x;
    (void)y;
    (void)sprite;
    (void)color_key;
    // TODO: implement full sprite blitting with proper format
}

// RLE decompressor for sprites (Week 5)
int 
fb_rle_decompress(const uint8 *src, uint32 *dst, int max_pixels)
{
    int count = 0;
    if(!src || !dst) return 0;
    
    while(count < max_pixels && *src != 0xFF) {
        uint8 byte = *src++;
        if(byte & 0x80) {
            // Run of identical pixels
            int run = (byte & 0x7F) + 1;
            uint32 color = *(uint32*)src;
            src += 4;
            for(int i = 0; i < run && count < max_pixels; i++) {
                dst[count++] = color;
            }
        } else {
            // Literal pixels
            int count_lit = (byte & 0x7F) + 1;
            for(int i = 0; i < count_lit && count < max_pixels; i++) {
                dst[count++] = *(uint32*)src;
                src += 4;
            }
        }
    }
    return count;
}

// ASCII preview support (Week 4) - On-demand printing via syscall

static char const *ascii_map = " .:-=+*#%@";

static void
fb_print_ascii_now(void)
{
    int sw = 64; // sample width
    int sh = 16; // sample height
    int sx = FB_WIDTH / sw;
    int sy = FB_HEIGHT / sh;

    printf("\n[fb] ASCII preview (coarse %dx%d)\n", sw, sh);

    for (int ry = 0; ry < sh; ry++) {
        for (int rx = 0; rx < sw; rx++) {
            int x = rx * sx;
            int y = ry * sy;
            uint32 c = fb_mem[y * FB_WIDTH + x];
            // extract RGB and compute simple luminance
            int r = (c >> 16) & 0xff;
            int g = (c >> 8) & 0xff;
            int b = c & 0xff;
            int lum = (30*r + 59*g + 11*b) / 100; // 0..255
            int idx = (lum * (int)(strlen(ascii_map) - 1)) / 255;
            printf("%c", ascii_map[idx]);
        }
        printf("\n");
    }
    printf("[fb] frame\n");
}

void 
fb_print_ascii_if_needed(void)
{
    // ASCII preview disabled for cleaner terminal output
    // Animation still runs in background, just no console visualization
}

// Print ASCII preview on demand (called via syscall)
void
fb_print_ascii_preview(void)
{
    // Note: No lock needed here - framebuffer is read-only during preview
    // Animation updates are protected by spinlock in animation.c
    fb_print_ascii_now();
}


