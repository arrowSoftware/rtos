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

/* rt11FsLib.h - Rt11 compatible filesystem header */

#ifndef _rt11FsLib_h
#define _rt11FsLib_h

#include <rtos.h>
#include <rtos/semLib.h>
#include <io/blkIo.h>
#include <io/iosLib.h>

#define RT_DEFAULT_MAX_FILES		10
#define RT_DIR_BLOCK			6
#define RT_BYTES_PER_BLOCK		512
#define RT_FILES_FOR_2_BLOCK_SEG	72
#define RT_MAX_BLOCKS_PER_FILE		0xffff

#define DES_BOGUS			0x0000		/* Not real dir entry */
#define DES_TENTATIVE			0x0100		/* Tentative file */
#define DES_EMPTY			0x0200		/* Empty space */
#define DES_PERMANENT			0x0400		/* Permanent file */
#define DES_END				0x0800		/* End of directory */

#define RT_VD_READY_CHANGED		0		/* Ready change */
#define RT_VD_RESET			1		/* Volume reset */
#define RT_VD_MOUNTED			2		/* Volume mounted */
#define RT_VD_CANT_RESET		3		/* Volume reset fail */
#define RT_VD_CANT_MOUNT		4		/* Volume mount fail */

#define NOT_IN_USE			-1		/* Not in use flag */

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned short			nm_name1;	/* Filename 1-3 chars */
  unsigned short			nm_name2;	/* Filename 4-6 chars */
  unsigned short			nm_type;	/* Filetype 1-3 chars */
} RT_NAME;

typedef struct {
  short					de_status;	/* File status */
  RT_NAME				de_name;	/* Filename */
  unsigned short			de_nblocks;	/* Number of blocks */
  char					de_jobnum;	/* Job with file */
  char					de_channel;	/* Channel with file */
  short					de_date;	/* File date */
} RT_DIR_ENTRY;

typedef struct {
  short					ds_nsegs;	/* Number of segments */
  short					ds_next_seg;	/* Next segment */
  short					ds_last_seg;	/* Last segment */
  short					ds_extra;	/* Extra date */
  short					ds_start;	/* Start block */
  RT_DIR_ENTRY				ds_entries[1];	/* Directory entries */
} RT_DIR_SEG;

typedef struct {
  DEV_HEADER				vd_devhdr;	/* Device header */
  short					vd_status;	/* Status */
  SEM_ID				vd_semId;	/* Semaphore */
  CBIO_DEV_ID				vd_dev;		/* Device */
  int 					vd_nblocks;	/* Number of blocks */
  BOOL					vd_rtFmt;	/* Skev track offset */
  int 					vd_secBlock;	/* Sectors per block */
  int 					vd_state;	/* State */
  int					vd_nSegBlocks;	/* Block per dir seg */
  RT_DIR_SEG				*vd_dir_seg;	/* Directory segment */
  BOOL					vd_changeNoWrn; /* No change warn */
} RT_VOL_DESC;

typedef struct {
  short					rfd_status;	/* Status */
  RT_VOL_DESC				*rfd_vdptr;	/* Volume descriptor */
  int					rfd_start;	/* First block file */
  short					rfd_mode;	/* Mode */
  RT_DIR_ENTRY				rfd_dir_entry;	/* Directory entry */
  int					rfd_curptr;	/* Current pos */
  int					rfd_newptr;	/* New pos */
  int					rfd_endptr;	/* End pos */
  BOOL					rfd_modified;	/* Modified flag */
  char					rfd_buffer[RT_BYTES_PER_BLOCK];
} RT_FILE_DESC;

IMPORT STATUS rt11FsLibInit(int maxFiles);
IMPORT RT_VOL_DESC* rt11FsDevInit(char *name,
				  BLK_DEV *pDev,
				  BOOL rt11Fmt,
				  int nEntries,
				  BOOL changeNoWrn);
IMPORT void rt11FsReadyChange(RT_VOL_DESC *pVol);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _rt11FsLib_h */

