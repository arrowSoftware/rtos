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

/* dosDirLibP.h - Dos directory header */

#ifndef _dosDirLibP_h
#define _dosDirLibP_h

#include <time.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/private/dosFsLibP.h>

#define DOS_VFAT_NAME_LEN		255	/* Max filename lenght */
#define ROOT_DIRENT			NULL
#define DOS_DEL_MARK			0xe5

#define DOS_DIRENT_STD_LEN		32	/* Default max dir entires */
#define DOS_RESERVED_LEN		10	/* Reserved bytes */
#define DOS_STDNAME_LEN			8	/* Standard filename length */
#define DOS_STDEXT_LEN			3	/* Standard extension lenght */

#define DOS_ATTRIB_OFF			(DOS_STDNAME_LEN + DOS_STDEXT_LEN)
#define DOS_NAME_CASE_OFF		(DOS_ATTRIB_OFF + 1)
#define DOS_CREAT_MS_OFF		(DOS_NAME_CASE_OFF + 1)
#define DOS_CREAT_TIME_OFF		(DOS_CREAT_MS_OFF + 1)
#define DOS_CREAT_DATE_OFF		(DOS_CREAT_TIME_OFF + 2)
#define DOS_LAST_ACCESS_TIME_OFF	NONE
#define DOS_LAST_ACCESS_DATE_OFF	(DOS_CREAT_DATE_OFF + 2)
#define DOS_EXT_START_CLUST_OFF		(DOS_LAST_ACCESS_DATE_OFF + 2)
#define DOS_MODIF_TIME_OFF		(DOS_EXT_START_CLUST_OFF + 2)
#define DOS_MODIF_DATE_OFF		(DOS_MODIF_TIME_OFF + 2)
#define DOS_START_CLUST_OFF		(DOS_MODIF_DATE_OFF + 2)
#define DOS_FILE_SIZE_OFF		(DOS_START_CLUST_OFF + 2)
#define DOS_EXT_FILE_SIZE_OFF		NONE
#define DOS_EXT_FILE_SIZE_LEN		NONE

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct direntDesc {
  unsigned char		dirEntSize;		/* Directory entry size */
  unsigned char		nameLen;		/* Name length */
  unsigned char		extLen;			/* Extension length */
  unsigned char		attribOff;		/* Attribute offset */
  unsigned char		creatTimeOff;		/* Creation time offset */
  unsigned char		creatDateOff;		/* Creation date offset */
  unsigned char		modifTimeOff;		/* Modification time offset */
  unsigned char		modifDateOff;		/* Modification date offset */
  unsigned char		accessTimeOff;		/* Access time offset */
  unsigned char		accessDateOff;		/* Access date offset */
  unsigned char		startClustOff;		/* Start cluster offset */
  unsigned char		extStartClustOff;	/* Extended start clust off */
  unsigned char		sizeOff;		/* Size offset */
  unsigned char		extSizeOff;		/* Extended size offset */
  unsigned char		extSizeLen;		/* Extended size length */
} DIRENT_DESC;

typedef struct dosDirPdesc {
  DOS_DIR_DESC		dirDesc;		/* Directory descriptor */
  unsigned char		*nameBuf;		/* Name buffer */
  SEM_ID		bufSem;			/* Semaphore */

  enum {
    STDDOS = 0,
    FAT = 2
  }			nameStyle;

  DIRENT_DESC		deDesc;			/* Dirent decriptor */

  u_int32_t		rootStartClust;		/* Root dir start cluster */
  unsigned int		rootMaxEntries;		/* Max root entries */
  time_t		rootModifTime;		/* Root dir mod time */
  
} DOS_DIR_PDESC;

typedef struct direntPtr {
  unsigned int		deNum;			/* Direntry number */
  unsigned int		sector;			/* Sector */
  off_t			offset;
} DIRENT_PTR;

typedef struct pathArray {
  unsigned char		*pName;			/* Name */
  unsigned short	nameLen;		/* Name length */
} PATH_ARRAY;

/* Macros */
#define START_CLUST_DECODE(volId, pDeDesc, pDirEnt)			       \
  ( DISK_TO_16BIT((char *) (pDirEnt) + (pDeDesc)->startClustOff) +	       \
		  (((volId)->fatType == FAT32) ?			       \
		    (DISK_TO_16BIT((char *) (pDirEnt) +			       \
			           (pDeDesc)->extStartClustOff) << 16) :       \
		    0) )

#define START_CLUST_ENCODE(pDeDesc, clust, pDirEnt)			       \
{									       \
  TO_DISK_16BIT((clust),						       \
		(char *) (pDirEnt) + (pDeDesc)->startClustOff);		       \
  TO_DISK_16BIT((clust) >> 16,						       \
		(char *) (pDirEnt) + (pDeDesc)->extStartClustOff);	       \
}

#define EXT_SIZE_DECODE(pDeDesc, pDirEnt)		0
#define EXT_SIZE_ENCODE(pDeDesc, pDirEnt, size)				       \
  memset((char *) (pDirEnt) + (pDeDesc)->extSizeOff, 0, (pDeDesc)->extSizeLen);

/* Functions */
IMPORT STATUS dosDirLibInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _dosDirLibP_h */

