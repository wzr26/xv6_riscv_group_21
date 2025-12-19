// user/animctl.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: animctl start|stop|speed <n>\n");
    exit(0);
  }

  if (strcmp(argv[1], "start") == 0) {
    start_anim();
    printf("animctl: start requested\n");
  } else if (strcmp(argv[1], "stop") == 0) {
    stop_anim();
    printf("animctl: stop requested\n");
  } else if (strcmp(argv[1], "speed") == 0) {
    if (argc < 3) {
      printf("animctl: need speed argument\n");
      exit(0);
    }
    int s = atoi(argv[2]);
    if (set_speed(s) < 0) {
      printf("animctl: set speed failed\n");
    } else {
      printf("animctl: speed set to %d\n", s);
    }
  } else {
    printf("animctl: unknown command\n");
  }
  exit(0);
}

