// user/libfb.c
// User-space framebuffer graphics library implementation (Week 5)

#include "user.h"
#include "libfb.h"

// File descriptor for /dev/fb
static int fb_fd = -1;

// User-space pixel buffer for batch operations
static unsigned int fb_buffer[FB_WIDTH * FB_HEIGHT];

void
libfb_init(void)
{
    fb_fd = open("/dev/fb", O_RDWR);
    if(fb_fd < 0) {
        printf("Error: cannot open /dev/fb\n");
        return;
    }
    libfb_clear(0x000000);
}

void
libfb_close(void)
{
    if(fb_fd >= 0) {
        close(fb_fd);
        fb_fd = -1;
    }
}

void
libfb_clear(unsigned int color)
{
    if(fb_fd < 0) return;
    
    // Fill buffer with color
    for(int i = 0; i < FB_WIDTH * FB_HEIGHT; i++) {
        fb_buffer[i] = color;
    }
    
    // Write to device
    write(fb_fd, (char*)fb_buffer, FB_WIDTH * FB_HEIGHT * sizeof(unsigned int));
}

void
libfb_draw_pixel(int x, int y, unsigned int color)
{
    if(fb_fd < 0) return;
    if(x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) return;
    
    fb_buffer[y * FB_WIDTH + x] = color;
}

void
libfb_draw_rect(int x, int y, int w, int h, unsigned int color)
{
    for(int yy = y; yy < y + h; yy++) {
        for(int xx = x; xx < x + w; xx++) {
            libfb_draw_pixel(xx, yy, color);
        }
    }
    
    // Flush to device after drawing
    write(fb_fd, (char*)fb_buffer, FB_WIDTH * FB_HEIGHT * sizeof(unsigned int));
}

void
libfb_draw_line(int x0, int y0, int x1, int y1, unsigned int color)
{
    // Bresenham's line algorithm
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
            libfb_draw_pixel(x, y, color);
            err -= dy;
            if (err < 0) { y += sy; err += dx; }
        }
        libfb_draw_pixel(x1, y1, color);
    } else {
        int err = dy/2;
        int x = x0;
        for (int y = y0; y != y1; y += sy) {
            libfb_draw_pixel(x, y, color);
            err -= dx;
            if (err < 0) { x += sx; err += dy; }
        }
        libfb_draw_pixel(x1, y1, color);
    }
    
    write(fb_fd, (char*)fb_buffer, FB_WIDTH * FB_HEIGHT * sizeof(unsigned int));
}

void
libfb_draw_circle(int cx, int cy, int r, unsigned int color)
{
    // Midpoint circle algorithm
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        libfb_draw_pixel(cx + x, cy + y, color);
        libfb_draw_pixel(cx + y, cy + x, color);
        libfb_draw_pixel(cx - y, cy + x, color);
        libfb_draw_pixel(cx - x, cy + y, color);
        libfb_draw_pixel(cx - x, cy - y, color);
        libfb_draw_pixel(cx - y, cy - x, color);
        libfb_draw_pixel(cx + y, cy - x, color);
        libfb_draw_pixel(cx + x, cy - y, color);

        y += 1;
        if (err <= 0) {
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
    
    write(fb_fd, (char*)fb_buffer, FB_WIDTH * FB_HEIGHT * sizeof(unsigned int));
}

void
libfb_draw_box(int x, int y, int w, int h, unsigned int color)
{
    // Draw outline of box
    libfb_draw_line(x, y, x + w - 1, y, color);
    libfb_draw_line(x, y, x, y + h - 1, color);
    libfb_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
    libfb_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
}

int
libfb_width(void)
{
    return FB_WIDTH;
}

int
libfb_height(void)
{
    return FB_HEIGHT;
}

void
libfb_show_ascii_preview(void)
{
    // Read framebuffer and display ASCII preview
    int sw = 64;
    int sh = 16;
    int sx = FB_WIDTH / sw;
    int sy = FB_HEIGHT / sh;

    printf("\n[fb] ASCII preview (coarse %dx%d)\n", sw, sh);

    for (int ry = 0; ry < sh; ry++) {
        for (int rx = 0; rx < sw; rx++) {
            int x = rx * sx;
            int y = ry * sy;
            unsigned int c = fb_buffer[y * FB_WIDTH + x];
            
            int r = (c >> 16) & 0xff;
            int g = (c >> 8) & 0xff;
            int b = c & 0xff;
            int lum = (30*r + 59*g + 11*b) / 100;
            
            char ascii_map[] = " .:-=+*#%@";
            int idx = (lum * 9) / 255;
            if(idx > 9) idx = 9;
            printf("%c", ascii_map[idx]);
        }
        printf("\n");
    }
}

void
libfb_fill_color(unsigned int color)
{
    libfb_clear(color);
}

void
libfb_test_pattern(void)
{
    for(int y = 0; y < FB_HEIGHT; y++) {
        for(int x = 0; x < FB_WIDTH; x++) {
            fb_buffer[y * FB_WIDTH + x] = (x * 5) ^ (y * 7);
        }
    }
    write(fb_fd, (char*)fb_buffer, FB_WIDTH * FB_HEIGHT * sizeof(unsigned int));
}

void
libfb_draw_gradient(int x, int y, int w, int h)
{
    for(int yy = y; yy < y + h; yy++) {
        for(int xx = x; xx < x + w; xx++) {
            int r = (xx * 255) / w;
            int g = (yy * 255) / h;
            int b = 128;
            unsigned int color = (r << 16) | (g << 8) | b;
            libfb_draw_pixel(xx, yy, color);
        }
    }
    write(fb_fd, (char*)fb_buffer, FB_WIDTH * FB_HEIGHT * sizeof(unsigned int));
}

unsigned long
libfb_get_ticks(void)
{
    // Placeholder for getting system ticks
    return 0;
}

void
libfb_profile_start(void)
{
    // Profiling placeholder
}

void
libfb_profile_end(void)
{
    // Profiling placeholder
}
