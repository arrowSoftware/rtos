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

/* dosFsLib.h - Dos filesystem header */

#ifndef _dosFsLib_h
#define _dosFsLib_h

#include <rtos.h>
#include <io/blkIo.h>
#include <io/cbioLib.h>

#define DOS_MAX_DIR_LEVELS		20
#define DOS_VOL_LABEL_LEN		11

#define DOS_ATTR_RDONLY			0x01
#define DOS_ATTR_HIDDEN			0x02
#define DOS_ATTR_SYSTEM			0x04
#define DOS_ATTR_VOL_LABEL		0x08
#define DOS_ATTR_DIRECTORY		0x10
#define DOS_ATTR_ARCHIVE		0x20

#define DOS_O_CONTIG_CHK		0x10000
#define DOS_O_CASENS			0x20000

#define DOS_CHK_ONLY			1
#define DOS_CHK_REPAIR			2
#define DOS_CHK_VERB_0			(0xff << 8)
#define DOS_CHK_VERB_1			(1 << 8)
#define DOS_CHK_VERB_2			(2 << 8)
#define DOS_CHK_VERB_SILENT		DOS_CHK_VERB_0

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <oldfs/private/dosFsLibP.h>

typedef struct {

  enum { _AUTO = 0,
	 _FAT12 = 12,
	 _FAT16 = 16,
	 _FAT32 = 32
  } fatType;				/* FAT format type */

  char sysId[10];			/* Sys id string */
  int secPerClust;			/* Sectors per cluster */
  short nReserved;			/* Number of reserved sectors */
  short maxRootEnts;			/* Maximum root entries */
  char nFats;				/* Number of FAT copies */
  unsigned long secPerFat;		/* Number of sectors per FAT copy */
  unsigned nClust;			/* Number of clusters on disk */
  unsigned long nHidden;		/* Number of hidden sectors */
  unsigned long volSerial;		/* Disk serial number */
  int secPerTrack;			/* Sectors per track */
  char nHeads;				/* Number of heads */
  char mediaByte;			/* Media descriptor byte */
  char volLabel[11];			/* Volume label */

} DOS_VOL_CONFIG;

typedef DOS_VOL_DESC *DOS_VOL_DESC_ID;

IMPORT STATUS dosFsLibInit(void);
IMPORT DOS_VOL_DESC_ID dosFsDevInit(char *name,
			            BLK_DEV *pDev,
			            int maxFiles,
			            int autoCheck);
IMPORT DOS_VOL_DESC_ID dosFsVolDescGet(void *idOrName, char **ppTail);
IMPORT STATUS dosFsVolLabelGet(DOS_VOL_DESC_ID volId, char *name);
IMPORT STATUS dosFsVolUnmount(DOS_VOL_DESC_ID volId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _dosFsLib_h */

