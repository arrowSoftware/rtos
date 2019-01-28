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

/* dosFsLibP.h - Private dos filesystem header */

#ifndef _dosFsLibP_h
#define _dosFsLibP_h

#include <dirent.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <io/iosLib.h>
#include <io/cbioLib.h>

#define DOS_NFILES_DEFAULT		20
#define DOS_FS_MAGIC			0xdfac9723
#define DOS_MAX_HDLRS			4
#define DOS_VDIR_HDLR_ID		1
#define DOS_FAT_HDLR_ID			2

/* Filesystem constants */
#define DOS_BOOT_SEC_NUM		0
#define DOS_MIN_CLUST			2
#define DOS_SYS_ID_LEN			8
#define DOS_FAT_12BIT_MAX		4085
#define DOS_NFILES_DEFAULT		20

/* Boot sector data */
#define DOS_BOOT_JMP			0x00
#define DOS_BOOT_SYS_ID			0x03
#define DOS_BOOT_BYTES_PER_SEC		0x0b
#define DOS_BOOT_SEC_PER_CLUST		0x0d
#define DOS_BOOT_NRESRVD_SECS		0x0e
#define DOS_BOOT_NFATS			0x10
#define DOS_BOOT_MAX_ROOT_ENTS		0x11
#define DOS_BOOT_NSECTORS		0x13
#define DOS_BOOT_MEDIA_BYTE		0x15
#define DOS_BOOT_SEC_PER_FAT		0x16
#define DOS_BOOT_SEC_PER_TRACK		0x18
#define DOS_BOOT_NHEADS			0x1a
#define DOS_BOOT_NHIDDEN_SECS		0x1c
#define DOS_BOOT_LONG_NSECTORS		0x20
#define DOS_BOOT_DRIVE_NUM		0x24
#define DOS_BOOT_SIG_REC		0x26
#define DOS_BOOT_VOL_ID			0x27
#define DOS_BOOT_VOL_LABEL		0x2b

#define DOS_BOOT_FSTYPE_ID		0x36
#define DOS_BOOT_FSTYPE_LEN		0x08
#define DOS_BOOT_FSTYPE_FAT12		"FAT12   "
#define DOS_BOOT_FSTYPE_FAT16		"FAT16   "

#define DOS32_BOOT_SEC_PER_FAT		0x24
#define DOS32_BOOT_EXT_FLAGS		0x28
#define DOS32_BOOT_FS_VERSION		0x2a
#define DOS32_BOOT_ROOT_START_CLUST	0x2c
#define DOS32_BOOT_FS_INFO_SEC		0x30
#define DOS32_BOOT_BOOT_BKUO		0x32
#define DOS32_BOOT_RESERVED		0x3a
#define DOS32_BOOT_BIOS_DRV_NUM		0x40
#define DOS32_BOOT_SIGNATURE		0x42
#define DOS32_BOOT_VOL_ID		0x43
#define DOS32_BOOT_VOL_LABEL		0x52

#define DOS_BOOT_BUF_SIZE		0x80

/* Fat handler */
#define FAT_NOT_ALLOC			0
#define FAT_ALLOC			(1 << 31)
#define FAT_ALLOC_ONE			(FAT_ALLOC | 1)
#define FH_INCLUDE			0
#define FH_EXCLUDE			1
#define FH_FILE_START			((unsigned int) (-1))

/* Directory handler */
#define DH_TIME_CREAT			0x01
#define DH_TIME_MODIFY			0x02
#define DH_TIME_ACCESS			0x04
#define DH_DELETE			0x08

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dosFatDesc {
  FUNCPTR		getNext;
  FUNCPTR		contigChk;
  FUNCPTR		truncate;
  FUNCPTR		seek;
  FUNCPTR		nFree;
  FUNCPTR		contigAlloc;
  FUNCPTR		maxContig;
  VOIDFUNCPTR		volUnmount;
  FUNCPTR		show;
  FUNCPTR		flush;
  VOIDFUNCPTR		syncToggle;
  FUNCPTR		clustValueSet;
  FUNCPTR		clustValueGet;
  u_int8_t		activeCopyNum;
} DOS_FAT_DESC;

typedef struct dosDirDesc {
  FUNCPTR		pathLkup;
  FUNCPTR		readDir;
  FUNCPTR		updateEntry;
  FUNCPTR		dateGet;
  FUNCPTR		volLabel;
  FUNCPTR		nameChk;
  VOIDFUNCPTR		volUnmount;
  FUNCPTR		show;
  unsigned int		rootStartSec;
  unsigned int		rootNSec;
} DOS_DIR_DESC;

typedef struct dosFatHdl {
  u_int32_t		lastClust;		/* Number of last cluster */
  u_int32_t		nextClust;		/* Contents */
  cookie_t		cbioCookie;		/* Cookie */
  u_int32_t		errCode;		/* Error code */
} DOS_FAT_HDL;

typedef struct dosFileDesc {
  struct dosVolDesc	*pVolDesc;		/* Volume descriptor */
  struct dosFileHdl	*pFileHdl;		/* File handle */
  fsize_t		pos;			/* Current position */
  fsize_t		seekOutPos;		/* Save pos if seek pass EOF */
  block_t		curSec;			/* Current sector */
  unsigned		int nSec;		/* Number of sectors */
  cookie_t		cbioCookie;		/* Cookie */
  unsigned char		accessed;		/* File accessed */
  unsigned char		changed;		/* File changed */
  struct dosFatHdl	fatHdl;			/* Fat handle */
  unsigned char		openMode;		/* File open mode */
  BOOL			busy;			/* Busy flag */
} DOS_FILE_DESC;

typedef struct dosDirHdl {
  unsigned int		parDirStartCluster;	/* Parent dir cluster */
  block_t		sector;			/* Sector containing alias */
  off_t			offset;			/* Alias dir entry offset */

  block_t		lnSector;		/* Sector longname start */
  off_t			lnOffset;		/* Long name offset in sector */
  cookie_t		cookie;			/* Cookie */

  unsigned long		db_Priv1;
  unsigned long		db_Priv2;
  unsigned long		db_Priv3;
  unsigned long		db_Priv4;

  unsigned char		crc;			/* Crc on directry entry */
} DOS_DIR_HDL;

typedef struct dosFileHdl {
  fsize_t		size;			/* File size */
  unsigned int		deleted:1,		/* File deleted */
	       		obsolete:1,		/* File obsolete */
	       		changed:1,		/* File changed */
	       		res:29;

  /* FAT related */
  u_int32_t		startClust;		/* File start cluster */
  u_int32_t		contigEndPlus1;		/* End contig area + 1 */
  u_int32_t		fatSector;		/* Last modified FAT sector */
  struct dosDirHdl	dirHdl;			/* Directory entry descriptor */
  unsigned short	nRef;			/* Number of references */
  unsigned char		attrib;			/* File attributes */
} DOS_FILE_HDL;

typedef struct dosVolDesc {
  DEV_HEADER		devHeader;		/* Device header */

  unsigned int 		magic;			/* Magic number */
  BOOL 			mounted;		/* Mounted */
  BOOL			updateLastAccessDate;	/* Update last access date */
  BOOL			volIsCaseSens;		/* Case sensitive */

  CBIO_DEV_ID		pCbio;			/* Block I/O device */

  SEM_ID		devSem;			/* Device semaphore */
  SEM_ID		shortSem;		/* For short operations */

  int			maxFiles;		/* Maximum num of open files */
  unsigned short	nBusyFd;		/* Number of used filedesc */
  struct dosFileDesc	*pFdList;		/* File descriptor list */
  struct dosFileHdl	*pFhdlList;		/* File handle list */
  SEM_ID		*pFsemList;		/* File semaphores list */

  struct dosDirDesc	*pDirDesc;		/* Directory descriptor */
  struct dosFatDesc	*pFatDesc;		/* FAT descriptor */

  enum { FAT12, FAT16, FAT32 } fatType;		/* Fat type */

  unsigned int		dataStartSec;		/* Data start sector */
  unsigned short	volIdOff;		/* Offset of vol id field */
  unsigned short	volLabOff;		/* Offset of vol label */
  unsigned char		secSizeShift;		/* Bytes per sec (log 2) */

  /* Boot sector */
  unsigned int		bootSecNum;		/* Boot sector number */
  u_int32_t		volId;			/* Volume id */
  u_int32_t		secPerFat;		/* Sector per fat */
  u_int32_t		nHiddenSecs;		/* Hidden sectors */
  u_int32_t		totalSec;		/* Total number of sectors */
  u_int16_t		bytesPerSec;		/* Bytes per sector */
  u_int16_t		secPerClust;		/* Sectors per cluster */
  u_int16_t		nReservedSecs;		/* Reserved sectors */
  u_int8_t		nFats;			/* Number of fat copies */
  u_int32_t		nFatEnts;		/* Number of fat entries */
  char			bootVolLab[DOS_VOL_LABEL_LEN];

  /* Check disk */
  unsigned char		autoChk;		/* Auto check disk on mount */
  unsigned char		autoChkVerb;		/* Auto check verbose */
  unsigned char		chkLevel;		/* Check progress level */
} DOS_VOL_DESC;

typedef struct dosHdlrDesc {
  unsigned int		id;			/* Id */
  FUNCPTR		mountFunc;		/* Mount function */
  ARG			arg;			/* Argument */
} DOS_HDLR_DESC;

/* Macros */
#define DOS_MAX_FSIZE	( ~((fsize_t) 1 << (sizeof(fsize_t) * 8 - 1)) )

#define DISK_TO_16BIT(src)						       \
  (u_int16_t) (  ( *((u_int8_t *) (src) + 1) << 8) |			       \
		 ( *(u_int8_t *) (src) )  )

#define DISK_TO_32BIT(src)						       \
  (u_int32_t) (  ( *((u_int8_t *) (src) + 3) << 24) |			       \
		 ( *((u_int8_t *) (src) + 2) << 16) |			       \
		 ( *((u_int8_t *) (src) + 1) << 8) |			       \
		 ( *(u_int8_t *) (src) )  )

#define TO_DISK_16BIT(src, dest)					       \
  ( *(u_int8_t *) (dest) = (src) & 0xff,				       \
    *((u_int8_t *) (dest) + 1) = ((src) >> 8) & 0xff )

#define TO_DISK_32BIT(src, dest)					       \
  ( *(u_int8_t *) (dest) = (src) & 0xff,				       \
    *((u_int8_t *) (dest) + 1) = ((src) >> 8) & 0xff,			       \
    *((u_int8_t *) (dest) + 2) = ((src) >> 16) & 0xff,			       \
    *((u_int8_t *) (dest) + 3) = ((src) >> 24) & 0xff )

#define NSECTORS(volId, off)						       \
  ( (off) >> (volId)->secSizeShift )

#define OFFSET_IN_SEC(volId, off)					       \
  ( (off) & ((volId)->bytesPerSec - 1) )

#define IS_ROOT(pFd)							       \
  ( (pFd)->pFileHdl->dirHdl.parDirStartCluster == (unsigned int) NONE )

#define ROOT_SET(pFd)							       \
  ( (pFd)->pFileHdl->dirHdl.parDirStartCluster = (unsigned int) NONE )

#define POS_TO_INO(pos)		( ((ino_t) (pos)) | 1)
#define DIRENT_TO_POS(pDir)	( ((struct dirent *) (pDir))->d_ino & (~1) )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _dosFsLibP_h */

