/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2010 Surplus Users Ham Society
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

/* rawFsLib.h - Raw filesystem header */

#ifndef _rawFsLib_h
#define _rawFsLib_h

#include <sys/types.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <io/iosLib.h>
#include <io/ioLib.h>
#include <io/blkIo.h>
#include <io/cbioLib.h>

#define RAWFS_DEFAULT_MAX_FILES		10

#define RAW_VD_READY_CHANGED		0
#define RAW_VD_RESET			1
#define RAW_VD_MOUNTED			2
#define RAW_VD_CANT_RESET		3
#define RAW_VD_CANT_MOUNT		4

#define RAWFD_AVAIL			-1
#define RAWFD_USED			0
#define RAWFD_OBSOLETE			1

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  DEV_HEADER rawVdDeHeader;
  int rawVdStatus;
  SEM_ID rawVdSemId;
  CBIO_DEV_ID rawVdCbio;
  int rawVdState;
  int rawVdRetry;
} RAW_VOL_DESC;

typedef struct {
  LIST_NODE rawFdNode;
  SEM_ID rawFdSemId;
  int rawFdStatus;
  RAW_VOL_DESC *pRawFdVd;
  int rawFdMode;
  fsize_t rawFdNewPtr;
  fsize_t rawFdEndPtr;
  cookie_t rawFdCookie;
} RAW_FILE_DESC;

IMPORT STATUS rawFsLibInit(int maxFiles);
IMPORT RAW_VOL_DESC* rawFsDevInit(char *name, BLK_DEV *pDev);
IMPORT STATUS rawFsVolMount(RAW_VOL_DESC *pVol);
IMPORT STATUS rawFsVolVerify(RAW_VOL_DESC *pVol);
IMPORT void rawFsReadyChange(RAW_VOL_DESC *pVol);
IMPORT STATUS rawFsVolUmount(RAW_VOL_DESC *pVol);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _rawFsLib_h */

