// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  // Ensure framebuffer device node exists for demos (/dev/fb -> major 2)
  // Create /dev directory if necessary, then create device node
  mkdir("/dev");
  if (open("/dev/fb", O_RDWR) < 0) {
    mknod("/dev/fb", 2, 0);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  // Launch drawdemo in background for automated demo captures.
  // This is optional and intended for automated test runs.
  if (fork() == 0) {
    char *dargs[] = { "drawdemo", 0 };
    exec("drawdemo", dargs);
    // if exec fails, child should exit
    exit(0);
  }
  // Animation will be started manually via 'animctl start' command
  // start_anim();

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
