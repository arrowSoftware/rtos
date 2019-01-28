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


/* xbd.h - extended block device header */

#ifndef __XBD_H
#define __XBD_H

/* includes */

#include <rtos.h>
#include <tools/moduleNumber.h>
#include <fs/bio.h>

/* defines */

#define S_xbdLib_NAME_TOO_LONG      (M_xbdLib | 0x0001)
#define S_xbdLib_DEVICE_TABLE_FULL  (M_xbdLib | 0x0002)
#define S_xbdLib_INVALID_PARAM      (M_xbdLib | 0x0003)

#define XBD_GEOMETRY_GET  0xebd00001

#define XBD_MAX_NAMELEN   32

#define NULLDEV  (-1)

/* typedefs */

typedef int   device_t;        /* ??? at least for now ??? */

typedef struct xbd_geometry {
    unsigned  sectorsPerTrack;
    unsigned  numHeads;
    unsigned  numCylinders;
} XBD_GEOMETRY;

typedef char  devname_t[XBD_MAX_NAMELEN];

struct xbd;

typedef struct xbd_funcs {
    int  (*xbdIoctl) (struct xbd *pXbd, unsigned command, void *data);
    int  (*xbdStrategy) (struct xbd *pXbd, struct bio *bio);
    int  (*xbdDump) (struct xbd *pXbd, lblkno_t blk, void *data, size_t nBytes);
} XBD_FUNCS;

/* When creating a new XBD device structure, XBD shall be its first field. */
typedef struct xbd {
    unsigned      blkSize;
    lblkno_t      nBlks;
    XBD_FUNCS     xbdFuncs;
    devname_t     name;
} XBD;

/* externs */

extern int  xbdEventCategory;
extern int  xbdEventRemove;
extern int  xbdEventMediaChanged;
extern int  xbdEventPrimaryInsert;
extern int  xbdEventSecondaryInsert;
extern int  xbdEventSoftInsert;

extern STATUS xbdLibInit (int  maxXbds);
extern STATUS xbdAttach  (XBD *xbd, XBD_FUNCS *xbdFuncs,
                          const devname_t xbdName, unsigned blkSize,
                          lblkno_t nBlks, device_t *pDevice);

extern int  xbdNBlocks   (device_t device, lblkno_t *pBlkCount);
extern int  xbdBlockSize (device_t device, unsigned *pBlkSize);
extern int  xbdStrategy  (device_t device, struct bio *pBio);
extern int  xbdIoctl     (device_t device, unsigned command, void *data);
extern int  xbdDump      (device_t device, lblkno_t lblkno,
                          void *data, size_t nBytes);
extern int  xbdDevName   (device_t device, devname_t devname);

#endif
