#include <rtos.h>

#include <stdlib.h>

#include <fs/xbd.h>
#include <rtos/semLib.h>

#define XBD_MIN_BLKSIZE   512
#define XBD_MAX_BLKSIZE   8096

typedef struct {
    XBD           xbd;
    XBD_GEOMETRY  geometry;
    SEMAPHORE     mutex;
    char *        name;
    char *        data;
} XBD_RAMDISK;

LOCAL int xbdRamDiskIoctl (XBD *xbd, unsigned  command, void *data);
LOCAL int xbdRamDiskStrategy (XBD *xbd, struct bio *pBio);
LOCAL int xbdRamDiskDump (XBD *xbd, lblkno_t lblkno, void *data, size_t nBytes);

LOCAL XBD_FUNCS xbdRamDiskFuncs = {
    &xbdRamDiskIoctl,
    &xbdRamDiskStrategy,
    &xbdRamDiskDump,
};

/***************************************************************************
 *
 * xbdRamDiskDevCreate - create an XBD ramdisk device
 *
 * RETURNS: XBD's device ID on success, NULLDEV on error
 */

device_t xbdRamDiskDevCreate (
    unsigned      blockSize,   /* block size (bytes) */
    unsigned      diskSize,    /* disk size (bytes) */
    BOOL          flag,        /* TRUE if partitions supported, else FALSE */
    const char *  name         /* ramdisk name */
    ) {
    XBD_RAMDISK *  xbdRamDisk;
    STATUS         status;
    device_t       device;
    int            len;
    unsigned       numBlocks;

    /*
     * Fail if ...
     * 1. <blockSize> is not a power of two or is out of range.
     * 2. <name> is NULL or does not start with '/'.
     * 3. <name> has more than one '/'.
     */

    if (((blockSize & (blockSize - 1)) != 0) || (diskSize < XBD_MIN_BLKSIZE) ||
        (blockSize < XBD_MIN_BLKSIZE) || (blockSize > XBD_MAX_BLKSIZE) ||
        (name == NULL) || (name[0] != '/') ||
        (strchr (name + 1, '/') != NULL)) {
        errnoSet (EINVAL);
        return (NULLDEV);
    }

    if ((len = strlen (name)) >= XBD_MAX_NAMELEN - 4) {
        errnoSet (EINVAL);
        return (NULLDEV);
    }

    /* Allocate memory for XBD_RAMDISK structure, name and ramdisk data. */
    xbdRamDisk = (XBD_RAMDISK *)
                 malloc (diskSize + sizeof (XBD_RAMDISK) + len + 4);
    if (xbdRamDisk == NULL) {
        /* errno set by malloc() */
        return (NULLDEV);
    }

    /*
     * Copy the name to the XBD_RAMDISK.  If this device can be
     * partitioned, then append ":0" to it.
     */

    xbdRamDisk->name = (char *) &xbdRamDisk[1];
    strncpy (xbdRamDisk->name, name, len);
    if (flag) {
        strcpy (xbdRamDisk->name + len, ":0");
    }

    semMInit (&xbdRamDisk->mutex, SEM_Q_PRIORITY);

    numBlocks = diskSize / blockSize;

    /*
     * The geometry information must be faked for dosfs (aka FAT file systems).
     * An arbitrary (power of two) number has been chosen for the number of
     * sectors per track.
     */

    xbdRamDisk->geometry.sectorsPerTrack = 32;
    xbdRamDisk->geometry.numHeads        = 1;
    xbdRamDisk->geometry.numCylinders    = numBlocks /
                                           xbdRamDisk->geometry.sectorsPerTrack;

    xbdRamDisk->data = &xbdRamDisk->name[len + 4];
    memset (xbdRamDisk->data, 0, diskSize + sizeof (XBD_RAMDISK));

    status = xbdAttach (&xbdRamDisk->xbd, &xbdRamDiskFuncs, name, blockSize,
                        numBlocks, &device);

    if (status != OK) {
        /* errno set by xbdAttach() */
        free (xbdRamDisk);
        return (NULLDEV);
    } 

    status = erfEventRaise (xbdEventCategory, xbdEventPrimaryInsert,
                            (void *) device, NULL);

    if (status != OK) {
        /* TODO: Need an xbdDetach() I think */
        free (xbdRamDisk);
        return (NULLDEV);
    }


    /*
     * TODO:
     * Eventually some other steps will be required to wait for any devices
     * on top of this to initialize (such as partitions or file systems).
     * If the ramdisk was created (as opposed to initialized), then we can
     * expect it to be mounted with rawFS.
     */

    return (device);
}

/***************************************************************************
 *
 * xbdRamDiskDevDelete - delete an XBD ramdisk device
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS xbdRamDiskDevDelete (
    device_t  device
    ) {
    return (ERROR);
}

/***************************************************************************
 *
 * xbdRamDiskIoctl -
 *
 * RETURNS: OK on success, error otherwise
 */

LOCAL int xbdRamDiskIoctl (
    XBD *     xbd,
    unsigned  command,
    void *    data
    ) {
    XBD_RAMDISK *  xbdRamDisk = (XBD_RAMDISK *) xbd;
    XBD_GEOMETRY * xbdGeometry;

    switch (command) {
        case XBD_GEOMETRY_GET:
            if (data == NULL) {
                return (EINVAL);
            }
            xbdGeometry = (XBD_GEOMETRY *) data;
            memcpy (xbdGeometry, &xbdRamDisk->geometry, sizeof (XBD_GEOMETRY));
            break;

        default:
            return (ENOSYS);
    }

    return (OK);
}

/***************************************************************************
 *
 * xbdRamDiskStrategy -
 *
 * RETURNS: OK on success, error otherwise
 */

LOCAL int xbdRamDiskStrategy (
    XBD *         xbd,
    struct bio *  pBio
    ) {
    XBD_RAMDISK *  xbdRamDisk = (XBD_RAMDISK *) xbd;
    lblkno_t       lblkno;
    char *         pData;

    /* LATER: Add BIO parameter to checks. */
    
    semTake (&xbdRamDisk->mutex, WAIT_FOREVER);

    if (pBio->bio_flags == BIO_READ) {
        for (; pBio != NULL; pBio = pBio->bio_chain) {
            pData = xbdRamDisk->data +
                    (pBio->bio_blkno * xbdRamDisk->xbd.blkSize);
            memcpy  (pBio->bio_data, pData, pBio->bio_bcount);
        }
    } else {
        for (; pBio != NULL; pBio = pBio->bio_chain) {
            pData = xbdRamDisk->data +
                    (pBio->bio_blkno * xbdRamDisk->xbd.blkSize);
            memcpy  (pData, pBio->bio_data, pBio->bio_bcount);
        }
    }
    semGive (&xbdRamDisk->mutex);

    pBio->bio_error = OK;
    pBio->bio_resid = OK;
    pBio->bio_done (pBio->bio_param);   /* BIO operation is done. */

    return (OK);
}

/***************************************************************************
 *
 * xbdRamDiskDump - 
 *
 * RETURNS: OK on success, error otherwise
 */

LOCAL int xbdRamDiskDump (
    XBD *       xbd,
    lblkno_t    lblkno,
    void *      data,
    size_t      nBytes
    ) {
    XBD_RAMDISK *  xbdRamDisk = (XBD_RAMDISK *) xbd;
    char *         pData;

    /*
     * xbdDump() may get called upon some sort of critical system error.
     * At such a time, the kernel may have suffered some unrecoverable
     * error, so it may not be safe to use kernel primitives such as
     * semTake() or semGive().
     */

    pData = xbdRamDisk->data + (lblkno * xbdRamDisk->xbd.blkSize);
    memcpy (pData, data, nBytes);

    return (OK);
}
