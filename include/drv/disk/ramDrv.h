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

/* ramDrv.h - Ramdisk driver */

#ifndef _ramDrv_h
#define _ramDrv_h

#include <rtos.h>
#include <io/blkIo.h>

#define RAMDRV_DEFAULT_DISK_SIZE	51200
#define RAMDRV_DEFAULT_SEC_SIZE		512

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  BLK_DEV	blkDev;
  char		*ramAddr;
  int 		blkOffset;
} RAM_DEV;

IMPORT BLK_DEV *ramDevCreate(char *radAddr,
			     int bytesPerBlk,
			     int blksPerTrack,
			     int nBlocks,
			     int blkOffset);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ramDrv_h */

