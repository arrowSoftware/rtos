#ifndef _dirent_h
#define _dirent_h

#include <stdio.h>		/* FILENAME_MAX defined here */

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ			FILENAME_MAX

typedef struct dir_stream DIR;     /* Opaque type (see dirLib.c) */

struct dirent {
    ino_t  d_ino;
    char   d_name[FILENAME_MAX + 1];
};

extern int closedir (DIR *);
extern DIR *  opendir (const char *);
extern struct dirent *  readdir (DIR *);
extern void rewinddir (DIR *);
extern void seekdir (DIR *, long);
extern long telldir (DIR *);

#endif /* _dirent_h */
