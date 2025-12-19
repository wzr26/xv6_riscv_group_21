#ifndef FILE_H
#define FILE_H

#include "types.h"
#include "fs.h"          // NDIRECT, dinode layout
#include "sleeplock.h"   // struct sleeplock

// File descriptor types.
struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int   ref;          // reference count
  char  readable;
  char  writable;
  struct pipe *pipe;  // FD_PIPE
  struct inode *ip;   // FD_INODE or FD_DEVICE
  uint  off;          // read/write offset
  short major;        // FD_DEVICE only
};

// Device major/minor helpers.
#define major(dev)   (((dev) >> 16) & 0xFFFF)
#define minor(dev)   ((dev) & 0xFFFF)
#define mkdev(m, n)  ((uint)((m)<<16 | (n)))

// In-memory inode structure.
struct inode {
  uint   dev;          // Device number
  uint   inum;         // Inode number
  int    ref;          // Reference count
  struct sleeplock lock;  // protects all fields below
  int    valid;           // inode loaded from disk?

  short  type;
  short  major;
  short  minor;
  short  nlink;
  uint   size;
  uint   addrs[NDIRECT+1];
};

// Device switch table.
struct devsw {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
// Framebuffer device major number
#define FB_DEVICE 2

#endif

