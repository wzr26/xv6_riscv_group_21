// user/fbviewer.c
// User-space framebuffer viewer and graphics demo application (Week 5)

#include "user.h"
#include "libfb.h"

#define COLOR_RED     0xff0000
#define COLOR_GREEN   0x00ff00
#define COLOR_BLUE    0x0000ff
#define COLOR_YELLOW  0xffff00
#define COLOR_CYAN    0x00ffff
#define COLOR_MAGENTA 0xff00ff
#define COLOR_WHITE   0xffffff
#define COLOR_BLACK   0x000000

void
draw_demo_scene(void)
{
    // Clear screen to black
    libfb_clear(COLOR_BLACK);
    
    // Draw colored rectangles in corners
    libfb_draw_rect(2, 2, 20, 20, COLOR_RED);
    libfb_draw_rect(FB_WIDTH - 22, 2, 20, 20, COLOR_GREEN);
    libfb_draw_rect(2, FB_HEIGHT - 22, 20, 20, COLOR_BLUE);
    libfb_draw_rect(FB_WIDTH - 22, FB_HEIGHT - 22, 20, 20, COLOR_YELLOW);
    
    // Draw circles
    libfb_draw_circle(FB_WIDTH/2, FB_HEIGHT/2, 15, COLOR_CYAN);
    libfb_draw_circle(FB_WIDTH/2, FB_HEIGHT/2, 10, COLOR_MAGENTA);
    libfb_draw_circle(FB_WIDTH/2, FB_HEIGHT/2, 5, COLOR_WHITE);
    
    // Draw boxes (outlines)
    libfb_draw_box(30, 30, 40, 40, COLOR_GREEN);
    libfb_draw_box(FB_WIDTH - 70, 30, 40, 40, COLOR_RED);
    
    // Draw a diagonal line
    libfb_draw_line(0, 0, FB_WIDTH-1, FB_HEIGHT-1, COLOR_CYAN);
    libfb_draw_line(FB_WIDTH-1, 0, 0, FB_HEIGHT-1, COLOR_MAGENTA);
}

void
draw_animation_demo(void)
{
    // Simple animation: moving rectangle
    int x = 10, y = 10;
    int dx = 1, dy = 1;
    
    for(int frame = 0; frame < 50; frame++) {
        libfb_clear(COLOR_BLACK);
        
        // Draw moving rectangle
        libfb_draw_rect(x, y, 15, 15, COLOR_RED);
        
        // Draw border
        libfb_draw_box(0, 0, FB_WIDTH, FB_HEIGHT, COLOR_WHITE);
        
        // Update position
        x += dx;
        y += dy;
        
        // Bounce off walls
        if(x <= 0 || x + 15 >= FB_WIDTH) dx = -dx;
        if(y <= 0 || y + 15 >= FB_HEIGHT) dy = -dy;
        
        // Small delay (busy wait for demo)
        for(int i = 0; i < 100000; i++);
    }
}

void
draw_test_pattern(void)
{
    // Gradient pattern
    libfb_draw_gradient(0, 0, FB_WIDTH, FB_HEIGHT);
}

int
main(int argc, char *argv[])
{
    printf("fbviewer: Framebuffer Graphics Viewer (Week 5)\n");
    
    libfb_init();
    
    if(argc < 2) {
        printf("Usage: fbviewer <mode>\n");
        printf("  demo       - Draw static demo scene\n");
        printf("  animate    - Draw animation\n");
        printf("  pattern    - Draw test pattern\n");
        printf("  gradient   - Draw gradient\n");
        printf("  preview    - Show ASCII preview\n");
        libfb_close();
        exit(0);
    }
    
    char *mode = argv[1];
    
    if(strcmp(mode, "demo") == 0) {
        printf("Drawing demo scene...\n");
        draw_demo_scene();
        libfb_show_ascii_preview();
    } else if(strcmp(mode, "animate") == 0) {
        printf("Running animation demo...\n");
        draw_animation_demo();
    } else if(strcmp(mode, "pattern") == 0) {
        printf("Drawing test pattern...\n");
        libfb_test_pattern();
        libfb_show_ascii_preview();
    } else if(strcmp(mode, "gradient") == 0) {
        printf("Drawing gradient...\n");
        libfb_draw_gradient(10, 10, 100, 100);
        libfb_show_ascii_preview();
    } else if(strcmp(mode, "preview") == 0) {
        printf("Showing ASCII preview...\n");
        libfb_show_ascii_preview();
    } else {
        printf("Unknown mode: %s\n", mode);
    }
    
    libfb_close();
    exit(0);
}
