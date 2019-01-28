#ifndef __STATVFS_H
#define __STATVFS_H

/* includes */

#include <sys/types.h>

/* defines */

/* typedefs */

#define ST_RDONLY   0x0001      /* Read-only file system */
#define ST_NOSUID   0x0002      /* ST_ISUID & ST_ISGID not supported */

/* structs */

struct statvfs {
    unsigned long  f_bsize;    /* file system block size */
    unsigned long  f_frsize;   /* fundamental file system block size */
    fsblkcnt_t     f_blocks;   /* total # of blocks on FS */
    fsblkcnt_t     f_bfree;    /* # of free blocks */
    fsblkcnt_t     f_bavail;   /* # of free blocks available to */
                               /*       non-privileged user */
    fsfilcnt_t     f_files;    /* total # of file identifiers (inodes) */
    fsfilcnt_t     f_ffree;    /* # of free inodes */
    fsfilcnt_t     f_favail;   /* # of free inodes available to */
                               /*      non-privileged user */
    unsigned long  f_fsid;     /* file system ID */
    unsigned long  f_flag;     /* bit mask of f_flag values */
    unsigned long  f_namemax;  /* max length of filename */
};

/* externs */

int  statvfs (const char *path, struct statvfs * st);
int  fstatvfs (int fd, struct statvfs * st);
#endif
