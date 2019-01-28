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

/* cbioLibP.h - Cached block I/O interface private header */

#ifndef _cbioLibP_h
#define _cbioLibP_h

#include <rtos.h>
#include <util/classLib.h>
#include <rtos/semLib.h>
#include <io/blkIo.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cbioFuncs {
  FUNCPTR cbioDevBlkRW;
  FUNCPTR cbioDevBytesRW;
  FUNCPTR cbioDevBlkCopy;
  FUNCPTR cbioDevIoctl;
} CBIO_FUNCS;

typedef struct cbioDev {
  OBJ_CORE objCore;
  SEM_ID cbioMutex;
  CBIO_FUNCS *pFuncs;

  /* Public */
  char cbioDesc[24];
  short cbioMode;
  BOOL readyChanged;

  CBIO_PARAMS cbioParams;

  char *cbioMemBase;
  size_t cbioMemSize;

  /* Extra data */
  unsigned long cbioPriv0;
  unsigned long cbioPriv1;
  unsigned long cbioPriv2;
  unsigned long cbioPriv3;
  unsigned long cbioPriv4;
  unsigned long cbioPriv5;
  unsigned long cbioPriv6;
  unsigned long cbioPriv7;

  void *pDc;
  CBIO_DEV_ID cbioSubDev;
  BLK_DEV *blkSubDev;

  BOOL isDriver;
} CBIO_DEV;

/* Macros */
#define CBIO_MODE(devId)		( (devId)->cbioMode )
#define CBIO_READYCHANGED(devId)	( (devId)->readyChanged )
#define CBIO_REMOVABLE(devId)		( (devId)->cbioParams.cbioRemovable )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _cbioLibP_h */

