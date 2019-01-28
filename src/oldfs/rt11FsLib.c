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

/* rt11FsLib.c - Rt11 compatible file system */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <io/blkIo.h>
#include <io/iosLib.h>
#include <io/ioLib.h>
#include <io/cbioLib.h>
#include <oldfs/rt11FsLib.h>

/* Defines */
#define RT_NAME_LEN				11

/* Imports */

/* Locals */
LOCAL BOOL rt11FsLibInstalled = FALSE;
LOCAL int rt11FsDrvNum = 0;
LOCAL SEM_ID rt11FsFdSemId;
LOCAL RT_FILE_DESC *rt11FsFd;

LOCAL char rad50[] = " abcdefghijklmnopqrstuvwxyz$.\3770123456789";

LOCAL STATUS rt11FsVolVerify(RT_VOL_DESC *pVol, BOOL doMount);
LOCAL STATUS rt11FsVolReset(RT_VOL_DESC *pVol);
LOCAL STATUS rt11FsVolMode(RT_VOL_DESC *pVol);
LOCAL STATUS rt11FsVolFlush(RT_VOL_DESC *pVol);
LOCAL STATUS rt11FsVolInit(RT_VOL_DESC *pVol);
LOCAL STATUS rt11FsVolMount(RT_VOL_DESC *pVol);
LOCAL STATUS rt11FsVolBlkRW(RT_VOL_DESC *pVol,
			    int blkNum,
			    int numBlocks,
			    char *buffer,
			    CBIO_RW rw);

LOCAL RT_FILE_DESC* rt11FsFdGet(void);
LOCAL STATUS rt11FsFdFlush(RT_FILE_DESC *pFd);
LOCAL void rt11FsFdFree(RT_FILE_DESC *pFd);

LOCAL void rt11FsGetEntry(RT_VOL_DESC *pVol,
			  int entryNum,
			  RT_DIR_ENTRY *pEntry);
LOCAL void rt11FsPutEntry(RT_VOL_DESC *pVol,
			  int entryNum,
			  RT_DIR_ENTRY *pEntry);
LOCAL void rt11FsInsertEntry(RT_VOL_DESC *pVol,
			     int entryNum,
			     RT_DIR_ENTRY *pEntry);
LOCAL int rt11FsFindEntry(RT_VOL_DESC *pVol,
			  char *name,
			  int *pStart);

LOCAL void rt11FsDirMergeEmpty(RT_VOL_DESC *pVol, int entryNum);
LOCAL STATUS rt11FsDirRead(RT_FILE_DESC *pFd, struct dirent *pDir);
LOCAL int rt11FsWhere(RT_FILE_DESC *pFd);
LOCAL STATUS rt11FsSeek(RT_FILE_DESC *pFd, int position);
LOCAL STATUS rt11FsStatGet(RT_FILE_DESC *pFd, struct stat *pStat);
LOCAL STATUS rt11FsRename(RT_FILE_DESC *pFd, char *name);

LOCAL RT_FILE_DESC* rt11FsCreate(RT_VOL_DESC *pVol,
			         char *name,
			         int mode);
LOCAL STATUS rt11FsDelete(RT_VOL_DESC *pVol, char *name);
LOCAL RT_FILE_DESC* rt11FsOpen(RT_VOL_DESC *pVol,
			       char *name,
			       int mode);
LOCAL STATUS rt11FsClose(RT_FILE_DESC *pFd);
LOCAL int rt11FsRead(RT_FILE_DESC *pFd, char *buffer, int maxBytes);
LOCAL int rt11FsWrite(RT_FILE_DESC *pFd, char *buffer, int maxBytes);
LOCAL STATUS rt11FsIoctl(RT_FILE_DESC *pFd,
			 int cmd,
			 ARG arg);

LOCAL int rt11FsR50out(char *string);
LOCAL void rt11FsR50in(unsigned int r50, char *string);
LOCAL void rt11FsNameR50(char *string, RT_NAME *pName);
LOCAL void rt11FsNameString(RT_NAME name, char *string);
LOCAL int rt11FsDate(BOOL now);
LOCAL void rt11FsFileDate(RT_FILE_DESC *pFd, struct tm *pDate);
LOCAL int rt11FsNewBlk(RT_FILE_DESC *pFd);
LOCAL int rt11FsAbsSector(unsigned long secPerTrack, int sector);
LOCAL void rt11FsSwapBytes(char *pSrc, char *pDest, int nBytes);

/* Globals */
int rt11FsMaxFiles = 0;
int rt11FsFdMutexOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);
int rt11FsVolMutexOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);

/* Functions */

/*******************************************************************************
 * rt11FsLibInit - Initialize library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS rt11FsLibInit(int maxFiles)
{
  int i;

  /* Check if installed */
  if (rt11FsLibInstalled)
    return OK;

  /* Initialize semaphore */
  rt11FsFdSemId = semMCreate(rt11FsFdMutexOptions);
  if (rt11FsFdSemId == NULL)
    return ERROR;

  /* Check max files */
  if (maxFiles <= 0)
    maxFiles = RT_DEFAULT_MAX_FILES;

  /* Mark as installed */
  rt11FsLibInstalled = TRUE;

  /* Allocate file storage */
  rt11FsFd = (RT_FILE_DESC *) malloc( maxFiles * sizeof(RT_FILE_DESC));
  if (rt11FsFd == NULL)
    return ERROR;

  for (i = 0; i < maxFiles; i++)
    rt11FsFdFree(&rt11FsFd[i]);

  rt11FsMaxFiles = maxFiles;

  /* Initialize driver */
  rt11FsDrvNum = iosDrvInstall((FUNCPTR) rt11FsCreate,	/* Create */
			       (FUNCPTR) rt11FsDelete,	/* Delete */
			       (FUNCPTR) rt11FsOpen,	/* Open */
			       (FUNCPTR) rt11FsClose,	/* Close */
			       (FUNCPTR) rt11FsRead,	/* Read */
			       (FUNCPTR) rt11FsWrite,	/* Write */
			       (FUNCPTR) rt11FsIoctl);	/* Ioctl */
  if (rt11FsDrvNum == ERROR) {

    free(rt11FsFd);
    return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * rt11FsDevInit - Initialize device
 *
 * RETURNS: Pointer to volume descriptor
 ******************************************************************************/

RT_VOL_DESC* rt11FsDevInit(char *name,
			   BLK_DEV *pDev,
			   BOOL rt11Fmt,
			   int nEntries,
			   BOOL changeNoWrn)
{
  CBIO_DEV_ID devId;
  RT_VOL_DESC *pVol;
  CBIO_PARAMS params;
  int i, seglen;

  /* If device needs to be initialized */
  if (cbioDevVerify((CBIO_DEV_ID) pDev) != OK)
    devId = cbioWrapBlkDev(pDev);
  else
    devId = (CBIO_DEV_ID) pDev;

  /* Get parameters */
  if (cbioParamsGet(devId, &params) != OK)
    return NULL;

  if ( rt11Fmt && (params.blocksPerTrack & 1))
    return NULL;

  /* Allocate volume descriptor */
  pVol = (RT_VOL_DESC *) malloc( sizeof(RT_VOL_DESC) );
  if (pVol == NULL)
    return NULL;

  /* Initialize semaphore */
  pVol->vd_semId = semMCreate(rt11FsVolMutexOptions);
  if (pVol->vd_semId == NULL) {

    free(pVol);
    return NULL;

  }

  /* Setup struct */
  pVol->vd_dev = devId;
  pVol->vd_rtFmt = rt11Fmt;
  pVol->vd_status = OK;
  pVol->vd_state = RT_VD_READY_CHANGED;
  pVol->vd_secBlock = RT_BYTES_PER_BLOCK / params.bytesPerBlk;
  pVol->vd_nblocks = params.nBlocks / pVol->vd_secBlock;

  if (params.cbioRemovable)
    pVol->vd_changeNoWrn = changeNoWrn;
  else
    pVol->vd_changeNoWrn = FALSE;

  seglen = sizeof(RT_DIR_SEG) + (nEntries - 1) * sizeof(RT_DIR_ENTRY);

  if ( (i = 1 + (seglen / RT_BYTES_PER_BLOCK)) < 2)
    pVol->vd_nSegBlocks = 2;
  else
    pVol->vd_nSegBlocks = i;

  /* Allocate memory for directory segment */
  pVol->vd_dir_seg = (RT_DIR_SEG *) malloc(pVol->vd_nSegBlocks *
					   RT_BYTES_PER_BLOCK);
  if (pVol->vd_dir_seg == NULL) {

    semDelete(pVol->vd_semId);
    free(pVol);
    return NULL;

  }

  /* Add device */
  if (iosDevAdd((DEV_HEADER *) pVol, name, rt11FsDrvNum) != OK) {

    free(pVol->vd_dir_seg);
    semDelete(pVol->vd_semId);
    free(pVol);
    return NULL;

  }

  return pVol;
}

/*******************************************************************************
 * rt11FsReadyChange - Change ready state
 *
 * RETURNS: N/A
 ******************************************************************************/

void rt11FsReadyChange(RT_VOL_DESC *pVol)
{
  pVol->vd_state = RT_VD_READY_CHANGED;
}

/*******************************************************************************
 * rt11FsVolVerify - Verify volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsVolVerify(RT_VOL_DESC *pVol, BOOL doMount)
{
  /* Check ready change */
  if ( (cbioIoctl(pVol->vd_dev, CBIO_STATUS_CHK, NULL) != OK) ||
       (cbioRdyChgdGet(pVol->vd_dev) == TRUE) )
    pVol->vd_state = RT_VD_READY_CHANGED;

  /* Select state */
  switch(pVol->vd_state) {

    case RT_VD_CANT_RESET:
    return ERROR;

    case RT_VD_CANT_MOUNT:
      if (doMount)
        return ERROR;
    break;

    case RT_VD_READY_CHANGED:
      if (rt11FsVolReset(pVol) != OK)
        return ERROR;

      pVol->vd_state = RT_VD_RESET;

    /* FALL TRU */

    case RT_VD_RESET:
      if (doMount) {

        if (rt11FsVolMount(pVol) != OK) {

          pVol->vd_state = RT_VD_CANT_MOUNT;
          return ERROR;

        }

        pVol->vd_state = RT_VD_MOUNTED;
      }
    break;

    case RT_VD_MOUNTED:
    break;

  }

  return OK;
}

/*******************************************************************************
 * rt11FsVolReset - Reset volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsVolReset(RT_VOL_DESC *pVol)
{
  if (cbioIoctl(pVol->vd_dev, CBIO_RESET, NULL) != OK) {

    pVol->vd_state = RT_VD_CANT_RESET;
    return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * rt11FsVolMode - Get current mode
 *
 * RETURNS: Mode
 ******************************************************************************/

LOCAL STATUS rt11FsVolMode(RT_VOL_DESC *pVol)
{
  return cbioModeGet(pVol->vd_dev);
}

/*******************************************************************************
 * rt11FsVolFlush - Flush volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsVolFlush(RT_VOL_DESC *pVol)
{
  int nSegBytes;
  char *pSegTop;

  /* Initialize locals */
  nSegBytes = pVol->vd_nSegBlocks * RT_BYTES_PER_BLOCK;
  pSegTop = (char *) pVol->vd_dir_seg;

  /* Reset status */
  pVol->vd_status = OK;

  /* Swap bytes */
  rt11FsSwapBytes(pSegTop, pSegTop, nSegBytes);

  /* Write to disk */
  if (rt11FsVolBlkRW(pVol, RT_DIR_BLOCK, pVol->vd_nSegBlocks,
		     pSegTop, CBIO_WRITE) != OK)
    pVol->vd_status = ERROR;

  /* Swap bytes */
  rt11FsSwapBytes(pSegTop, pSegTop, nSegBytes);

  return pVol->vd_status;
}

/*******************************************************************************
 * rt11FsVolInit - Initialize/format volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsVolInit(RT_VOL_DESC *pVol)
{
  RT_DIR_SEG *pSeg;
  int i, freeBlks;
  STATUS status;

  /* Initialize locals */
  pSeg = pVol->vd_dir_seg;
  i = 0;

  semTake(pVol->vd_semId, WAIT_FOREVER);

  /* Initialize directory segment */
  pSeg->ds_nsegs = 1;
  pSeg->ds_next_seg = 0;
  pSeg->ds_last_seg = 1;
  pSeg->ds_extra = 0;

  /* Fist data block */
  pSeg->ds_start = RT_DIR_BLOCK + pVol->vd_nSegBlocks;

  /* Free blocks */
  freeBlks = pVol->vd_nblocks - pSeg->ds_start;

  /* All files of size RT_MAX_BLOCKS_PER_FILE */
  while (freeBlks > RT_MAX_BLOCKS_PER_FILE) {

    pSeg->ds_entries[i].de_status = DES_EMPTY;
    pSeg->ds_entries[i++].de_nblocks = RT_MAX_BLOCKS_PER_FILE;
    freeBlks -= RT_MAX_BLOCKS_PER_FILE;

  }

  /* Last directory empty */
  if (freeBlks > 0) {

    pSeg->ds_entries[i].de_status = DES_EMPTY;
    pSeg->ds_entries[i++].de_nblocks = freeBlks;

  }

  /* Terminating entry */
  pSeg->ds_entries[i].de_status = DES_END;

  /* Flush volume */
  status = rt11FsVolFlush(pVol);

  if (status == OK)
    rt11FsReadyChange(pVol);
  else
    pVol->vd_status = ERROR;

  semGive(pVol->vd_semId);

  return status;
}

/*******************************************************************************
 * rt11FsVolMount - Mount volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsVolMount(RT_VOL_DESC *pVol)
{
  char *pSeg;

  /* Initialize pointer to directory segment */
  pSeg = (char *) pVol->vd_dir_seg;

  if (rt11FsVolBlkRW(pVol, RT_DIR_BLOCK, pVol->vd_nSegBlocks,
		     pSeg, CBIO_READ) != OK) {

    pVol->vd_status = ERROR;
    return pVol->vd_status;

  }

  /* Initialize pointer to directory segment */
  pSeg = (char *) pVol->vd_dir_seg;

  /* Swap bytes */
  rt11FsSwapBytes(pSeg, pSeg, pVol->vd_nSegBlocks * RT_BYTES_PER_BLOCK);

  /* Check directory segment */
  if ( (pVol->vd_dir_seg->ds_nsegs == 1) &&
       (pVol->vd_dir_seg->ds_next_seg == 0) &&
       (pVol->vd_dir_seg->ds_last_seg == 1) &&
       (pVol->vd_dir_seg->ds_extra == 0) )
    pVol->vd_status = OK;

  return pVol->vd_status;
}

/*******************************************************************************
 * rt11FsVolBlkRW - Read/write block from/to volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsVolBlkRW(RT_VOL_DESC *pVol,
			    int blkNum,
			    int numBlocks,
			    char *buffer,
			    CBIO_RW rw)
{
  CBIO_DEV_ID devId;
  CBIO_PARAMS params;
  int physSector, secNum;

  /* Initialize locals */
  devId = pVol->vd_dev;
  secNum = blkNum * pVol->vd_secBlock;

  /* Get parameters */
  if (cbioParamsGet(devId, &params) != OK)
    return ERROR;

  /* Calculate physical sector */
  if (pVol->vd_rtFmt)
    physSector = rt11FsAbsSector(params.blocksPerTrack, secNum);
  else
    physSector = secNum;

  return cbioBlkRW(devId, physSector, (numBlocks * pVol->vd_secBlock),
	           buffer, rw, NULL);
}

/*******************************************************************************
 * rt11FsFdGet - Get file descriptor
 *
 * RETURNS: Pointer to file descriptor
 ******************************************************************************/

LOCAL RT_FILE_DESC* rt11FsFdGet(void)
{
  RT_FILE_DESC *pFd;
  int i;

  semTake(rt11FsFdSemId, WAIT_FOREVER);

  for (i = 0, pFd = &rt11FsFd[0];
       i < rt11FsMaxFiles;
       i++, pFd++) {

    if (pFd->rfd_status == NOT_IN_USE) {

      pFd->rfd_status = OK;
      semGive(rt11FsFdSemId);
      return pFd;

    }

  }

  semGive(rt11FsFdSemId);

  return NULL;
}

/*******************************************************************************
 * rt11FsFdFlush - Flush file descriptor
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsFdFlush(RT_FILE_DESC *pFd)
{
  int blkNum;

  if (pFd->rfd_modified) {

    blkNum = (pFd->rfd_curptr / RT_BYTES_PER_BLOCK) + pFd->rfd_start;
    if (rt11FsVolBlkRW(pFd->rfd_vdptr, blkNum, 1,
		       pFd->rfd_buffer, CBIO_WRITE) != OK)
      return ERROR;

    pFd->rfd_modified = FALSE;

  }

  return OK;
}

/*******************************************************************************
 * rt11FsFdFree - Free file descriptor
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsFdFree(RT_FILE_DESC *pFd)
{
  semTake(rt11FsFdSemId, WAIT_FOREVER);

  pFd->rfd_status = NOT_IN_USE;

  semGive(rt11FsFdSemId);
}

/*******************************************************************************
 * rt11FsGetEntry - Get directory entry
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsGetEntry(RT_VOL_DESC *pVol,
			  int entryNum,
			  RT_DIR_ENTRY *pEntry)
{
  memcpy(pEntry,
	 &pVol->vd_dir_seg->ds_entries[entryNum],
         sizeof(RT_DIR_ENTRY));
}

/*******************************************************************************
 * rt11FsPutEntry - Put directory entry
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsPutEntry(RT_VOL_DESC *pVol,
			  int entryNum,
			  RT_DIR_ENTRY *pEntry)
{
  pVol->vd_dir_seg->ds_entries[entryNum] = *pEntry;
}

/*******************************************************************************
 * rt11FsInsertEntry - Insert directory entry
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsInsertEntry(RT_VOL_DESC *pVol,
			     int entryNum,
			     RT_DIR_ENTRY *pEntry)
{
  RT_DIR_ENTRY entry, prevEntry;

  /* Replace current entry with new */
  rt11FsGetEntry(pVol, entryNum, &entry);
  rt11FsPutEntry(pVol, entryNum, pEntry);

  /* Replace entry with previous entry */
  while (entry.de_status != DES_END) {

    prevEntry = entry;
    entryNum++;

    rt11FsGetEntry(pVol, entryNum, &entry);
    rt11FsPutEntry(pVol, entryNum, &prevEntry);

  }

  /* Put end indicator in next slot */
  rt11FsPutEntry(pVol, entryNum + 1, &entry);
}

/*******************************************************************************
 * rt11FsFindEntry - Find directory entry with name
 *
 * RETURNS: Index in directory
 ******************************************************************************/

LOCAL int rt11FsFindEntry(RT_VOL_DESC *pVol,
			  char *name,
			  int *pStart)
{
  RT_DIR_ENTRY entry;
  RT_NAME rname;
  int i;

  /* Remove leading slashes */
  while ( (*name == '/') ||
	  (*name == '\\') )
    name++;

  /* Get radix-50 name */
  rt11FsNameR50(name, &rname);
  *pStart = pVol->vd_dir_seg->ds_start;

  /* Search for entry */
  for (i = 0; ; i++) {

    rt11FsGetEntry(pVol, i, &entry);
    if (entry.de_status == DES_END)
      return ERROR;

    /* Check file */
    if ( ((entry.de_status == DES_PERMANENT) ||
	  (entry.de_status == DES_TENTATIVE) ) &&
         (entry.de_name.nm_name1 == rname.nm_name1) &&
	 (entry.de_name.nm_name2 == rname.nm_name2) &&
	 (entry.de_name.nm_type == rname.nm_type) )
      return i;


    /* Not a match */
    *pStart += entry.de_nblocks;
  }

  return ERROR;
}

/*******************************************************************************
 * rt11FsDirMergeEmpty - Merge empty directory entries
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsDirMergeEmpty(RT_VOL_DESC *pVol, int entryNum)
{
  RT_DIR_ENTRY entry, nextEntry;
  int i;

  /* Initialize locals */
  i = 0;

  /* Get previous entry */
  rt11FsGetEntry(pVol, --entryNum, &entry);
  if ( (entryNum < 0) ||
       (entry.de_status != DES_EMPTY) )
    rt11FsGetEntry(pVol, ++entryNum, &entry);

  /* Get next entry */
  rt11FsGetEntry(pVol, entryNum + 1, &nextEntry);

  while (nextEntry.de_status == DES_EMPTY) {

    if ( (entry.de_nblocks + nextEntry.de_nblocks) > RT_MAX_BLOCKS_PER_FILE ) {

      if (i > 0) {

        break;

      }

      else {

        entryNum++;
        entry = nextEntry;

      }

    }

    else {

      entry.de_nblocks += nextEntry.de_nblocks;
      ++i;

    }

    /* Get next entry */
    rt11FsGetEntry(pVol, entryNum + 1 + i, &nextEntry);

  }

  /* If any entries merged */
  if (i > 0) {

    rt11FsPutEntry(pVol, entryNum, &entry);
    rt11FsPutEntry(pVol, ++entryNum, &nextEntry);

    while (nextEntry.de_status != DES_END) {

      rt11FsGetEntry(pVol, ++entryNum + i, &nextEntry);
      rt11FsPutEntry(pVol, entryNum, &nextEntry);

    }

  }

}

/*******************************************************************************
 * rt11FsDirRead - Read directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsDirRead(RT_FILE_DESC *pFd, struct dirent *pDir)
{
  RT_VOL_DESC *pVol;
  RT_DIR_ENTRY *pEntry;
  int entryNum, maxEntries;
  char *pc;
  int namelen;

  /* Initialize locals */
  pVol = pFd->rfd_vdptr;

  semTake(pVol->vd_semId, WAIT_FOREVER);

  if (rt11FsVolVerify(pVol, TRUE) != OK) {

    semGive(pVol->vd_semId);
    return ERROR;

  }

  entryNum = pDir->d_ino;
  maxEntries = (pVol->vd_nSegBlocks * RT_BYTES_PER_BLOCK -
		(sizeof(RT_DIR_SEG) - sizeof(RT_DIR_ENTRY))) /
	       sizeof(RT_DIR_ENTRY);

  /* Do while dir status is empty */
  do {

    if (entryNum >= maxEntries) {

      semGive(pVol->vd_semId);
      return ERROR;

    }

    /* Get entry */
    pEntry = &pVol->vd_dir_seg->ds_entries[entryNum];
    if (pEntry->de_status == DES_END) {

      semGive(pVol->vd_semId);
      return ERROR;

    }

    /* Advance */
    entryNum++;

  } while (pEntry->de_status == DES_EMPTY);

  /* Copy name to dirent */
  rt11FsNameString(pEntry->de_name, pDir->d_name);

  /* Cancel leading spaces and dots */
  namelen = strlen(pDir->d_name);
  if (namelen > 0) {

    pc = pDir->d_name + namelen - 1;
    while ( (*pc == ' ') ||
	    (*pc == '.') ) {

      pc = EOS;
      pc++;

    }

  }

  /* Update entry number */
  pDir->d_ino = entryNum;

  semGive(pVol->vd_semId);

  return OK;
}

/*******************************************************************************
 * rt11FsWhere - Get position in file
 *
 * RETURNS: Position in file
 ******************************************************************************/

LOCAL int rt11FsWhere(RT_FILE_DESC *pFd)
{
  if (pFd->rfd_status == NOT_IN_USE)
    return ERROR;

  return pFd->rfd_newptr;
}

/*******************************************************************************
 * rt11FsSeek - Set position in file
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsSeek(RT_FILE_DESC *pFd, int position)
{
  if (pFd->rfd_status == NOT_IN_USE)
    return ERROR;

  if ( position > (pFd->rfd_dir_entry.de_nblocks * RT_BYTES_PER_BLOCK) )
    return ERROR;

  /* New position */
  pFd->rfd_newptr = position;

  /* Check range */
  if (pFd->rfd_endptr < position)
    pFd->rfd_endptr = position;

  return OK;
}

/*******************************************************************************
 * rt11FsStatGet - Get file status
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsStatGet(RT_FILE_DESC *pFd, struct stat *pStat)
{
  struct tm fileDate;
  time_t timer;

  /* Clear stat struct */
  memset(pStat, 0, sizeof(struct stat));

  /* Setup struct */
  pStat->st_dev = (dev_t) pFd->rfd_vdptr->vd_dev;
  pStat->st_ino = 0;
  pStat->st_nlink = 1;
  pStat->st_uid = 0;
  pStat->st_gid = 0;
  pStat->st_rdev = (dev_t) 0;
  pStat->st_size = (off_t) pFd->rfd_dir_entry.de_nblocks * RT_BYTES_PER_BLOCK;
  pStat->st_blksize = 0;
  pStat->st_blocks = 0;

  /* Get file date */
  rt11FsFileDate(pFd, &fileDate);
  timer = mktime(&fileDate);

  /* Setup file timestamp */
  pStat->st_atime = timer;
  pStat->st_mtime = timer;
  pStat->st_ctime = timer;

  /* Get mode */
  if (pFd->rfd_dir_entry.de_status == DES_BOGUS)
    pStat->st_mode = S_IFDIR;
  else
    pStat->st_mode = S_IFREG;

  /* Get permissions */
  pStat->st_mode |= (S_IRWXU | S_IRWXG | S_IRWXO);

  return OK;

}

/*******************************************************************************
 * rt11FsRename - Change filename
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsRename(RT_FILE_DESC *pFd, char *name)
{
  char oldName[RT_NAME_LEN];
  char *tail;
  RT_DIR_ENTRY entry;
  int start, entryNum;

  /* Device name should not be included */
  if (iosDevFind(name, &tail) != NULL)
    strcpy(name, tail);

  semTake(pFd->rfd_vdptr->vd_semId, WAIT_FOREVER);

  /* Check if any other file already has name */
  if (rt11FsFindEntry(pFd->rfd_vdptr, name, &start) != ERROR) {

    semGive(pFd->rfd_vdptr->vd_semId);
    return ERROR;

  }

  /* Get old name string */
  rt11FsNameString(pFd->rfd_dir_entry.de_name, oldName);

  /* Find old entry */
  entryNum = rt11FsFindEntry(pFd->rfd_vdptr, oldName, &start);
  if (entryNum == ERROR) {

    semGive(pFd->rfd_vdptr->vd_semId);
    return ERROR;

  }

  /* Change name */
  rt11FsGetEntry(pFd->rfd_vdptr, entryNum, &entry);
  rt11FsNameR50(name, &entry.de_name);
  rt11FsNameR50(name, &pFd->rfd_dir_entry.de_name);
  rt11FsPutEntry(pFd->rfd_vdptr, entryNum, &entry);

  if (rt11FsVolFlush(pFd->rfd_vdptr) != OK) {

    semGive(pFd->rfd_vdptr->vd_semId);
    return ERROR;

  }

  semGive(pFd->rfd_vdptr->vd_semId);

  return OK;
}

/*******************************************************************************
 * rt11FsCreate - Create function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL RT_FILE_DESC* rt11FsCreate(RT_VOL_DESC *pVol,
			         char *name,
			         int mode)
{
  RT_FILE_DESC *pFd;
  RT_DIR_ENTRY entry, max_entry;
  CBIO_DEV_ID devId;
  int i, start, max_i, max_start, entryNum, nEntries;

  /* Initialize locals */
  devId = pVol->vd_dev;
  max_start = 0;
  nEntries = (pVol->vd_nSegBlocks * RT_BYTES_PER_BLOCK -
	      (sizeof(RT_DIR_SEG) - sizeof(RT_DIR_ENTRY))) /
	      sizeof(RT_DIR_ENTRY);

  pFd = rt11FsFdGet();
  if (pFd == NULL)
    return (RT_FILE_DESC *) ERROR;

  /* If raw device create */
  if (name[0] == EOS) {

    semTake(pVol->vd_semId, WAIT_FOREVER);

    /* Initialize device */
    if (rt11FsVolVerify(pVol, FALSE) != OK) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Check mode */
    if (rt11FsVolMode(pVol) == O_RDONLY) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    semGive(pVol->vd_semId);

    /* Setup struct */
    pFd->rfd_dir_entry.de_status = DES_BOGUS;
    pFd->rfd_dir_entry.de_date = rt11FsDate(TRUE);
    pFd->rfd_dir_entry.de_nblocks = pVol->vd_nblocks;
    rt11FsNameR50("device.raw", &pFd->rfd_dir_entry.de_name);
    pFd->rfd_start = 0;
    pFd->rfd_endptr = pVol->vd_nblocks * RT_BYTES_PER_BLOCK;

  }

  /* Else file create */
  else {

   /* Ignore leding slashes */
   while ( (*name == '/') ||
	   (*name == '\\') )
     name++;

    /* Delete file if it exists */
    rt11FsDelete(pVol, name);

    semTake(pVol->vd_semId, WAIT_FOREVER);

    /* Mount device */
    if ( (rt11FsVolVerify(pVol, TRUE) != OK) ||
	 (pVol->vd_status != OK) ) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Check mode */
    if (rt11FsVolMode(pVol) == O_RDONLY) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Search directory for biggest empty entry */
    start = pVol->vd_dir_seg->ds_start;
    max_i = NONE;

    for (i = 0; i < nEntries; i++) {

      rt11FsGetEntry(pVol, i, &entry);
      if (entry.de_status == DES_END)
        break;

      /* If create in progress */
      if (entry.de_status == DES_TENTATIVE) {

        semGive(pVol->vd_semId);
        rt11FsFdFree(pFd);
        return (RT_FILE_DESC *) ERROR;

      }

      /* If this is the biggest file so far */
      if ( (entry.de_status == DES_EMPTY) &&
	   ((max_i == NONE) || (entry.de_nblocks > max_entry.de_nblocks)) ) {

        max_i = i;
        max_entry = entry;
        max_start = start;

      }

      /* Advance */
      start += entry.de_nblocks;

    }

    /* If maximum number of files reached */
    if (i >= nEntries - 1) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* If no empty space */
    if (max_i == NONE) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Setup entry */
    max_entry.de_status = DES_TENTATIVE;
    max_entry.de_date = rt11FsDate(TRUE);
    rt11FsNameR50(name, &max_entry.de_name);

    /* Put entry in directory */
    rt11FsPutEntry(pVol, max_i, &max_entry);

    /* Flush */
    rt11FsVolFlush(pVol);

    semGive(pVol->vd_semId);

    /* Setup file desctiptor */
    pFd->rfd_dir_entry = max_entry;
    pFd->rfd_start = max_start;
    pFd->rfd_endptr = 0;

  }

  /* Setup rest of file descriptor */
  pFd->rfd_mode = mode;
  pFd->rfd_vdptr = pVol;
  pFd->rfd_curptr = NONE;
  pFd->rfd_newptr = 0;
  pFd->rfd_modified = FALSE;

  return pFd;
}

/*******************************************************************************
 * rt11FsDelete - Delete function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsDelete(RT_VOL_DESC *pVol, char *name)
{
  RT_DIR_ENTRY entry;
  int start, entryNum;
  STATUS status;

  semTake(pVol->vd_semId, WAIT_FOREVER);

  /* Mount device */
  if ( (rt11FsVolVerify(pVol, TRUE) != OK) ||
       (pVol->vd_status != OK) ) {

    semGive(pVol->vd_semId);
    return ERROR;

  }

  /* Check mode */
  if (rt11FsVolMode(pVol) == O_RDONLY) {

    semGive(pVol->vd_semId);
    return ERROR;

  }

  /* Find entry */
  entryNum = rt11FsFindEntry(pVol, name, &start);
  if (entryNum == ERROR) {

    semGive(pVol->vd_semId);
    return ERROR;

  }

  rt11FsGetEntry(pVol, entryNum, &entry);

  /* Change in struct */
  entry.de_status = DES_EMPTY;
  entry.de_date = rt11FsDate(FALSE);

  /* Update entry */
  rt11FsPutEntry(pVol, entryNum, &entry);
  rt11FsDirMergeEmpty(pVol, entryNum);

  /* Flush */
  status = rt11FsVolFlush(pVol);

  semGive(pVol->vd_semId);

  return status;
}

/*******************************************************************************
 * rt11FsOpen - Open function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL RT_FILE_DESC* rt11FsOpen(RT_VOL_DESC *pVol,
			       char *name,
			       int mode)
{
  RT_FILE_DESC *pFd;
  CBIO_DEV_ID devId;
  int start, entryNum;

  /* Initialize locals */
  devId = pVol->vd_dev;

  pFd = rt11FsFdGet();
  if (pFd == NULL)
    return (RT_FILE_DESC *) ERROR;

  /* If create or truncate */
  if ( (mode & O_CREAT) ||
       (mode & O_TRUNC) ) {

    mode &= ~O_CREAT;

    /* If truncate and non existant */
    if ( (mode & O_TRUNC) ||
	 (rt11FsFindEntry(pVol, name, &start) == ERROR) ) {

      mode &= ~O_TRUNC;
      rt11FsFdFree(pFd);
      return rt11FsCreate(pVol, name, mode);

    }

  }

  /* If raw device open */
  if (name[0] == EOS) {

    semTake(pVol->vd_semId, WAIT_FOREVER);

    /* Initialize device */
    if (rt11FsVolVerify(pVol, FALSE) != OK) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Check mode */
    if (rt11FsVolMode(pVol) < mode) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    semGive(pVol->vd_semId);

    start = 0;

    /* Setup struct */
    pFd->rfd_dir_entry.de_status = DES_BOGUS;
    pFd->rfd_dir_entry.de_date = rt11FsDate(TRUE);
    pFd->rfd_dir_entry.de_nblocks = pVol->vd_nblocks;
    rt11FsNameR50("device.raw", &pFd->rfd_dir_entry.de_name);

  }

  /* Else file open */
  else {

    semTake(pVol->vd_semId, WAIT_FOREVER);

    /* Mount device */
    if ( (rt11FsVolVerify(pVol, TRUE) != OK) ||
	 (pVol->vd_status != OK) ) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Check mode */
    if (rt11FsVolMode(pVol) < mode) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Search for entry */
    entryNum = rt11FsFindEntry(pVol, name, &start);
    if (entryNum == ERROR) {

      semGive(pVol->vd_semId);
      rt11FsFdFree(pFd);
      return (RT_FILE_DESC *) ERROR;

    }

    /* Get entry */
    rt11FsGetEntry(pVol, entryNum, &pFd->rfd_dir_entry);

    semGive(pVol->vd_semId);
  }

  /* Setup struct */
  pFd->rfd_mode = mode;
  pFd->rfd_vdptr = pVol;
  pFd->rfd_start = start;
  pFd->rfd_curptr = NONE;
  pFd->rfd_newptr = 0;
  pFd->rfd_endptr = pFd->rfd_dir_entry.de_nblocks * RT_BYTES_PER_BLOCK;
  pFd->rfd_modified = FALSE;

  return pFd;
}

/*******************************************************************************
 * rt11FsClose - Close function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsClose(RT_FILE_DESC *pFd)
{
  RT_DIR_ENTRY *pEntry;
  int i, nBlocks, blksLeft, entryNum, start;
  char name[RT_NAME_LEN];
  STATUS status;

  /* Initialize locals */
  pEntry = &pFd->rfd_dir_entry;

  /* If modified */
  if (pFd->rfd_modified &&
      ((pFd->rfd_curptr / RT_BYTES_PER_BLOCK) ==
       (pFd->rfd_endptr / RT_BYTES_PER_BLOCK))) {

    i = pFd->rfd_endptr - pFd->rfd_curptr;
    memset(&pFd->rfd_buffer[i], 0, RT_BYTES_PER_BLOCK - i);

  }

  /* Flush */
  status = rt11FsFdFlush(pFd);

  /* If new file update directory */
  if (pEntry->de_status == DES_TENTATIVE) {

    nBlocks = ((pFd->rfd_endptr - 1) / RT_BYTES_PER_BLOCK) + 1;
    blksLeft = pEntry->de_nblocks - nBlocks;

    /* Mark file as permanent */
    pEntry->de_status = DES_PERMANENT;
    pEntry->de_nblocks = nBlocks;

    semTake(pFd->rfd_vdptr->vd_semId, WAIT_FOREVER);

    /* Get name string */
    rt11FsNameString(pFd->rfd_dir_entry.de_name, name);

    /* Get entry number */
    entryNum = rt11FsFindEntry(pFd->rfd_vdptr, name, &start);
    if (entryNum == ERROR) {

      status = ERROR;

    }

    else {

      /* Put entry */
      rt11FsPutEntry(pFd->rfd_vdptr, entryNum, pEntry);

      /* If entry space left */
      if (blksLeft != 0) {

        pEntry->de_status = DES_EMPTY;
        pEntry->de_nblocks = blksLeft;

        /* Insert entry */
        rt11FsInsertEntry(pFd->rfd_vdptr, entryNum + 1, pEntry);
         rt11FsDirMergeEmpty(pFd->rfd_vdptr, entryNum + 1);

      }

      /* Flush */
      status = rt11FsVolFlush(pFd->rfd_vdptr);

    }

    semGive(pFd->rfd_vdptr->vd_semId);

  }

  /* Free file descriptor */
  rt11FsFdFree(pFd);

  return status;
}

/*******************************************************************************
 * rt11FsRead - Read function
 *
 * RETURNS: Bytes read
 ******************************************************************************/

LOCAL int rt11FsRead(RT_FILE_DESC *pFd, char *buffer, int maxBytes)
{
  int i, bytesRead, bytesLeft;
  int startBlock, currBlock, numBlocks;
  int ret;

  /* If invalid space */
  if (maxBytes <= 0)
    return ERROR;

  /* Initialize locals */
  bytesLeft = maxBytes;

  /* Read loop */
  while (bytesLeft > 0) {

    /* If data within space space */
    if ( (bytesLeft >= RT_BYTES_PER_BLOCK) &&
	 (pFd->rfd_newptr % RT_BYTES_PER_BLOCK == 0) &&
	 (pFd->rfd_newptr + bytesLeft <= (pFd->rfd_dir_entry.de_nblocks *
					  RT_BYTES_PER_BLOCK)) ) {

      /* Number of blocks */
      if (pFd->rfd_vdptr->vd_rtFmt)
        numBlocks = 1;
      else
        numBlocks = bytesLeft / RT_BYTES_PER_BLOCK;

      /* Starting block */
      startBlock = (pFd->rfd_newptr / RT_BYTES_PER_BLOCK) + pFd->rfd_start;

      /* Read */
      if (rt11FsVolBlkRW(pFd->rfd_vdptr, startBlock, numBlocks,
		         buffer, CBIO_READ) != OK) {

        if (bytesLeft == maxBytes)
          return ERROR;
        else
          break;
      }

      /* If modified */
      if (pFd->rfd_modified) {

        currBlock = (pFd->rfd_curptr / RT_BYTES_PER_BLOCK) + pFd->rfd_start;
        if ( (currBlock >= startBlock) &&
	     (currBlock < (startBlock + numBlocks)) )
          memcpy(buffer + ((currBlock - startBlock) * RT_BYTES_PER_BLOCK),
		 pFd->rfd_buffer,
		 RT_BYTES_PER_BLOCK);

      }

      bytesRead = numBlocks * RT_BYTES_PER_BLOCK;

    }

    /* Else data in extra block space */
    else {

      ret = rt11FsNewBlk(pFd);
      if (ret <= 0) {

        if (bytesLeft == maxBytes)
          return ret;
        else
          break;

      }

      i = pFd->rfd_newptr % RT_BYTES_PER_BLOCK;
      bytesRead = min(bytesLeft, RT_BYTES_PER_BLOCK - i);
      memcpy(buffer, &pFd->rfd_buffer[i], bytesRead);

    }

    /* Advance */
    bytesLeft -= bytesRead;
    buffer += bytesRead;
    pFd->rfd_newptr += bytesRead;

  } /* End read loop */

  return (maxBytes - bytesLeft);
}

/*******************************************************************************
 * rt11FsWrite - Write function
 *
 * RETURNS: Bytes written
 ******************************************************************************/

LOCAL int rt11FsWrite(RT_FILE_DESC *pFd, char *buffer, int maxBytes)
{
  int i, bytesWritten, bytesLeft;
  int startBlock, currBlock, numBlocks;

  /* If invalid space */
  if (maxBytes <= 0)
    return ERROR;

  /* Initialize locals */
  bytesLeft = maxBytes;

  /* Write loop */
  while (bytesLeft > 0) {

    /* If data within space space */
    if ( (bytesLeft >= RT_BYTES_PER_BLOCK) &&
	 (pFd->rfd_newptr % RT_BYTES_PER_BLOCK == 0) &&
	 (pFd->rfd_newptr + bytesLeft <= (pFd->rfd_dir_entry.de_nblocks *
					  RT_BYTES_PER_BLOCK)) ) {

      /* Number of blocks */
      if (pFd->rfd_vdptr->vd_rtFmt)
        numBlocks = 1;
      else
        numBlocks = bytesLeft / RT_BYTES_PER_BLOCK;

      /* Starting block */
      startBlock = (pFd->rfd_newptr / RT_BYTES_PER_BLOCK) + pFd->rfd_start;

      /* Write */
      if (rt11FsVolBlkRW(pFd->rfd_vdptr, startBlock, numBlocks,
		         buffer, CBIO_WRITE) != OK)
        return ERROR;

      /* Current block */
      currBlock = (pFd->rfd_curptr / RT_BYTES_PER_BLOCK) + pFd->rfd_start;
      if ( (currBlock >= startBlock) &&
	   (currBlock < (startBlock + numBlocks)) ) {

        pFd->rfd_curptr = NONE;
        pFd->rfd_modified = FALSE;

      }

      /* Advance */
      bytesWritten = numBlocks * RT_BYTES_PER_BLOCK;

    }

    /* Else data in extra block space */
    else {

      /* Request new block */
      if (rt11FsNewBlk(pFd) <= 0)
        return ERROR;

      i = pFd->rfd_newptr % RT_BYTES_PER_BLOCK;
      bytesWritten = min(bytesLeft, RT_BYTES_PER_BLOCK - i);
      memcpy(&pFd->rfd_buffer[i], buffer, bytesWritten);

      /* File descriptor modified */
      pFd->rfd_modified = TRUE;

    }

    /* Advance */
    pFd->rfd_newptr += bytesWritten;
    if (pFd->rfd_endptr < pFd->rfd_newptr)
      pFd->rfd_endptr = pFd->rfd_newptr;

    buffer += bytesWritten;
    bytesLeft -= bytesWritten;

  } /* End write loop */

  return (maxBytes - bytesLeft);
}

/*******************************************************************************
 * rt11FsIoctl - Ioctl function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rt11FsIoctl(RT_FILE_DESC *pFd,
			 int cmd,
			 ARG arg)
{
  int pos;
  /* Select command */
  switch(cmd) {

    case FIODISKINIT:
      return rt11FsVolInit(pFd->rfd_vdptr);

    case FIOREADDIR:
      return rt11FsDirRead(pFd, (struct dirent *) arg);

    case FIOWHERE:
      *((int *) arg) = rt11FsWhere(pFd);
      return OK;

    case FIOSEEK:
      return rt11FsSeek(pFd, (int) arg);

    case FIOSTATGET:
      return rt11FsStatGet(pFd, (struct stat *) arg);

    case FIORENAME:
      return rt11FsRename(pFd, (char *) arg);

    case FIOFLUSH:
      return rt11FsFdFlush(pFd);

    case FIONREAD:
      pos = rt11FsWhere(pFd);
      if (pos == ERROR)
        return ERROR;

      *((int *) arg) = pFd->rfd_endptr - pos;
      return OK;

    case FIODISKCHANGE:
      rt11FsReadyChange(pFd->rfd_vdptr);
      return OK;

    default:
      return cbioIoctl(pFd->rfd_vdptr->vd_dev, cmd, arg);

  }

  return ERROR;
}

/*******************************************************************************
 * rt11FsR50out - Convert up to 3 ASCII chars to radix-50
 *
 * RETURNS: Radix-50 number
 ******************************************************************************/

LOCAL int rt11FsR50out(char *string)
{
  unsigned int r50;
  int i, r;
  char ch;

  /* Initialize locals */
  r50 = 0;

  for (i = 0; i < 3; i++) {

    if (*string == EOS) {

      r = 0;

    }

    else {

      ch = *string;

      /* Convert to lowercase */
      if (__isupper(ch))
        ch = __tolower(ch);

      r = (char *) strchr(rad50, ch) - rad50;
      string++;

    }

    r50 = (50 * r50) + r;

  }

  return r50;

}

/*******************************************************************************
 * rt11FsR50in - Convert radix-50 to 3 ASCII chars
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsR50in(unsigned int r50, char *string)
{
  int i;

  for (i = 2; i >= 0; i--) {

    string[i] = rad50[r50 % 50];

    r50 /= 50;

  }

}

/*******************************************************************************
 * rt11FsNameR50 - Convert ASCII string to radix-50 name
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsNameR50(char *string, RT_NAME *pName)
{
  char tmp[RT_NAME_LEN];
  char *cp;

  strncpy(tmp, string, RT_NAME_LEN - 1);
  tmp[RT_NAME_LEN - 1] = EOS;

  /* Find dot in filenmae */
  cp = strchr(tmp, '.');

  if (cp == NULL) {

    pName->nm_type = 0;

  }

  else {

    *cp = EOS;
    pName->nm_type = rt11FsR50out(cp + 1);

  }

  /* Convert rest of filename */
  pName->nm_name1 = rt11FsR50out(tmp);

  if (strlen(tmp) <= 3)
    pName->nm_name2 = 0;
  else
    pName->nm_name2 = rt11FsR50out(tmp + 3);
}

/*******************************************************************************
 * rt11FsNameString - Convert radix-50 name string to ASCII string
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsNameString(RT_NAME name, char *string)
{
  char *pString;

  rt11FsR50in(name.nm_name1, string);
  rt11FsR50in(name.nm_name2, string + 3);

  for (pString = string;
       (pString < (string + 6)) && (*pString != ' ');
       ++pString);

  *pString++ = '.';

  rt11FsR50in(name.nm_type, pString);

  *(pString + 3) = EOS;
}

/*******************************************************************************
 * rt11FsDate - Get encoded date
 *
 * RETURNS: Date
 ******************************************************************************/

LOCAL int rt11FsDate(BOOL now)
{
  time_t currTime;
  struct tm currDate;
  int year, month, day;

  /* Should get current date here */
  if (now) {

    currTime = time(NULL);

    if (currTime == ERROR) {

      year = 72;
      month = 0;
      day = 1;

    }

    else {

      localtime_r(&currTime, &currDate);
      year = currDate.tm_year;
      month = currDate.tm_mon;
      day = currDate.tm_mday;

    }

  }

  /* Else zero date */
  else {

    year = 72;
    month = 0;
    year = 0;

  }

  /* Year 72 is year 0 */
  year -= 72;
  if (year < 0)
    year += 100;

  return ( (month << 10) | (day << 5) | (year & 0x1f) );
}

/*******************************************************************************
 * rt11FsFileDate - Get file modification date
 *
 * RETURNS: Byte position of block
 ******************************************************************************/

LOCAL void rt11FsFileDate(RT_FILE_DESC *pFd, struct tm *pDate)
{
  /* Clear tm struct */
  memset(pDate, 0, sizeof(struct tm));

  /* Get day */
  pDate->tm_mday = (pFd->rfd_dir_entry.de_date >> 5) & 0x001f;
  if (pDate->tm_mday == 0)
    pDate->tm_mday = 1;

  /* Get month */
  pDate->tm_mon = (pFd->rfd_dir_entry.de_date >> 10) & 0x000f;

  /* Get year */
  pDate->tm_year = (pFd->rfd_dir_entry.de_date & 0x001f) + 72;
}

/*******************************************************************************
 * rt11FsNewBlk - Add new block for file descriptor
 *
 * RETURNS: Byte position of block
 ******************************************************************************/

LOCAL int rt11FsNewBlk(RT_FILE_DESC *pFd)
{
  int currBlock, newBlock;

  /* Initialize locals */
  newBlock = pFd->rfd_newptr / RT_BYTES_PER_BLOCK;

  if (pFd->rfd_curptr == NONE)
    currBlock = NONE;
  else
    currBlock = pFd->rfd_curptr / RT_BYTES_PER_BLOCK;

  /* If new block inside current */
  if (newBlock == currBlock)
    return RT_BYTES_PER_BLOCK;

  /* If past end-of-file */
  if (newBlock >= pFd->rfd_dir_entry.de_nblocks)
    return 0;

  /* Flush fd */
  if (rt11FsFdFlush(pFd) != OK)
    return ERROR;

  /* If not write-only mode */
  if (pFd->rfd_mode != O_WRONLY) {

    if (rt11FsVolBlkRW(pFd->rfd_vdptr, newBlock + pFd->rfd_start, 1,
		       pFd->rfd_buffer, CBIO_READ) != OK)
      return ERROR;
  }

  pFd->rfd_curptr = newBlock * RT_BYTES_PER_BLOCK;

  return RT_BYTES_PER_BLOCK;
}

/*******************************************************************************
 * rt11FsAbsSector - Calculate absolute sector
 *
 * RETURNS: Sector number
 ******************************************************************************/

LOCAL int rt11FsAbsSector(unsigned long secPerTrack, int sector)
{
  int physSector, track, trkSector;

  /* Initialize locals */
  track = sector / secPerTrack;
  trkSector = sector % secPerTrack;

  physSector = ((trkSector * 2) + (track * 6)) % secPerTrack;
  if (trkSector >= (secPerTrack / 2))
    physSector++;

  return (track * secPerTrack * physSector);
}

/*******************************************************************************
 * rt11FsSwapBytes - Swap bytes
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rt11FsSwapBytes(char *pSrc, char *pDest, int nBytes)
{
  short *src, *dst, *dst_end;

  /* Initialize locals */
  src = (short *) pSrc;
  dst = (short *) pDest;
  dst_end = (short *) dst + (nBytes / 2);

  for (; dst < dst_end; dst++, src++)
    *dst = ((*src & 0x00ff) << 8) | ((*src & 0xff00) >> 8);
}

