// user/hello.c
// Simple user program to call the hello() system call

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(void)
{
  // Gọi system call hello() (chạy trong kernel mode)
  hello();

  // Dòng này in ra từ user mode
  printf("user: hello() returned, back in user mode\n");

  exit(0);
}

