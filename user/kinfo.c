#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(void)
{
  printf("user: calling kinfo syscall...\n");
  kinfo();
  printf("user: returned from kinfo syscall.\n");
  exit(0);
}

