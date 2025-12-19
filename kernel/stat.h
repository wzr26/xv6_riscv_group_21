#ifndef KERNEL_STAT_H
#define KERNEL_STAT_H

// On-disk file system format helper for in-kernel and user headers.
// Simple stat structure used by xv6-style projects.

#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

struct stat {
  int dev;        // File system's disk device
  unsigned int ino;// Inode number
  short type;     // Type of file (T_DIR, T_FILE, T_DEVICE)
  short nlink;    // Number of links to file
  unsigned long long size; // Size of file in bytes (use 64-bit to be safe)
};

#endif /* KERNEL_STAT_H */

