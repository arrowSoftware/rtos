// in-core file system types

#ifndef _fsvar_h
#define _fsvar_h

struct inode {
  unsigned int dev;           // Device number
  unsigned int inum;          // Inode number
  int ref;            	      // Reference count
  int busy;                   // Is the inode "locked"?

  short type;         	      // copy of disk inode
  short major;
  short minor;
  short nlink;
  unsigned int size;
  unsigned int addrs[NADDRS];
};

extern unsigned int rootdev;

#define NAMEI_LOOKUP 1
#define NAMEI_CREATE 2
#define NAMEI_DELETE 3

#endif /* _fsvar_h */

