#ifndef _common_h
#define _common_h

#include <fs/defs.h>
#include <fs/file.h>

extern struct file *ofile[NFILE];

extern void panic(char *str);

#endif /* _common_h */

