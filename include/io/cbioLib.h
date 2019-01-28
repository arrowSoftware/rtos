/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2008 - 2009 Surplus Users Ham Society
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

/* cbioLib.h - Cached block I/O header */

#ifndef _cbioLib_h
#define _cbioLib_h

#include <rtos.h>
#include <util/classLib.h>
#include <io/blkIo.h>
#include <tools/moduleNumber.h>

#define S_cbioLib_NOT_INSTALLED    (M_cbioLib | 0x0001)
#define S_cbioLib_INVALID_DEV_ID   (M_cbioLib | 0x0002)

/* Defines */
#define CBIO_RESET		0xcb100000
#define CBIO_STATUS_CHK		0xcb100001
#define CBIO_DEVICE_LOCK	0xcb100002
#define CBIO_DEVICE_UNLOCK	0xcb100003
#define CBIO_DEVICE_EJECT	0xcb100004
#define CBIO_CACHE_FLUSH	0xcb100010
#define CBIO_CACHE_INVAL	0xcb100030
#define CBIO_CACHE_NEWBLK	0xcb100050

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef unsigned long block_t;
typedef unsigned long cookie_t;
typedef struct cbioDev *CBIO_DEV_ID;

typedef enum {
  CBIO_READ,
  CBIO_WRITE
} CBIO_RW;

typedef struct cbioParams {
  BOOL		cbioRemovable;
  block_t	nBlocks;
  size_t	bytesPerBlk;
  size_t	blockOffset;
  short		blocksPerTrack;
  short		nHeads;
  short		retryCnt;
  block_t	lastErrBlk;
  int		lastErrno;
} CBIO_PARAMS;

extern STATUS cbioLibInit(void);
extern CBIO_DEV_ID cbioDevCreate(char *ramAddr, size_t ramSize);
extern STATUS cbioDevVerify(CBIO_DEV_ID devId);
extern STATUS cbioLock(CBIO_DEV_ID devId, int timeout);
extern STATUS cbioUnlock(CBIO_DEV_ID devId);
extern STATUS cbioBlkRW(CBIO_DEV_ID devId,
			block_t startBlock,
			block_t numBlock,
			void *buffer,
			CBIO_RW rw,
			cookie_t *pCookie);
extern STATUS cbioBytesRW(CBIO_DEV_ID devId,
			  block_t startBlock,
			  off_t offset,
			  void *buffer,
			  size_t nBytes,
			  CBIO_RW rw,
			  cookie_t *pCookie);
extern STATUS cbioBlkCopy(CBIO_DEV_ID devId,
			  block_t src,
			  block_t dest,
			  block_t numBlocks);
extern STATUS cbioIoctl(CBIO_DEV_ID devId,
			int command,
			ARG arg);
extern STATUS cbioModeSet(CBIO_DEV_ID devId, int mode);
extern int cbioModeGet(CBIO_DEV_ID devId);
extern STATUS cbioRdyChgdSet(CBIO_DEV_ID devId, BOOL status);
extern int cbioRdyChgdGet(CBIO_DEV_ID devId);
extern STATUS cbioParamsGet(CBIO_DEV_ID devId, CBIO_PARAMS *pCbioParams);
extern CBIO_DEV_ID cbioWrapBlkDev(BLK_DEV *pBlkDev);

extern CLASS_ID cbioClassId;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _cbioLib_h */

