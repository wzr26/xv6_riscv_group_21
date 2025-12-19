#ifndef FS_H
#define FS_H

#include "types.h"   // required for uint, ushort

//
// On-disk file system format.
// Both the kernel and user programs use this header file.
//

#define ROOTINO  1        // root i-number
#define BSIZE    1024     // block size

// Disk layout:
//
// [ boot block | super block | log | inode blocks |
//   free bit map | data blocks ]
//
// mkfs computes the superblock and builds an initial file system.
struct superblock {
  uint magic;        // Must be FSMAGIC
  uint size;         // Total number of blocks in file system
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

#define FSMAGIC 0x10203040

//
// On-disk inode structure
//
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

struct dinode {
  short type;               // File type
  short major;              // Major device number (T_DEVICE only)
  short minor;              // Minor device number (T_DEVICE only)
  short nlink;              // Number of links to inode in file system
  uint size;                // Size of file (bytes)
  uint addrs[NDIRECT+1];    // Data block addresses
};

//
// Inode / block helpers
//
#define IPB        (BSIZE / sizeof(struct dinode))  // inodes per block
#define IBLOCK(i, sb) ((i) / IPB + sb.inodestart)   // block of inode i

//
// Bitmap helpers
//
#define BPB        (BSIZE * 8)                      // bits per block
#define BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)    // block of free map

//
// Directory entries
//
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ] __attribute__((nonstring));
};

#endif // FS_H

