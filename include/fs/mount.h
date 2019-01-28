/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2009 Surplus Users Ham Society
*
*   Real rtos is free software: you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation, either version 2.1 of the License, or
*   (at your option) any later version.
*
*   Real rtos is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with Real rtos.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/* mount.h - mount header file */

/*
 * XXX
 * This file is definitely incomplete.  It is a work in progress.
 * XXX
 */

#ifndef __MOUNT_H
#define __MOUNT_H

/* includes */

#include <rtos.h>
#include <fs/vnode.h>
#include <sys/statvfs.h>
#include <rtos/semLib.h>
#include <util/listLib.h>
#include <io/iosLib.h>
#include <tools/moduleNumber.h>

/* defines */

#define MOUNT_LOCK_EXCLUSIVE  0
#define MOUNT_LOCK_SHARE      1

#define S_vfsLib_NOT_INSTALLED      (M_vfsLib | 0x0001)
#define S_vfsLib_NOT_ENOUGH_VNODES  (M_vfsLib | 0x0002)
#define S_vfsLib_FILEDESC_EXHAUSTED (M_vfsLib | 0x0003)

#define VFS_START(mp, flags)        (mp)->mnt_ops->start (mp, flags)
#define VFS_UNMOUNT(mp, flags)      (mp)->mnt_ops->unmount (mp, flags)
#define VFS_ROOT(mp, vpp)           (mp)->mnt_ops->root (mp, vpp)
#define VFS_STAT_VFS(mp, statvfs)   (mp)->mnt_ops->statvfs (mp, statvfs)
#define VFS_VGET(mp, inode, vpp)    (mp)->mnt_ops->vget (mp, inode, vpp)
#define VFS_INIT(mp)                (mp)->mnt_ops->init (void)
#define VFS_REINIT(mp)              (mp)->mnt_ops->reinit (void)
#define VFS_DONE(mp)                (mp)->mnt_ops->done (void)
#define VFS_TRANS_START(mp, write)  (mp)->mnt_ops->trans_start (mp, write)
#define VFS_TRANS_END(mp, error)    (mp)->mnt_ops->trans_end (mp, error)

/* structs */

struct vnode;
struct mount;

/* typedefs */

typedef struct vfsops {
    char *  typeStr;
    int     devSize;
    int     inodeSize;
    int     maxCookies;
    int     (*start)       (struct mount * pMount, int flags);
    int     (*unmount)     (struct mount * pMount, int flags);
    int     (*root)        (struct mount * pMount, struct vnode ** ppVnode);
    int     (*statvfs)     (struct mount * pMount, struct statvfs *);
    int     (*vget)        (struct mount * pMount, ino_t inode,
                            struct vnode ** ppVnode);
    int     (*init)        (void);
    int     (*reinit)      (void);
    int     (*done)        (void);
    int     (*trans_start) (struct mount * pMount, BOOL writeFlag);
    int     (*trans_end)   (struct mount * pMount, int error);
} vfsops_t;


/*
 * The following structure is used by the VFS to plug into the core IO
 * framework.  The {FD_ENTRY} structure's [value] field with will point
 * to this structure.
 */

typedef struct filedesc {
    struct vnode *  fd_vnode;        /* vnode used by file descriptor */
    int             fd_mode;         /* mode flags with which it was opened */
    off_t           fd_off;          /* position within the file */
    int             fd_num_cookies;  /* # of 32-bit cookies */
    u_long *        fd_cookie_data;  /* set of cookie data */
    BOOL            fd_inuse;        /* TRUE if in use; FALSE otherwise */
} filedesc_t;

typedef struct mount {
    DEV_HEADER          mnt_devhdr;   /* device header should be 1st */
    struct vfsops *     mnt_ops;      /* ptr to VFS operators */
    struct vnode *      mnt_syncer;   /* ptr to syncer vnode */
    struct buf *        mnt_bufs;     /* ptr to all bufs */
    LIST                mnt_vlist;    /* lru list of mount's free vnodes */
    LIST                mnt_vused;    /* list of vnodes in use */
    LIST                mnt_buflist;  /* lru list of mount's free buffers */
    int                 mnt_bufsize;  /* size of each buffer in list */
    int                 mnt_maxfiles; /* max # of open files */
    filedesc_t *        mnt_fdlist;   /* VFS list (array) file descriptors */
    SEM_ID              mnt_lock;     /* semaphore to lock the mount data */
    unsigned            mnt_dev;      /* temporary field as VFS is written */
    char *              mnt_data;     /* FS mount specific data */
} mount_t;

/* externs */

extern int mountCreate (vfsops_t *vfsops, unsigned device, int maxFiles,
                        char *devName, mount_t **ppMount);
extern int mountBufsCreate (mount_t *pMount, int nBufs, int bufSize);

extern STATUS mountLock (mount_t *pMount);
extern STATUS mountUnlock (mount_t *pMount);

#endif
