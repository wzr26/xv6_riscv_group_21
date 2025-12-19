// user/animtest.c
// Automated animation test - runs on boot and captures output
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("\n=== ANIMATION TEST START ===\n");
  printf("Starting kernel animation...\n\n");

  // Start animation
  start_anim();

  // Let it run for several seconds (approximately)
  // At default tick rate, each tick is ~0.1ms, so 50000 ticks ~ 5 seconds
  pause(50000);

  printf("\n=== STOPPING ANIMATION ===\n\n");

  // Stop animation
  stop_anim();

  printf("=== ANIMATION TEST COMPLETE ===\n");
  printf("Test output captured above. Now exiting to shell.\n\n");

  exit(0);
}
