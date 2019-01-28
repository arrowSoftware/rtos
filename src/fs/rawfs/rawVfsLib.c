/*
 * This file references structures that have not yet been defined.  As such,
 * these names are subject to change when the structures actually do get
 * declared.  That is, there may be typos and inconsistencies.
 */

LOCAL int rawVfsStart       (mount_t * pMount, int flags);
LOCAL int rawVfsUnmount     (mount_t * pMount, int flags);
LOCAL int rawVfsRoot        (mount_t * pMount, vnode_t ** ppVnode);
LOCAL int rawVfsStatVfs     (mount_t * pMount, struct statvfs *);
LOCAL int rawVfsVget        (mount_t * pMount, ino_t inode, vnode_t ** ppVnode);
LOCAL int rawVfsInit        (void);
LOCAL int rawVfsReInit      (void);
LOCAL int rawVfsDone        (void);
LOCAL int rawVfsTransStart (mount_t * pMount, BOOL writeFlag);
LOCAL int rawVfsTransEnd   (mount_t * pMount, int error);

const vfsops_t rawVfsOps =
    {
    "rawFS",
    sizeof (RAWFS_DEV),
    sizeof (RAWFS_INODE),
    1,
    rawVfsStart,
    rawVfsUnmount,
    rawVfsRoot,
    rawVfsStatVfs,
    rawVfsVget,
    rawVfsInit,
    rawVfsReInit,
    rawVfsDone,
    rawVfsTransStart,
    rawVfsTransEnd
    };



/***************************************************************************
 *
 * rawVfsStart -
 *
 * RETURNS: OK on success, error otherwise
 */

int rawVfsStart
    (
    mount_t *  pMount,   /* pointer to mount */
    int        flags     /* not used */
    )
    {
    RAWFS_DEV *    pFsDev;
    int             error;

    pFsDev = (RAWFS_DEV *) pMount->mnt_data;

    error = rawfsMount (pMount);
    if (error != OK)
        {
        /* TODO: some type of cleanup routine may need to be called */
        return (error);
        }

    error = mountBufAlloc (pMount, RAWFS_MIN_BUFFERS,
                           pFsDev->volDesc.blkSize);
    if (error != OK)
        {
        /* TODO: some type of cleanup routine may need to be called */
        return (error);
        }

    return (error);
    }

/***************************************************************************
 *
 * rawVfsUnmount -
 *
 * RETURNS: OK on success, error otherwise
 */

int rawVfsUnmount
    (
    mount_t *  pMount,   /* pointer to mount */
    int        flags     /* not used */
    )
    {
    RAWFS_DEV *  pFsDev;

    pFsDev = (RAWFS_DEV *) pMount->mnt_data;

    /*
     * TODO: some type of cleanup routine must be called.
     * For simplicity, make it the same one as the others above.
     */

    pMount->mnt_data = NULL;

    return (OK);
    }

/***************************************************************************
 *
 * rawVfsVget -
 *
 * RETURNS: OK on success, error otherwise
 */

int rawVfsVget
    (
    mount_t *   pMount,   /* pointer to mount */
    ino_t       inode,    /* inode number */
    vnode_t **  ppVnode   /* return pointer to vnode here */
    )
    {
    /*
     * rawFS treats the media (disk) as a single file.  Thus, there are no
     * directories, and no files other than the "root" entry.
     */
    if (inode != RAWFS_ROOT_INODE) {
        return (EINVAL);
    }
    return (vgetino (pMount, inode, rawOps, ppVnode));
    }

/***************************************************************************
 *
 * rawVfsRoot -
 *
 * RETURNS: OK on success, error otherwise
 */

int rawVfsRoot
    (
    mount_t *   pMount,   /* pointer to mount */
    vnode_t **  ppVnode   /* return pointer to vnode here */
    )
    {
    return (rawVfsVget (pMount, RAWFS_ROOT_INODE, ppVnode));
    }

/***************************************************************************
 *
 * rawVfsStatVfs -
 *
 * RETURNS: OK on success, error otherwise
 */

int rawVfsStatVfs
    (
    mount_t *    pMount,   /* pointer to mount */
    statvfs_t *  pStatVfs  /* pointer to VFS stat structure */
    )
    {
    return (rawStat (pMount, pStatVfs));
    }

/***************************************************************************
 *
 * rawVfsInit -
 *
 * RETURNS: N/A
 */

void rawVfsInit (void)
    {
    return;
    }

/***************************************************************************
 *
 * rawVfsReInit -
 *
 * RETURNS: N/A
 */

void rawVfsReInit (void)
    {
    return;
    }

/***************************************************************************
 *
 * rawVfsDone -
 *
 * RETURNS: N/A
 */

void rawVfsDone (void)
    {
    return;
    }

/***************************************************************************
 *
 * rawVfsTransStart -
 *
 * RETURNS: N/A
 */

int rawVfsTransStart
    (
    struct mount *  pMount,     /* pointer to mount */
    BOOL            writeFlag   /* TRUE if transaction will write */
    )
    {
    RAWFS_VOLUME_DESC *  pVolDesc;
    RAWFS_DEV *          pFsDev;
    int                   error;

    /*
     * Normally, if <writeFlag> were FALSE, then a shared lock could be used,
     * and if it were TRUE then an exclusive lock could be used.  However, as
     * the file system is currently single threaded (or at the time of writing
     * this, it is supposed to be/planned to be), the type of lock does not
     * make much of a difference; this makes <writeFlag> somewhat superfluous
     * for the time being.  For now, just take an exclusive lock (mutex) and
     * accept the hit on the read scenario.
     *
     * TODO: Actually take the lock.
     */

    error = vnodeLock (pMount->mnt_syncer);

    if (error == OK)
        {
        pFsDev   = (RAWFS_DEV *) pMount->mnt_data;
        pVolDesc = &pFsDev->volDesc;
        pVolDesc->diskModified = FALSE;
        }

    return (error);
    }

/***************************************************************************
 *
 * rawVfsTransEnd -
 *
 * RETURNS: N/A
 */

int rawVfsTransEnd
    (
    struct mount *  pMount,    /* pointer to mount */
    int             error      /* previous error */
    )
    {
    if (error != OK)
        {
        vnodeUnlock (pMount->mnt_syncer);
        return (error);
        }

    /*
     * When delayed or asynchronous writes are implemented, there are two
     * options.
     * #1.  Flush any pended writes here and now.
     * #2.  Flush any pended writes when the FS is unmounted/ejected.
     * At the present time, the idea is to use option #2 (when it gets done).
     */

    vnodeUnlock (pMount->mnt_syncer);
    return (OK);
    }

