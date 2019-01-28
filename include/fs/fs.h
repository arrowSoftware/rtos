// On-disk file system format. 
// This header is shared between kernel and user space.

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.

#ifndef _fs_h
#define _fs_h

#include <fs/vnode.h>
#include <fs/mount.h>

#define BSIZE 512  // block size

// File system super block
struct superblock {
  unsigned int size;         // Size of file system image (blocks)
  unsigned int nblocks;      // Number of data blocks
  unsigned int ninodes;      // Number of inodes.
};

#define NADDRS (NDIRECT+1)
#define NDIRECT 12
#define INDIRECT 12
#define NINDIRECT (BSIZE / sizeof(unsigned int))
#define MAXFILE (NDIRECT  + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           	// File type
  short major;          	// Major device number (T_DEV only)
  short minor;          	// Minor device number (T_DEV only)
  short nlink;          	// Number of links to inode in file system
  unsigned int size;    	// Size of file (bytes)
  unsigned int addrs[NADDRS];   // Data block addresses
};

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Special device

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i)     ((i) / IPB + 2)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block containing bit for block b
#define BBLOCK(b, ninodes) (b/BPB + (ninodes)/IPB + 3)

extern struct vnode *  pRootSyncer;
extern struct mount *  pRootMount;

#endif /* _fs_h */

