#include <stdlib.h>
#include <rtos.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/buf.h>

/***************************************************************************
 *
 * mountCreate -
 *
 * RETURNS: OK on success, non-zero otherwise
 */

int mountCreate (
    vfsops_t *  vfsops,
    unsigned    device,
    int         maxFiles,
    char *      devName,
    mount_t **  ppMount
    ) {
    SEM_ID     semId;
    mount_t *  mp;
    vnode_t *  vp;
    filedesc_t *  fdList;
    int        size;
    int        i;
    char *     data;
    u_long *   cookieData;

    if ((vfsops == NULL) || (devName == NULL) || (ppMount == NULL) ||
        (vfsops->devSize <= 0) || (vfsops->inodeSize <= 0) ||
        (maxFiles <= 0)) {
        return (EINVAL);
    }

    /* Allocate memory for the size of the device */
    mp = (mount_t *) malloc (sizeof (mount_t) + vfsops->devSize);
    if (mp == NULL) {
        return (ENOMEM);
    }

    /* Create syncer and file/logical vnodes.  <vp> will be syncer. */
    vp = vnodesCreate (mp, vfsops->inodeSize, maxFiles);
    if (vp == NULL) {
        free (mp);
        return (ENOMEM);
    }

    semId = semMCreate (SEM_Q_PRIORITY);
    if (semId == NULL) {
        vnodesDelete (vp, maxFiles);
        free (mp);
        return (ENOMEM);
    }

    fdList = malloc (sizeof (filedesc_t) +
                     sizeof (u_long) * vfsops->maxCookies);
    if (fdList == NULL) {
        vnodesDelete (vp, maxFiles);
        semDelete (semId);
        free (mp);
    }

    cookieData = (u_long *) &fdList[maxFiles];
    for (i = 0; i < maxFiles; i++) {
        fdList[i].fd_vnode = NULL;             /* not using any vnode */
        fdList[i].fd_mode  = 0;                /* no mode (yet) */
        fdList[i].fd_off   = 0;                /* current file position */
        fdList[i].fd_inuse = FALSE;            /* filedesc not in use */
        fdList[i].fd_num_cookies = 0;          /* no cookies used (yet) */
        fdList[i].fd_cookie_data = cookieData; /* cookie data area */

        cookieData += vfsops->maxCookies;
    }
    
    mp->mnt_ops = vfsops;
    mp->mnt_syncer = vp++;

    listInit (&mp->mnt_vlist);    /* lru list of available vnodes */
    listInit (&mp->mnt_vused);    /* list of used vnodes */
    listInit (&mp->mnt_buflist);  /* lru list of available buffers */

    for (i = 0; i < maxFiles; i++, vp++) {
        listAdd (&mp->mnt_vlist, &vp->v_node);
    }

    mp->mnt_bufsize = 0;    /* No buffers allocated yet */
    mp->mnt_maxfiles = maxFiles;
    mp->mnt_lock = semId;
    mp->mnt_dev = device;
                                        
    data = ((char *) mp) + sizeof (mount_t);
    mp->mnt_data = data;

    return (OK);
}

/***************************************************************************
 *
 * mountBufsCreate - create buffers for the FS
 *
 * This routine allcoates buffers for the file system to use.  It should
 * be called at some time after mountCreate().
 *
 * RETURNS: OK on success, non-zero otherwise
 */

int mountBufsCreate (
    mount_t *  pMount,   /* ptr to mount */
    int        nBufs,    /* # of buffers to create (> 0) */
    int        bufSize   /* size of each buffer (power of two) */
    ) {
    buf_t *  bp;
    char *   data;
    int      i;

    if ((pMount == NULL) || (nBufs <= 0) || (bufSize <= 0) ||
        ((bufSize & (bufSize - 1)) != 0)) {
        return (EINVAL);
    }

    bp = malloc ((sizeof (buf_t) + bufSize) * nBufs);
    if (bp == NULL) {
        return (ENOMEM);
    }

    memset (bp, 0, (sizeof (buf_t) + bufSize) * nBufs);

    pMount->mnt_bufs = bp;
    pMount->mnt_bufsize = bufSize;
    data = (char *) &bp[nBufs];

    for (i = 0; i < nBufs; i++) {
        listAdd (&pMount->mnt_buflist, &bp->b_node);

        bp->b_size = bufSize;
        bp->b_data = data;

/*
 *      b_blkno: leave as 0    (see memset above)
 *      b_flags: leave as 0    (see memset above)
 *      b_count: leave as 0    (see memset above)
 *      b_vp:    leave as NULL (see memset above)
 */

        data += sizeof (buf_t);
    }
    
    return (OK);
}

/***************************************************************************
 *
 * mountUnlock- unlock the mount data structures (such as buffer lists)
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS mountUnlock (
    mount_t *  pMount
    ) {
    STATUS  status;

    status = semGive (pMount->mnt_lock);

    return (status);
}

/***************************************************************************
 *
 * mountLock - lock the mount data structures (such as buffer lists)
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS mountLock (
    mount_t *  pMount
    ) {
    STATUS  status;

    status = semTake (pMount->mnt_lock, WAIT_FOREVER);

    return (status);
}
