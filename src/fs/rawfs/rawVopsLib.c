/*
DESCRIPTION:
RawFS treats the entire device as a single file.  It is useful for formatting
the device as well as for reading the contents so as to determine what type of
(if any) file system is installed on it.
*/
/* includes */

/* defines */

#define RAWFS_INODE     2;

/* typedefs */

/* forward declarations */

/* globals */

vnode_ops_t  rawVops =
{
    rawVopLookup,
    rawVopCreate,
    rawVopOpen,
    rawVopClose,
    rawVopAccess,
    rawVopRead,
    rawVopWrite,
    rawVopIoctl,
    rawVopLink,
    rawVopUnlink,
    rawVopSymlink,
    rawVopReadlink,
    rawVopMkdir,
    rawVopRmdir,
    rawVopReaddir,
    rawVopGetAttr,
    rawVopSetAttr,
    rawVopTruncate,
    rawVopFsync,
    rawVopActivate,
    rawVopInactive,
    rawVopPathconf,
    rawVopSeek,
    rawVopRename,
    rawVopAbort,
    rawVopStrategy,
    rawVopPrint
};

/***************************************************************************
 *
 * rawVopLookup - directory lookup
 *
 * On rawFS, the entire device is treated as a single file, so there is no
 * lookup like on other file systems.
 *
 * RETURNS: OK if found, error otherwise
 */

int rawVopLookup (
    lookup_args_t  args
    ) {
    /* 
     * TODO:
     * For now, only return ENOENT.  Later, figure out what is needed to
     * only open the device file.  That may require some changes to the VFS.
     */
    return (ENOENT);
}

/***************************************************************************
 *
 * rawVopCreate - creating a file is not supported for rawFS
 *
 * RETURNS: ENOSYS
 */

int  rawVopCreate (
    create_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopOpen - open a file (has nothing special to do)
 *
 * RETURNS: OK
 */

int rawVopOpen (
    open_args_t  args
    ) {
    return (OK);
}

/***************************************************************************
 *
 * rawVopClose - close a file (has nothing special to do)
 *
 * RETURNS: OK
 */

int  rawVopClose (
    close_args_t  args
    ) {
    /* rawFS does not have any timestamps to update */
    return (OK);
}

/***************************************************************************
 *
 * rawVopAccess - determine if this file can be accessed
 *
 * RETURNS: OK
 */

int  rawVopAccess (
    access_args_t  args
    ) {
    /*
     * rawFS does not have any permission restrictions.  If it did, it would
     * need to call vaccess().
     */

    return (OK);
}

/***************************************************************************
 *
 * rawVopRead - read from rawFS
 *
 * RETURNS: OK on success, errno otherwise
 */

int  rawVopRead (
    read_args_t  args
    ) {
    RAWFS_VOLUME_DESC *  pVolDesc;
    RAWFS_DEV *          pFsDev;
    buf_t *   pBuf;
    lblkon_t  lbn;
    int       error;

    if (args.uio->uio_resid == 0) {    /* If nothing to do, return early. */
        return (OK);
    }

    pFsDev = (RAWFS_DEV *)  args.vp->v_mount->mnt_data;
    pVolDesc = &pFsDev->volumeDesc;

    lbn = (args.uio->uio_offset >> pVolDesc->blkSize2);
    off = (args.uio->uio_offset & (pVolDesc->blkSize - 1));
    bytesToRead = pVolDesc->blkSize - off;
    bytesToEOF  = pVolDesc->diskSize - args.uio->uio_offset;

    while (args.uio->uio_resid > 0) {
        error = bread (args.vp, lbn, pVolDesc->blkSize, NULL, &pBuf);
        if (error != OK) {
            return (error);
        }

        if (bytesToRead > args.uio->uio_resid) {
            bytesToRead = args.uio->uio_resid;
        }

        if (bytesToEOF < bytesToRead) {
            bytesToRead = bytesToEOF;
        }
        uiomove ((char *) pBuf->b_data + off, bytesToRead, args.uio);
        brelse (pBuf);
        lbn++;

        if (bytesToRead == 0) {
            break;
        }

        /* Subsequent reads in this set will be block aligned */

        bytesToEOF -= bytesToRead;
        off = 0;
        bytesToRead = pVolDesc->blkSize;
    }

    return (error);
}

/***************************************************************************
 *
 * rawVopWrite - write to rawFS
 *
 * RETURNS: OK on success, errno otherwise
 */

int  rawVopWrite (
    write_args_t  args
    ) {
    RAWFS_VOLUME_DESC *  pVolDesc;
    RAWFS_DEV *          pFsDev;
    buf_t *   pBuf;
    lblkon_t  lbn;
    int       error;

    if (args.uio->uio_resid == 0) {    /* If nothing to do, return early. */
        return (OK);
    }

    pFsDev = (RAWFS_DEV *)  args.vp->v_mount->mnt_data;
    pVolDesc = &pFsDev->volumeDesc;

    lbn = (args.uio->uio_offset >> pVolDesc->blkSize2);
    off = (args.uio->uio_offset & (pVolDesc->blkSize - 1));
    bytesToEOF  = pVolDesc->diskSize - args.uio->uio_offset;

    while (args.uio->uio_resid > 0) {
        bytesToWrite = pVolDesc->blkSize - off;
        if (bytesToWrite > args.uio->uio_resid) {
            bytesToWrite = args.uio->uio_resid;
        }

        if (bytesToEOF < bytesToWrite) {
            bytesToWrite = bytesToEOF;
        }

        if ((bytesToWrite != pVolDesc->blkSize) && (bytesToWrite != 0)) {
            error = bread (args.vp, lbn, pVolDesc->blkSize, NULL, &pBuf);
            if (error != OK) {
                break;
            }
        } else if (bytesToWrite != 0) {
            pBuf = buf_getblk (args.vp, lbn, pVolDesc->blkSize);
        } else {
            return (EFBIG);
        }

        uiomove ((char *) pBuf->b_data + off, bytesToWrite, args.uio);
        brelse (pBuf);
        lbn++;

        if (bytesToWrite == 0) {
            break;
        }

        /* Subsequent writes in this set will be block aligned */

        bytesToEOF -= bytesToWrite;
        off = 0;
    }

    return (error);
}

/***************************************************************************
 *
 * rawVopIoctl - rawFS specific ioctl() commands
 *
 * RETURNS: value from XBD layer
 */

int  rawVopIoctl (
    ioctl_args_t  args
    ) {
    RAWFS_DEV *  pFsDev;
    int  rv;

    pFsDev = (RAWFS_DEV *) args.vp->v_mount->mnt_data;

    rv = xbdIoctl (pFsDev->volumeDesc.device, arg.cmd, arg.data);

    return (rv);
}

/***************************************************************************
 *
 * rawVopLink - create a hard link (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int rawVopLink (
    link_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopUnlink - unlink a file (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int rawVopUnlink (
    unlink_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopSymlink - create a symlink (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopSymlink (
    symlink_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopReadlink - read a symlink (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopReadlink (
    readlink_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopMkdir - create a directory (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopMkdir (
    mkdir_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopRmdir - remove a directory (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopRmdir (
    rmdir_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopReaddir - read a directory entry (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopReaddir (
    readdir_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawGetAttr - get file attributes
 *
 * RETURNS: OK
 */

int  rawVopGetAttr (
    getattr_args_t  args
    ) {
    RAWFS_DEV *  pFsDev;
    RAWFS_VOLUME_DESC *  pVolDesc;

    pFsDev   = (RAWFS_DEV *) args.vp->v_mount->mnt_data;
    pVolDesc = &pFsDev->volumeDesc;

    args.vap->va_type   = VREG;
    args.vap->va_mode   = 0666;
    args.vap->va_nlink  = 1;
    args.vap->va_uid    = 0;
    args.vap->va_gid    = 0;
    args.vap->va_fsid   = 0;           /* Ignored for now. */
    args.vap->va_fileid = RAWFS_INODE;
    args.vap->va_size   = pVolDesc ...
    args.vap->va_blocksize = pVolDesc->blkSize;
    args.vap->va_atime.tv_sec     = 0;  /* dummy value */
    args.vap->va_mtime.tv_sec     = 0;  /* dummy value */
    args.vap->va_ctime.tv_sec     = 0;  /* dummy value */
    args.vap->va_birthtime.tv_sec = 0;  /* dummy value */
    args.vap->va_flags = 0;
#if 0                /* remaining fields are not yet used */
    args.vap->va_gen = 0;
    args.vap->va_rdev = 0;
    args.vap->va_bytes = 0;
    args.vap->va_filerev = 0;
    args.vap->va_vaflags = 0;
#endif

    return (OK);
}

/***************************************************************************
 *
 * rawVopSetAttr - set file attributes (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int rawVopSetAttr (
    setattr_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopTruncate - change file size (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopTruncate (
    truncate_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopFsync - fsync (has nothing to do as rawFS is synchronous)
 *
 * RETURNS: OK
 */

int  rawVopFsync (
    fsync_args_t  args
    ) {
    return (OK);
}

/***************************************************************************
 *
 * rawVopActivate - activate the vnode
 *
 * RETURNS: OK
 */

int  rawVopActivate (
    activate_args_t  args
    ) {
    args.vp->v_type = VREG;
    args.vp->v_ops  = &rawVops;

    return (OK);
}

/***************************************************************************
 *
 * rawVopInactive - deactivate the vnode (has nothing to do)
 *
 * RETURNS: OK
 */

int  rawVopInactive (
    inactive_args_t  args
    ) {
    return (OK);
}

/***************************************************************************
 *
 * rawVopPathconf - pathconf() not applicable to rawFS
 *
 * RETURNS: EINVAL
 */

int  rawVopPathconf (
    pathconf_args_t  args
    ) {
    return (EINVAL);
}

/***************************************************************************
 *
 * rawVopSeek - seek (nothing to do as it is handled by VFS layer)
 *
 * RETURNS: OK
 */

int  rawVopSeek (
    seek_args_t  args
    ) {
    return (OK);    /* nothing to do for seek operation */
}

/***************************************************************************
 *
 * rawVopRename - rename file (not supported in rawFS)
 *
 * RETURNS: ENOSYS
 */

int  rawVopRename (
    rename_args_t  args
    ) {
    return (ENOSYS);
}

/***************************************************************************
 *
 * rawVopAbort - not yet used by VFS
 *
 * RETURNS: OK
 */

int  rawVopAbort (
    abort_args_t  args
    ) {
    return (OK);
}

/***************************************************************************
 *
 * rawVopStrategy - strategy routine for rawFS
 *
 * RETURNS: N/A
 */

void rawVopStrategy (
    strategy_args_t  args
    ) {
    printf("HEREaaaaaaaaaa\n");
    RAWFS_DEV *          pFsDev;
    RAWFS_VOLUME_DESC *  pVolDesc;
    struct bio *         pBio;

    pFsDev   = (RAWFS_DEV *) args.vp->v_mount->mnt_data;
    pVolDesc = &pFsDev->volumeDesc;

    /*
     * The logical block number is the same as the physical block number
     * in rawFS.  Thus no translation is required between the two.
     */

    if (pBuf->b_flags & B_READ) {
    }

    pBio = pBuf->b_bio;
    pBio->bio_dev    = pVolDesc->device;
    pBio->bio_blkno  = (pBuf->b_lbnlkno << pVolDesc->secPerBlk2);
    pBio->bio_bcount = pVolDesc->blkSize;
    pBio->bio_error  = OK;
    pBio->bio_flags  = (pBuf->b_flags & B_READ) ? BIO_READ : BIO_WRITE;
    pBio->bio_data   = pBuf->b_data;
    pBio->bio_done   = ...;
    pBio->bio_resid  = 0;

    xbdStrategy (pVolDesc->device, pBio);
    semTake (semId, WAIT_FOREVER);

    buf_done (pBuf, OK);
}

/***************************************************************************
 *
 * rawVopPrint - print a vnode for debugging (nothing to do)
 *
 * RETURNS: N/A
 */

void rawVopPrint (
    print_args_t  args
    ) {
    return;
}
