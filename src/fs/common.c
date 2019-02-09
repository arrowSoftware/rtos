#include <stdio.h>
#include <stdlib.h>

#include <fs/param.h>
#include <fs/fs.h>
#include <fs/fsvar.h>
#include <fs/file.h>

struct inode *cwd;
struct file *ofile[NFILE];
/*
void panic(char *msg)
{
  fprintf(stderr, "PANIC: %s\n", msg);
  exit(1);
}
*/
