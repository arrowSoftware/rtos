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

/* dosFatLibP.h - Dos FAT library header */

#ifndef _dosFatLibP_h
#define _dosFatLibP_h

#include <sys/types.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <oldfs/private/dosFsLibP.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef enum fatRw {
  FAT_READ,
  FAT_WRITE_GROUP,
  FAT_WRITE_CLUST
} FAT_RW;

typedef struct fatInfo {
  DOS_FAT_DESC		dosFatDesc;		/* Fat descriptor */
  FUNCPTR		entryRead;		/* Read FAT entry */
  FUNCPTR		entryWrite;		/* Write entry */

  u_int32_t		fatStartSec;		/* Start sector */
  u_int32_t		nFatEnts;		/* Number of fat entries */
  u_int32_t		fatEntFreeCnt;		/* Free fat entries */
  u_int32_t		dos_fat_avail;		/* Cluster available */
  u_int32_t		dos_fat_reserv;		/* Cluster reserved */
  u_int32_t		dos_fat_bad;		/* Bad cluster */
  u_int32_t		dos_fat_eof;		/* End-of-file cluster */
  u_int32_t		fatGroupSize;		/* Size clusters */
  u_int32_t		groupAllocStart;	/* Dos minumum clusters */
  u_int32_t		clustAllocStart;	/* Dos minimum clusters */

  BOOL			syncEnabled;		/* Fat mirroring */
  SEM_ID		allocSem;		/* Alloc semaphore */
} FAT_INFO;

/* Macros */
#define CLUST_TO_SEC(volId, clustNum)					       \
  (((clustNum) - DOS_MIN_CLUST) * (volId)->secPerClust + (volId)->dataStartSec)

#define SEC_TO_CLUST(volId, secNum)					       \
  (((secNum) - (volId)->dataStartSec) / (volId)->secPerClust + DOS_MIN_CLUST)

/* Functions */
IMPORT STATUS dosFatLibInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _dosFatLibP_h */

