/* rawfsLib.c - raw file system library */

/* includes */

#include <rtos.h>

#include <fs/erfLib.h>
#include <fs/xbd.h>
#include <fs/rawfsLib.h>

/*
 * TODO:
 * As rawfs is the default file system and must be in the image if file systems
 * are to be used, it is unclear at this point if any special functionality
 * or cases must be considered when ejecting or deleting the raw file system
 * device.  More thought must be given to this.
 */

LOCAL BOOL rawfsLibInitialized = FALSE;
LOCAL int  rawfsMaxBuffers;
LOCAL int  rawfsMaxFiles;

/***************************************************************************
 *
 * rawfsEject - eject the faw file system
 *
 * RETURNS: N/A
 */

LOCAL void rawfsEject (
    int     category,
    int     type,
    void *  pEventData,   /* device to eject */
    void *  pUserData     /* ptr to device structure */
    ) {
    device_t     device;
    RAWFS_DEV *  pFsDev;

    if ((category != xbdEventCategory) ||
        ((type != xbdEventRemove) && (type != xbdEventMediaChanged))) {
        return;
    }

    device = (device_t) pEventData;
    pFsDev = (RAWFS_DEV *) pUserData;

    /* If this event is not for us, then return */
    if (pFsDev->volDesc.device != device) {
        return;
    }

    /* 
     * Unregister the registered events.  Then inform the vnode layer to
     * unmount the raw file system.
     */

    erfHandlerUnregister (xbdEventCategory, xbdEventRemove,
                          rawfsEject, pFsDev);
    erfHandlerUnregister (xbdEventCategory, xbdeventMediaChanged,
                          rawfsEject, pFsDev);

    mountEject (pFsDev->volDesc.pMount, type == xbdEventMediaChanged);
}

/***************************************************************************
 *
 * rawfsDiskProbe - probes the media for the rawfs file system
 *
 * Unlike other file systems, rawfs does not have any superblocks or other
 * disk structures to test/access.  The entire device is treated as one
 * giant file.
 *
 * RETURNS: OK
 */

int  rawfsDiskProbe (
    device_t  device    /* identify device to probe */
    ) {
    return (OK);        /* rawfs probe always succeeds */
}

/***************************************************************************
 *
 * rawfsLibInit - initialize the rawfs library
 *
 * RETURNS: OK
 */

STATUS rawfsLibInit (
    int  maxBufs,
    int  maxFiles,
    int  reserved2,
    int  reserved1
    ) {
    if (rawfsLibInitialized) {
        return (OK);
    }

    rawfsMaxBuffers = maxBufs;
    rawfsMaxFiles   = maxFiles;

    rawfsLibInitialized = TRUE;
    return (OK);
}

/***************************************************************************
 *
 * rawfsDevCreate - create a raw file system device
 *
 * This routine creates a raw file system device.
 *
 * RETURNS: ptr to the file system device on success, NULL otherwise
 */

RAWFS_DEV *  rawfsDevCreate (
    char *    pDevName,
    device_t  device,
    int       numBufs,
    int       maxFiles,
    int       reserved2,
    int       reserved1
    ) {
    mount_t *    pMount;
    RAWFS_DEV *  pFsDev;
    int          error;

    /*
     * If the library is not initialized or no device name has been supplied,
     * then return NULL and set the errno.
     */   

    if ((!rawfsLibInitialized) || (pDevName == NULL) ||
        (pDevName[0] == '\0')) {
        errnoSet (EINVAL);
        return (NULL);
    }

    if (numBufs < RAWFS_MIN_BUFFERS) {
        numBufs = RAWFS_MIN_BUFFERS;
    }

    error = mountCreate (rawfsVfsOps, device, maxFiles, pDevname, &pMount);
    if (error != OK) {
        errnoSet (error);
        return (NULL);
    }

    error = mountBufAlloc (pMount, numBufs, pFsDev->volDesc.blkSize);
    if (error != OK) {
        mountDelete (pMount, 0);
        errnoSet (error);
        return (NULL);
    }

    return (pFsDev);
}

/***************************************************************************
 *
 * rawfsDevDelete - delete a raw file system device
 *
 * This routine deletes a raw file system device.  It should only be called
 * if the underlying device is no longer in use.  This is typically used upon
 * the handling of an ejection event.
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS rawfsDevDelete (
    RAWFS_DEV *  pFsDev    /* ptr to the raw file system device */
    ) {
    if (INT_RESTRICT () != OK) {
        return (ERROR);
    }

    if (pFsDev == NULL) {
        return (ERROR);
    }

    mountDelete (pFsDev->volDesc.pMount, 0);

    return (OK);
}

/***************************************************************************
 *
 * rawfsDevCreate2 - create a raw file system device
 *
 * This routine creates a raw file system device.  It is meant to be called
 * upon a successful rawfs probe.
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS rawfsDevCreate2 (
    device_t  device,    /* XBD device ID */
    char *    pDevName   /* device name */
    ) {
    RAWFS_DEV *  pFsDev;
    int          fd;

    pFsDev = rawfsDevCreate (pDevName, device, rawfsMaxBuffers,
                             rawfsMaxFiles, 0, 0);
    if (pFsDev == NULL) {
        /* errno set by rawfsDevCreate() */
        return (NULL);
    }

    fd = open (pDevName, O_RDONLY, 0777);
    if (fd < 0) {
        goto errorReturn;

    close (fd);

    if ((erfHandlerRegister (xbdEventCategory, xbdEventRemove,
                             rawfsEject, pFsDev, 0) != 0K) ||
        (erfHandlerRegister (xbdEventCategory, xbdEventMediaChanged,
                             rawfsEject, pFsDev, 0) != OK)) {
        goto errorReturn;
    }

    xbdIoctl (device, XBD_STACK_COMPLETE, NULL);
    fsPathAddedEventRaise (pDevName);
    return (OK);

errorReturn:
    rawfsDevDelete (pFsDev);
    return (ERROR);
}
