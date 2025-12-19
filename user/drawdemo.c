// user/drawdemo.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "user/fb.h"

// Simple demo that sends rectangle writes to /dev/fb using the
// header format: int x,y,w,h followed by w*h uint32 pixels (row-major).

int
main(int argc, char **argv)
{
  int fd = open("/dev/fb", O_WRONLY);
  if (fd < 0) {
    printf("drawdemo: failed to open /dev/fb\n");
    exit(1);
  }

  int w = 16;
  int h = 16;
  int bufsize = 16 + w * h * 4;
  char *buf = malloc(bufsize);
  if (!buf) {
    printf("drawdemo: alloc failed\n");
    exit(1);
  }

  uint32 *hdr = (uint32 *)buf;
  uint32 *pix = (uint32 *)(buf + 16);

  // initial pixel pattern
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int i = y * w + x;
      // gradient + alpha set
      pix[i] = 0xff000000 | ((x * 255 / w) << 16) | ((y * 255 / h) << 8);
    }
  }

  // animate moving box across the screen
  for (int t = 0; t < 200; t++) {
    int xpos = (t % (FB_WIDTH - w));
    int ypos = 20;
    hdr[0] = xpos; hdr[1] = ypos; hdr[2] = w; hdr[3] = h;

    int written = write(fd, buf, bufsize);
    if (written != bufsize) {
      printf("drawdemo: write returned %d\n", written);
    }

    pause(10);
  }

  close(fd);
  exit(0);
}
