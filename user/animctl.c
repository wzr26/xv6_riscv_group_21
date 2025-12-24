// user/animctl.c
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: animctl start|stop|speed <n>|view\n");
    exit(0);
  }

  if (strcmp(argv[1], "start") == 0) {
    start_anim();
  } else if (strcmp(argv[1], "stop") == 0) {
    stop_anim();
  } else if (strcmp(argv[1], "speed") == 0) {
    if (argc < 3) {
      printf("animctl: need speed argument\n");
      exit(0);
    }
    set_speed(atoi(argv[2]));
  } else if (strcmp(argv[1], "view") == 0) {
    while (1) {
      view_anim();
      pause(100);
    }
  }
  exit(0);
}
