#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void printint(int fd, long long xx, int base, int sgn)
{
  char buf[20];
  int i, neg;
  unsigned long long x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

static void printptr(int fd, uint64 x)
{
  int i;
  putc(fd, '0');
  putc(fd, 'x');
  for(i = 0; i < sizeof(uint64) * 2; i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

void vprintf(int fd, const char *fmt, va_list ap)
{
  char *s;
  int c, i;

  for(i = 0; fmt[i]; i++){
    c = fmt[i];
    if(c != '%'){
      putc(fd, c);
      continue;
    }
    c = fmt[++i];
    if(!c) break;

    if(c == 'd')
      printint(fd, va_arg(ap, int), 10, 1);
    else if(c == 'x')
      printint(fd, va_arg(ap, int), 16, 0);
    else if(c == 'p')
      printptr(fd, va_arg(ap, uint64));
    else if(c == 's'){
      s = va_arg(ap, char*);
      if(!s) s = "(null)";
      while(*s) putc(fd, *s++);
    } else if(c == 'c')
      putc(fd, va_arg(ap, int));
    else if(c == '%')
      putc(fd, '%');
    else {
      putc(fd, '%');
      putc(fd, c);
    }
  }
}

void
fprintf(int fd, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fd, fmt, ap);
    va_end(ap);
}

int
printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(1, fmt, ap);   // stdout = fd 1
    va_end(ap);
    return 0;
}

