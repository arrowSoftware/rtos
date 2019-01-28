#ifndef _file_h
#define _file_h

struct file {
  enum { FD_CLOSED, FD_NONE, FD_PIPE, FD_FILE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  unsigned int off;
};

#endif /* _file_h */

