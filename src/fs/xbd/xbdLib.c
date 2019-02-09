/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2009 - 2010 Surplus Users Ham Society
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

/* xbdLib.c - Extended Block Device Library */

/*
DESCRIPTION
This library implements the extended block device module; other modules will
implement specific extended block devices.  This module is dependent upon the
event reporting framework (ERF), and so if used, it must be initialized AFTER
the ERF.

At present, it supports two ERF types--xbdEventRemove and xbdEventMediaChanged.
Each one is to be used when appropriate.  For example, some devices (such as
floppy drives or CD-ROM drives) allow have removable media.  Should the media
be removed or replaced from such a device, it should use xbdEventMediaChanged.
Other devices (such as RAM disks) when removed (or deleted) should use the
xbdEventRemove ERF type.
*/

/* includes */

#include <rtos.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arch/intArchLib.h>
#include <fs/xbd.h>
#include <fs/erfLib.h>
#include <rtos/errnoLib.h>
/* defines */

/* typedefs */

/* globals */

int xbdEventCategory        = -1;
int xbdEventRemove          = -1;
int xbdEventMediaChanged    = -1;
int xbdEventPrimaryInsert   = -1;
int xbdEventSecondaryInsert = -1;
int xbdEventSoftInsert      = -1;

/* locals */

LOCAL XBD ** xbdDeviceList = NULL;    /* list index is device_t value */
LOCAL int    xbdMaxDevices = 0;

/***************************************************************************
 *
 * xbdLibInit - initialize the XBD library
 *
 * This routine initializes the XBD library.  If used, it must be called
 * after erfLibInit().
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS xbdLibInit (int  maxXbds)
{
    if (xbdDeviceList != NULL) {
        return (OK);
    }

    if (maxXbds <= 0) {
        errnoSet (S_xbdLib_INVALID_PARAM);
        return (ERROR);
    }

    xbdDeviceList = (XBD **) malloc (maxXbds * sizeof (XBD *));
    if (xbdDeviceList == NULL) {
        /* errno set by malloc() */
        return (ERROR);
    }

    xbdMaxDevices = maxXbds;
    memset (xbdDeviceList, 0, maxXbds * sizeof (XBD *));

    if ((erfCategoryAllocate (&xbdEventCategory) != OK) ||
        (erfTypeAllocate (xbdEventCategory, &xbdEventRemove) != OK) ||
        (erfTypeAllocate (xbdEventCategory, &xbdEventMediaChanged) != OK) ||
        (erfTypeAllocate (xbdEventCategory, &xbdEventPrimaryInsert) != OK) ||
        (erfTypeAllocate (xbdEventCategory, &xbdEventSecondaryInsert) != OK) ||
        (erfTypeAllocate (xbdEventCategory, &xbdEventSoftInsert) != OK)) {
        /* errno set by ERF routines */
        free (xbdDeviceList);
        xbdDeviceList = NULL;
        return (ERROR);
    }

    return (OK);
}

/***************************************************************************
 *
 * xbdAttach - attach an XBD
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS xbdAttach (
    XBD *            xbd,        /* ptr to XBD */
    XBD_FUNCS *      xbdFuncs,   /* ptr to XBD functions */
    const devname_t  xbdName,    /* name to give XBD device */
    unsigned         blkSize,    /* block size */
    lblkno_t         nBlks,      /* number of blocks on device */
    device_t *       pDevice     /* resulting device_t ID */
    ) {
    int  i;
    int  level;

    if (strlen (xbdName) + 1 >= XBD_MAX_NAMELEN) {
        errnoSet (S_xbdLib_NAME_TOO_LONG);
        return (ERROR);
    }

    xbd->blkSize = blkSize;
    xbd->nBlks = nBlks;
    xbd->xbdFuncs.xbdIoctl    = xbdFuncs->xbdIoctl;
    xbd->xbdFuncs.xbdStrategy = xbdFuncs->xbdStrategy;
    xbd->xbdFuncs.xbdDump     = xbdFuncs->xbdDump;
    strcpy (xbd->name, xbdName);

    INT_LOCK (level);
    for (i = 0; i < xbdMaxDevices; i++) {
        if (xbdDeviceList[i] == NULL) {
            xbdDeviceList[i] = xbd;
            break;
        }
    }
    INT_UNLOCK (level);

    if (i == xbdMaxDevices) {
        errnoSet (S_xbdLib_DEVICE_TABLE_FULL);
        return  (ERROR);
    }

    return (OK);
}

/***************************************************************************
 *
 * xbdNBlocks - get the number of blocks in the device
 *
 * RETURNS: OK on success, ENODEV on error
 */

int xbdNBlocks (
    device_t    device,
    lblkno_t *  pNumBlks
    ) {
    XBD *  xbd;
    int    level;

    INT_LOCK (level);

    if ((device >= xbdMaxDevices) || ((xbd = xbdDeviceList[device]) == NULL)) {
        INT_UNLOCK (level);
        return (ENODEV);
    }

    *pNumBlks = xbd->nBlks;

    INT_UNLOCK (level);

    return (OK);
}

/***************************************************************************
 *
 * xbdBlockSize - get the device block size
 *
 * RETURNS: OK on success, ENODEV on error
 */

int xbdBlockSize (
    device_t    device,
    unsigned *  pBlkSize
    ) {
    XBD *  xbd;
    int    level;

    INT_LOCK (level);

    if ((device >= xbdMaxDevices) || ((xbd = xbdDeviceList[device]) == NULL)) {
        INT_UNLOCK (level);
        return (ENODEV);
    }

    *pBlkSize = xbd->blkSize;

    INT_UNLOCK (level);

    return (OK);
}

/***************************************************************************
 *
 * xbdStrategy -
 *
 * RETURNS: OK on success, error otherwise
 */

int xbdStrategy (
    device_t      device,
    struct bio *  pBio
    ) {
    XBD *  xbd;
    int    (*strategy)(XBD *, struct bio *);
    int    level;

    INT_LOCK (level);

    if ((device >= xbdMaxDevices) || ((xbd = xbdDeviceList[device]) == NULL)) {
        INT_UNLOCK (level);
        return (ENODEV);
    }

    strategy = xbd->xbdFuncs.xbdStrategy;

    INT_UNLOCK (level);

    return (strategy (xbd, pBio));
}

/***************************************************************************
 *
 * xbdIoctl -
 *
 * RETURNS: OK on success, error otherwise
 */

int xbdIoctl (
    device_t  device,
    unsigned  command,
    void *    data
    ) {
    XBD *  xbd;
    int    (*func)(XBD *, unsigned, void *);
    int    level;

    INT_LOCK (level);

    if ((device >= xbdMaxDevices) || ((xbd = xbdDeviceList[device]) == NULL)) {
        INT_UNLOCK (level);
        return (ENODEV);
    }

    func = xbd->xbdFuncs.xbdIoctl;

    INT_UNLOCK (level);

    return (func (xbd, command, data));
}

/***************************************************************************
 *
 * xbdDump -
 *
 * RETURNS: OK on success, error otherwise
 */

int xbdDump (
    device_t  device,
    lblkno_t  lblkno,
    void *    data,
    size_t    nBytes
    ) {
    XBD *  xbd;
    int    (*func)(XBD *, lblkno_t, void *, size_t);
    int    level;

    INT_LOCK (level);
    if ((device >= xbdMaxDevices) || ((xbd = xbdDeviceList[device]) == NULL)) {
        INT_UNLOCK (level);
        return (ENODEV);
    }

    func = xbd->xbdFuncs.xbdDump;

    INT_UNLOCK (level);

    return (func (xbd, lblkno, data, nBytes));
}

/***************************************************************************
 *
 * xbdDevName -
 *
 * RETURNS: OK on success, error otherwise
 */

int  xbdDevName (
    device_t   device,
    devname_t  devname
    ) {
    XBD *   xbd;
    int     level;

    INT_LOCK (level);
    if ((device >= xbdMaxDevices) || ((xbd = xbdDeviceList[device]) == NULL)) {
        INT_UNLOCK (level);
        return (ENODEV);
    }
    INT_UNLOCK (level);

    strcpy (devname, xbd->name);
    return (OK);
}
