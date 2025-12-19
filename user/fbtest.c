// user/fbtest.c
#include "kernel/types.h"
#include "user/user.h"
#include "user/fb.h"

int
main(int argc, char **argv)
{
  int w = 12, h = 6;

  // clear screen
  fb_clear(0x000000);

  for (int t = 0; t < 200; t++) {
    // draw moving filled box
    int xpos = (t % (FB_WIDTH - w));
    fb_clear(0x000000);
    for (int yy = 0; yy < h; yy++) {
      for (int xx = 0; xx < w; xx++) {
        fb_write(xpos + xx, 20 + yy, 0xff2020);
      }
    }
    // pause a bit (ticks)
    pause(10);
  }

  exit(0);
}
