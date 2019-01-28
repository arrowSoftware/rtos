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

/* dosFatLib.c - Dos FAT library */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <io/cbioLib.h>
#include <os/logLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/private/dosFsLibP.h>
#include <oldfs/private/dosFatLibP.h>

/* Defines */
#define FSINFO_SEC_NUM		1
#define FSINFO_SIGN		484
#define FSINFO_FREE_CLUSTS	488
#define FSINFO_NEXT_FREE	492

/* Macros */
#define ENTRY_READ(pFat, pFd, copy, entry)				       \
  (* (pFat)->entryRead) ( (pFd), (copy), (entry) )

#define ENTRY_WRITE(pFat, pFd, copy, entry, value)			       \
  (* (pFat)->entryWrite) ( (pFd), (copy), (entry), (value) )

#define FAT_READ_ERR		1

/* Imports */
IMPORT DOS_HDLR_DESC dosFatHdlrsList[];

IMPORT STATUS dosFsHdlrInstall(DOS_HDLR_DESC *hdlrList,
			       DOS_HDLR_DESC *hdlr);

/* Locals */
LOCAL STATUS dosFatVolMount(DOS_VOL_DESC_ID volId, ARG arg);
LOCAL void dosFatVolUnmount(DOS_VOL_DESC_ID volId);

LOCAL STATUS dosFatContigGet(DOS_FILE_DESC *pFd, u_int32_t numClusts);
LOCAL STATUS dosFatMarkAlloc(DOS_FILE_DESC *pFd,
			     u_int32_t firstClust,
			     u_int32_t numClusts);

LOCAL STATUS dosFatGetNext(DOS_FILE_DESC *pFd, unsigned int allocPolicy);
LOCAL STATUS dosFatShow(DOS_VOL_DESC_ID volId);
LOCAL STATUS dosFatTruncate(DOS_FILE_DESC *pFd,
			    u_int32_t sector,
			    u_int32_t flags);
LOCAL STATUS dosFatSeek(DOS_FILE_DESC *pFd,
			u_int32_t sector,
			u_int32_t secOff);
LOCAL fsize_t dosFatNFree(DOS_FILE_DESC *pFd);
LOCAL STATUS dosFatMirrorSect(DOS_FILE_DESC *pFd);

LOCAL u_int32_t dosFat12EntryRead(DOS_FILE_DESC *pFd,
				  u_int32_t copyNum,
				  u_int32_t cluster);
LOCAL STATUS dosFat12EntryWrite(DOS_FILE_DESC *pFd,
				u_int32_t copyNum,
				u_int32_t cluster,
				u_int32_t value);

LOCAL STATUS dosFatEmpty(DOS_VOL_DESC_ID volId, ARG arg);

/* Globals */
int fatClustFactor = 10000;
int fatSemOptions = SEM_Q_PRIORITY | SEM_DELETE_SAFE;

/* Functions */

/*******************************************************************************
 * dosFatLibInit - Initialize dos directory library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosFatLibInit(void)
{
  DOS_HDLR_DESC hdlr;

  /* Setup struct */
  hdlr.id = DOS_FAT_HDLR_ID;
  hdlr.mountFunc = (FUNCPTR) dosFatVolMount;
  hdlr.arg = (ARG) 0;

  return dosFsHdlrInstall(dosFatHdlrsList, &hdlr);
}

/*******************************************************************************
 * dosFatVolMount - Mount function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFatVolMount(DOS_VOL_DESC_ID volId, ARG arg)
{
  CBIO_DEV_ID devId;
  FAT_INFO *pFat;
  DOS_FILE_DESC fDesc;
  u_int8_t buf[8];

  /* Initialize locals */
  devId = volId->pCbio;
  memset(&fDesc, 0, sizeof(DOS_FILE_DESC));

  /* Unmount if needed */
  if ( (volId->pFatDesc != NULL) &&
       (volId->pFatDesc->volUnmount != ((VOIDFUNCPTR) dosFatVolUnmount)) &&
       (volId->pFatDesc->volUnmount != NULL) )
    ( *volId->pFatDesc->volUnmount) (volId);

  /* Allocate struct */
  volId->pFatDesc = (DOS_FAT_DESC *) realloc(volId->pFatDesc,
					     sizeof(FAT_INFO) );
  if (volId->pFatDesc == NULL)
    return ERROR;

  /* Setup pointer and clear */
  pFat = (FAT_INFO *) volId->pFatDesc;
  memset(pFat, 0, sizeof(FAT_INFO));

  /* Setup volume */
  fDesc.pVolDesc = volId;

  /* Calculate number of fat entries */
  pFat->nFatEnts = ( ((volId->totalSec - volId->dataStartSec) /
		       volId->secPerClust) + DOS_MIN_CLUST );
  volId->nFatEnts = pFat->nFatEnts;

  /* If FAT32 */
  if (volId->fatType == FAT32) {

    if (cbioBytesRW(devId, FSINFO_SEC_NUM, FSINFO_FREE_CLUSTS,
		    (char *) buf, sizeof(buf), CBIO_READ, NULL) != OK)
      goto mountError;

    /* Setup struct */
    pFat->fatEntFreeCnt = DISK_TO_32BIT(&buf[0]);
    pFat->groupAllocStart = DISK_TO_32BIT(&buf[4]);

    if ( (pFat->groupAllocStart < DOS_MIN_CLUST) ||
	 (pFat->groupAllocStart >= pFat->nFatEnts) )
      pFat->groupAllocStart = DOS_MIN_CLUST;

    pFat->entryRead = (FUNCPTR) dosFatEmpty;
    pFat->entryWrite = (FUNCPTR) dosFatEmpty;
    pFat->dos_fat_reserv = 0xfffffff0;
    pFat->dos_fat_bad = 0xfffffff7;
    pFat->dos_fat_eof = 0xffffffff;

  }

  /* Else FAT16 or less */
  else {

    pFat->fatEntFreeCnt = -1;
    pFat->groupAllocStart = DOS_MIN_CLUST;

    /* If FAT16 */
    if (volId->fatType == FAT16) {

      pFat->entryRead = (FUNCPTR) dosFatEmpty;
      pFat->entryWrite = (FUNCPTR) dosFatEmpty;
      pFat->dos_fat_reserv = 0xfff0;
      pFat->dos_fat_bad = 0xfff7;
      pFat->dos_fat_eof = 0xffff;

    }

    /* Else if FAT12 */
    else if (volId->fatType == FAT12) {

      pFat->entryRead = (FUNCPTR) dosFat12EntryRead;
      pFat->entryWrite = (FUNCPTR) dosFat12EntryWrite;
      pFat->dos_fat_reserv = 0xff0;
      pFat->dos_fat_bad = 0xff7;
      pFat->dos_fat_eof = 0xfff;

    }

    else {

      goto mountError;

    }

  }

  pFat->dosFatDesc.volUnmount = (VOIDFUNCPTR) dosFatVolUnmount;
  pFat->dosFatDesc.getNext = (FUNCPTR) dosFatGetNext;
  pFat->dosFatDesc.contigChk = (FUNCPTR) dosFatEmpty;
  pFat->dosFatDesc.show = (FUNCPTR) dosFatShow;
  pFat->dosFatDesc.truncate = (FUNCPTR) dosFatTruncate;
  pFat->dosFatDesc.seek = (FUNCPTR) dosFatSeek;
  pFat->dosFatDesc.contigAlloc = (FUNCPTR) dosFatEmpty;
  pFat->dosFatDesc.maxContig = (FUNCPTR) dosFatEmpty;
  pFat->dosFatDesc.nFree = (FUNCPTR) dosFatEmpty;
  pFat->dosFatDesc.flush = (FUNCPTR) dosFatMirrorSect;
  pFat->dosFatDesc.syncToggle = (VOIDFUNCPTR) dosFatEmpty;
  pFat->dosFatDesc.clustValueSet = (FUNCPTR) dosFatEmpty;
  pFat->dosFatDesc.clustValueGet = (FUNCPTR) dosFatEmpty;

  pFat->fatStartSec = volId->nReservedSecs;
  pFat->dosFatDesc.activeCopyNum = 0;

  pFat->fatGroupSize = pFat->nFatEnts / fatClustFactor + 1;
  pFat->dos_fat_avail = 0x00000000;
  pFat->syncEnabled = TRUE;
  pFat->clustAllocStart = DOS_MIN_CLUST;

  /* Create semaphore */
  pFat->allocSem = semMCreate(fatSemOptions);
  if (pFat->allocSem == NULL)
    goto mountError;

  dosFatNFree(&fDesc);

  return OK;

mountError:

  dosFatVolUnmount(volId);

  return ERROR;
}

/*******************************************************************************
 * dosFatVolUnmount - Unmount function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL void dosFatVolUnmount(DOS_VOL_DESC_ID volId)
{
}

/*******************************************************************************
 * dosFatContigGet - Get next cluster for a file
 *
 * RETURNS: Resulting chain of sectors
 ******************************************************************************/

LOCAL STATUS dosFatContigGet(DOS_FILE_DESC *pFd, u_int32_t numClusts)
{
  DOS_VOL_DESC_ID volId;
  FAT_INFO *pFat;
  DOS_FILE_HDL *pFileHdl;
  DOS_FAT_HDL * pFatHdl;
  u_int32_t startClust, nextClust;
  u_int32_t cluster, maxClust;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pFat = (FAT_INFO *) volId->pFatDesc;
  pFileHdl = pFd->pFileHdl;
  pFatHdl = &pFd->fatHdl;
  startClust = pFatHdl->nextClust;
  nextClust = startClust;
  cluster = pFatHdl->lastClust;

  if ( (startClust < DOS_MIN_CLUST) ||
       (startClust >= pFat->nFatEnts) ) {

    /* File just opened */
    if ( (startClust == 0) &&
	 (cluster == 0) )
      startClust = pFileHdl->startClust;

    /* End of chain */
    if (startClust > pFat->dos_fat_bad)
      startClust = ENTRY_READ(pFat,
			      pFd,
			      pFat->dosFatDesc.activeCopyNum,
			      cluster);

    if ( (startClust < DOS_MIN_CLUST) ||
	 (startClust >= pFat->nFatEnts) ) {

      assert( (startClust == 0) ||
	      (startClust > pFat->dos_fat_bad) );
      pFatHdl->nextClust = startClust;
      return ERROR;

    }

  }

  /* If in contiguous area */
  if ( (startClust < pFileHdl->contigEndPlus1) &&
       (startClust >= pFileHdl->startClust) ) {

    cluster = startClust + numClusts;
    if (cluster >= pFileHdl->contigEndPlus1) {

      cluster = pFileHdl->contigEndPlus1;
      nextClust = pFat->dos_fat_eof;

    }

    else {

      nextClust = cluster;

    }

  }

  /* Else not in contiguous area */
  else {

    maxClust = startClust + numClusts;
    if (maxClust > pFat->nFatEnts)
      maxClust = pFat->nFatEnts;

    cluster = startClust;
    while (cluster < maxClust) {

      nextClust = ENTRY_READ(pFat,
			     pFd,
			     pFat->dosFatDesc.activeCopyNum,
			     cluster);
      if (nextClust != ++cluster)
        break;

    }

    if (pFatHdl->errCode == FAT_READ_ERR)
      return ERROR;

  }

  /* Store contents of list entry in contiguous area */
  pFatHdl->nextClust = nextClust;
  pFatHdl->lastClust = cluster - 1;

  pFd->curSec = CLUST_TO_SEC(volId, startClust);
  pFd->nSec = (cluster - startClust) * volId->secPerClust;

  return OK;
}

/*******************************************************************************
 * dosFatMarkAlloc - Allocate contiguous set of clusters
 *
 * RETURNS: Resulting chain of sectors
 ******************************************************************************/

LOCAL STATUS dosFatMarkAlloc(DOS_FILE_DESC *pFd,
			     u_int32_t firstClust,
			     u_int32_t numClusts)
{
  FAT_INFO *pFat;
  u_int32_t currClust;

  /* Initialize locals */
  pFat = (FAT_INFO *) pFd->pVolDesc->pFatDesc;

  assert( (firstClust >= DOS_MIN_CLUST) &&
	  (firstClust < pFat->nFatEnts) );
  assert(numClusts <= (pFat->nFatEnts - DOS_MIN_CLUST));

  /* Build cluster chain */
  for (currClust = firstClust;
       currClust < (firstClust + numClusts - 1);
       currClust++) {

    /* Allocate entry */
    if (ENTRY_WRITE(pFat,
		    pFd,
		    pFat->dosFatDesc.activeCopyNum,
		    currClust,
		    currClust + 1) != OK)
      return ERROR;

    pFat->fatEntFreeCnt--;

  }

  /* Write last cluster as end-of-chain */
  if (ENTRY_WRITE(pFat,
		  pFd,
		  pFat->dosFatDesc.activeCopyNum,
		  currClust,
		  pFat->dos_fat_eof) != OK)
    return ERROR;

  pFat->fatEntFreeCnt--;

  return OK;
}

/*******************************************************************************
 * dosFatGetNext - Get next cluster for a file
 *
 * RETURNS: Resulting chain of sectors
 ******************************************************************************/

LOCAL STATUS dosFatGetNext(DOS_FILE_DESC *pFd, unsigned int allocPolicy)
{
  DOS_VOL_DESC_ID volId;
  FAT_INFO *pFat;
  DOS_FILE_HDL *pFileHdl;
  DOS_FAT_HDL * pFatHdl;
  int i;
  u_int32_t startClust, numClusts;
  u_int32_t prevClust, firstClust;
  u_int32_t contigCount;
  u_int32_t currClust, fatEntry, maxClust;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pFat = (FAT_INFO *) volId->pFatDesc;
  pFileHdl = pFd->pFileHdl;
  pFatHdl = &pFd->fatHdl;

  /* Try to get contiguous section */
  if (dosFatContigGet(pFd, pFat->fatGroupSize) == OK)
    return OK;

  if (pFatHdl->errCode == FAT_READ_ERR)
    return ERROR;

  if ( (allocPolicy & FAT_ALLOC) == 0)
    return ERROR;

  firstClust = pFatHdl->nextClust;
  prevClust = pFatHdl->lastClust;
  startClust = 0;

  if (pFat->groupAllocStart == 0)
    allocPolicy = FAT_ALLOC_ONE;

  if (allocPolicy == FAT_ALLOC_ONE)
    numClusts = 1;
  else
    numClusts = pFat->fatGroupSize;

  /* If end of chain */
  if (firstClust > pFat->dos_fat_bad) {

    startClust = prevClust;

  }

  /* Else if new file */
  else if ( (firstClust == 0) &&
	    (prevClust == 0) ) {

    if (allocPolicy == FAT_ALLOC_ONE)
      startClust = pFat->clustAllocStart;
    else
      startClust = pFat->groupAllocStart;

  }

  /* If zero */
  if (startClust == 0)
    return ERROR;

  /* Allocate clusters */
  maxClust = pFat->nFatEnts - 1;
  currClust = startClust;
  firstClust = 0;
  contigCount = 0;

  semTake(pFat->allocSem, WAIT_FOREVER);

  /* For each pass */
  for (i = 0; i < 2; i++) {

    /* While cluster in range */
    while (currClust <= maxClust) {

      fatEntry = ENTRY_READ(pFat,
			    pFd,
			    pFat->dosFatDesc.activeCopyNum,
			    currClust);
      if ( (fatEntry == FAT_READ_ERR) &&
	   (pFatHdl->errCode == FAT_READ_ERR) )
        goto groupAllocError;

      /* If free space found */
      if (fatEntry == pFat->dos_fat_avail) {

        if (contigCount == 0)
          firstClust = currClust;

        if (++contigCount == numClusts)
          goto groupAlloc;

      }

      /* Else space used */
      else {

        contigCount = 0;

      }

      /* Advace */
      currClust++;

    } /* End while cluster in range */

    maxClust = startClust - 1;
    currClust = DOS_MIN_CLUST;
    contigCount = 0;

  } /* End for each pass */

  if (firstClust == 0) {

    logMsg("!!! DISK FULL !!!\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);
    goto groupAllocError;

  }

  pFat->groupAllocStart = 0;
  numClusts = 1;

groupAlloc:

  if (dosFatMarkAlloc(pFd, firstClust, numClusts) != OK)
    goto groupAllocError;

  semGive(pFat->allocSem);

  if (startClust == prevClust) {

    if (ENTRY_WRITE(pFat,
		    pFd,
		    pFat->dosFatDesc.activeCopyNum,
		    prevClust,
		    firstClust) != OK)
      return ERROR;

    if (firstClust == pFileHdl->contigEndPlus1)
      pFileHdl->contigEndPlus1 = firstClust + numClusts;

  }

  else {

    if (allocPolicy == FAT_ALLOC_ONE) {

      pFat->clustAllocStart = firstClust + 1;

    }

    else {

      if (pFat->groupAllocStart != 0)
        pFat->groupAllocStart = firstClust + numClusts;

    }

    pFileHdl->startClust = firstClust;
    pFileHdl->contigEndPlus1 = firstClust + numClusts;

  }

  pFatHdl->nextClust = pFat->dos_fat_eof;
  pFatHdl->lastClust = firstClust + numClusts - 1;

  pFd->curSec = CLUST_TO_SEC(volId, firstClust);
  pFd->nSec = numClusts * volId->secPerClust;

  return OK;

groupAllocError:

  semGive(pFat->allocSem);

  return ERROR;
}

/*******************************************************************************
 * dosFatShow - Show FAT info
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS dosFatShow(DOS_VOL_DESC_ID volId)
{
  FAT_INFO *pFat;
  DOS_FILE_DESC fDesc;

  /* Inititalize locals */
  pFat = (FAT_INFO *) volId->pFatDesc;
  memset(&fDesc, 0, sizeof(DOS_FILE_DESC));
  fDesc.pVolDesc = volId;

  printf("FAT handler information:\n");
  printf("------------------------\n");

  printf(" - allocation group size:                  %ld cluster(s)\n",
	 pFat->fatGroupSize);
  printf(" - free space on volume:                   %d\n",
	 dosFatNFree(&fDesc));

  return OK;
}

/*******************************************************************************
 * dosFatTruncate - Truncate chain from cluster
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFatTruncate(DOS_FILE_DESC *pFd,
			    u_int32_t sector,
			    u_int32_t flags)
{
  DOS_VOL_DESC_ID volId;
  FAT_INFO *pFat;
  DOS_FILE_HDL *pFileHdl;
  u_int32_t currClust, nextClust, cluster;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pFat = (FAT_INFO *) volId->pFatDesc;
  pFileHdl = pFd->pFileHdl;

  /* If start of file */
  if (sector == FH_FILE_START) {

    cluster = pFileHdl->startClust;
    pFileHdl->contigEndPlus1 = 0;
    if (cluster < DOS_MIN_CLUST)
      return ERROR;

  }

  /* Else not start of file */
  else {

    if (sector < volId->dataStartSec)
      return ERROR;

    cluster = SEC_TO_CLUST(volId, sector);

  }

  /* If cluster within range */
  if (cluster >= pFat->nFatEnts)
    return ERROR;

  /* Select flags */
  switch (flags) {

    case FH_INCLUDE:

      if ( (sector == FH_FILE_START) ||
	   (((sector - volId->dataStartSec) % volId->secPerClust) == 0) ) {

        currClust = cluster;
        break;

      }

    case FH_EXCLUDE:

      currClust = ENTRY_READ(pFat,
			     pFd,
			     pFat->dosFatDesc.activeCopyNum,
			     cluster);

      /* If end of file */
      if (currClust > pFat->dos_fat_bad)
        return OK;

      /* If invalid cluster */
      if ( (currClust < DOS_MIN_CLUST) ||
	   (currClust >= pFat->nFatEnts) )
        return ERROR;

      /* Mark cluster as end-of-file */
      if (ENTRY_WRITE(pFat,
		      pFd,
		      pFat->dosFatDesc.activeCopyNum,
		      cluster,
		      pFat->dos_fat_eof) != OK)
        return ERROR;

    break;

    default:
      return ERROR;
  }

  if ( (currClust < pFileHdl->contigEndPlus1) &&
       (currClust >= pFileHdl->startClust) )
    pFileHdl->contigEndPlus1 = currClust;

  if (pFat->groupAllocStart == 0)
    pFat->groupAllocStart = currClust;

  if (pFat->clustAllocStart > currClust)
    pFat->clustAllocStart = currClust;

  /* Free cluster chain */
  while ( (currClust >= DOS_MIN_CLUST) &&
	  (currClust < pFat->nFatEnts) ) {

    /* Get next cluster */
    nextClust = ENTRY_READ(pFat,
			   pFd,
			   pFat->dosFatDesc.activeCopyNum,
			   currClust);

    /* Free current cluster */
    if (ENTRY_WRITE(pFat,
		    pFd,
		    pFat->dosFatDesc.activeCopyNum,
		    currClust,
		    pFat->dos_fat_avail) != OK)
      return ERROR;

    /* Update free cluster count */
    pFat->fatEntFreeCnt++;

    /* Advance */
    currClust = nextClust;

  }

  /* If not end-of-file */
  if (currClust <= pFat->dos_fat_bad)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * dosFatSeek - Seek cluster in file chain
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFatSeek(DOS_FILE_DESC *pFd,
			u_int32_t sector,
			u_int32_t secOff)
{
  DOS_VOL_DESC_ID volId;
  FAT_INFO *pFat;
  u_int32_t i, nextClust, clustOff, cluster;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pFat = (FAT_INFO *) volId->pFatDesc;

  /* If file start */
  if (sector == FH_FILE_START) {

    pFd->fatHdl.nextClust = 0;
    pFd->fatHdl.lastClust = 0;
    pFd->fatHdl.cbioCookie = 0;

    pFd->curSec = 0;
    pFd->nSec = 0;

    cluster = pFd->pFileHdl->startClust;
    if (cluster == 0)
      return OK;

    if (cluster < DOS_MIN_CLUST)
      return ERROR;

  }

  /* Else not file start */
  else {

    if (sector < volId->dataStartSec)
      return ERROR;

    cluster = SEC_TO_CLUST(volId, sector);
    secOff += (sector - volId->dataStartSec) % volId->secPerClust;

  }

  /* Check cluster range */
  if (cluster >= pFat->nFatEnts)
    return ERROR;

  clustOff = secOff / volId->secPerClust;
  secOff %= volId->secPerClust;

  /* If contiguous area */
  if ( (cluster < pFd->pFileHdl->contigEndPlus1) &&
       (cluster >= pFd->pFileHdl->startClust) ) {

    i = min(clustOff, pFd->pFileHdl->contigEndPlus1 - cluster - 1);
    cluster += i;
    clustOff -= i;

  }

  /* Seek */
  for (i = 0; i < clustOff; i++) {

    /* Get next cluster */
    nextClust = ENTRY_READ(pFat,
			   pFd,
			   pFat->dosFatDesc.activeCopyNum,
			   cluster);
    if ( (nextClust < DOS_MIN_CLUST) ||
	 (nextClust >= pFat->nFatEnts) )
      return ERROR;

    /* Advance */
    cluster = nextClust;

  }

  /* Store cluster */
  pFd->fatHdl.nextClust = cluster;
  if (dosFatContigGet(pFd, pFat->fatGroupSize) != OK)
    return ERROR;

  /* Advance */
  pFd->curSec += secOff;
  pFd->nSec -= secOff;

  return OK;
}

/*******************************************************************************
 * dosFatNFree - Get number of free bytes on disk
 *
 * RETURNS: Number of free bytes
 ******************************************************************************/

LOCAL fsize_t dosFatNFree(DOS_FILE_DESC *pFd)
{
  DOS_VOL_DESC_ID volId;
  FAT_INFO *pFat;
  u_int32_t clustNum, fatEntry, freeCount;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pFat = (FAT_INFO *) volId->pFatDesc;
  freeCount = pFat->fatEntFreeCnt;
  if (freeCount == 0xffffffff) {

    freeCount = 0;
    for (clustNum = DOS_MIN_CLUST; clustNum < pFat->nFatEnts; clustNum++) {

      fatEntry = ENTRY_READ(pFat,
			    pFd,
			    pFat->dosFatDesc.activeCopyNum,
			    clustNum);
      if ( (fatEntry == FAT_READ_ERR) &&
	   (pFd->fatHdl.errCode == FAT_READ_ERR) )
        return ERROR;

      if (fatEntry == pFat->dos_fat_avail)
        freeCount++;

    }

    pFat->fatEntFreeCnt = freeCount;
  }

  return ( (((fsize_t) freeCount) * volId->secPerClust) << volId->secSizeShift);
}

/*******************************************************************************
 * dosFatMirrorSect - Mirror FAT sector
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFatMirrorSect(DOS_FILE_DESC *pFd)
{
  DOS_VOL_DESC_ID volId;
  CBIO_DEV_ID devId;
  FAT_INFO *pFat;
  int fatNum;
  u_int32_t srcSecNum, destSecNum;
  u_int8_t buf[8];

  /* Initialize locals */
  volId = pFd->pVolDesc;
  devId = volId->pCbio;
  pFat = (FAT_INFO *) volId->pFatDesc;
  srcSecNum = pFd->pFileHdl->fatSector;

  /* If zero source sector */
  if (srcSecNum == 0)
    return OK;

  /* If sync diabled */
  if (!pFat->syncEnabled)
    return OK;

  /* Copy sector */
  for (fatNum = 1, destSecNum = srcSecNum + volId->secPerFat;
       fatNum < volId->nFats;
       fatNum++, destSecNum += volId->secPerFat) {

    if (cbioBlkCopy(devId, srcSecNum, destSecNum, 1) != OK)
      return ERROR;

  }

  /* If FAT32 */
  if (volId->fatType == FAT32) {

    TO_DISK_32BIT(pFat->fatEntFreeCnt, &buf[0]);
    TO_DISK_32BIT(pFat->groupAllocStart, &buf[4]);

    if (cbioBytesRW(devId, FSINFO_SEC_NUM, FSINFO_FREE_CLUSTS,
		    (char *) buf, sizeof(buf), CBIO_WRITE, NULL) != OK)
      return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * dosFat12EntryRead - Read FAT12 entry
 *
 * RETURNS: Content on entry
 ******************************************************************************/

LOCAL u_int32_t dosFat12EntryRead(DOS_FILE_DESC *pFd,
				  u_int32_t copyNum,
				  u_int32_t cluster)
{
  DOS_VOL_DESC_ID volId;
  CBIO_DEV_ID devId;
  FAT_INFO *pFat;
  block_t secNum;
  off_t secOff;
  cookie_t *pCookie;
  u_int32_t fatEntry, fatOff, nBytes;
  u_int8_t buf[2];

  /* Initialize locals */
  volId = pFd->pVolDesc;
  devId = volId->pCbio;
  pFat = (FAT_INFO *) volId->pFatDesc;

  /* If invalid fat copy */
  assert(copyNum < volId->nFats);

  /* If invalid cluster */
  assert( (cluster >= DOS_MIN_CLUST) &&
	  (cluster < pFat->nFatEnts) );

  /* Get cookie */
  pCookie = &pFd->fatHdl.cbioCookie;

  /* Calculate fat entry */
  fatOff = cluster + (cluster >> 1);
  secNum = pFat->fatStartSec + copyNum * volId->secPerFat +
	   (fatOff >> volId->secSizeShift);
  secOff = fatOff & (volId->bytesPerSec - 1);
  nBytes = volId->bytesPerSec - secOff;
  if (nBytes > 2)
    nBytes = 2;

  /* Read fat entry */
  if (cbioBytesRW(devId, secNum, secOff, (char *) buf, nBytes,
	          CBIO_READ, pCookie) != OK) {

    pFd->fatHdl.errCode = FAT_READ_ERR;
    return FAT_READ_ERR;

  }

  /* If only one byte read */
  if (nBytes == 1) {

    if (cbioBytesRW(devId, secNum + 1, 0, (char *) &buf[1], 1,
		    CBIO_READ, pCookie) != OK) {

      pFd->fatHdl.errCode = FAT_READ_ERR;
      return FAT_READ_ERR;

    }

  }

  /* Extract 12-bit fat entry */
  fatEntry = buf[0] | (buf[1] << 8);
  if (cluster & 0x01)
    fatEntry = (fatEntry >> 4) & 0xfff;
  else
    fatEntry &= 0xfff;

  return fatEntry;
}

/*******************************************************************************
 * dosFat12EntryWrite - Write FAT12 entry
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFat12EntryWrite(DOS_FILE_DESC *pFd,
				u_int32_t copyNum,
				u_int32_t cluster,
				u_int32_t value)
{
  DOS_VOL_DESC_ID volId;
  CBIO_DEV_ID devId;
  FAT_INFO *pFat;
  block_t secNum;
  off_t secOff;
  cookie_t cookie, *pCookie;
  u_int32_t fatOff, nBytes;
  u_int8_t buf[2];

  /* Initialize locals */
  volId = pFd->pVolDesc;
  devId = volId->pCbio;
  pFat = (FAT_INFO *) volId->pFatDesc;

  /* If invalid fat copy */
  assert(copyNum < volId->nFats);

  /* If invalid cluster */
  assert( (cluster >= DOS_MIN_CLUST) &&
	  (cluster < pFat->nFatEnts) );

  /* If invalid value */
  assert( copyNum != volId->pFatDesc->activeCopyNum ||
	  ( (value == pFat->dos_fat_avail) ||
	    ((value > DOS_MIN_CLUST) && (value < pFat->nFatEnts)) ||
	    (value == pFat->dos_fat_eof) ) );

  /* Get cookie */
  cookie = pFd->fatHdl.cbioCookie;
  pCookie = &pFd->fatHdl.cbioCookie;

  /* Calculate fat entry */
  fatOff = cluster + (cluster >> 1);
  secNum = pFat->fatStartSec + copyNum * volId->secPerFat +
	   (fatOff >> volId->secSizeShift);

  /* Mirror last modified fat sector */
  if ( (pFd->pFileHdl->fatSector != secNum) &&
       (copyNum == volId->pFatDesc->activeCopyNum) ) {

    dosFatMirrorSect(pFd);
    pFd->pFileHdl->fatSector = secNum;

  }

  secOff = fatOff & (volId->bytesPerSec - 1);
  nBytes = volId->bytesPerSec - secOff;
  if (nBytes > 2)
    nBytes = 2;

  /* Read old entry */
  if (cbioBytesRW(devId, secNum, secOff, (char *) buf, nBytes,
		  CBIO_READ, &cookie) != OK)
    return ERROR;

  /* If only one byte read */
  if (nBytes == 1) {

    if (cbioBytesRW(devId, secNum + 1, 0, (char *) &buf[1], 1,
		    CBIO_READ, pCookie) != OK)
      return ERROR;

  }

  /* If cluster number is odd */
  if (cluster & 0x01) {

    buf[0] &= 0x0f;
    buf[0] |= (value << 4);
    buf[1] = (value >> 4);

  }

  /* Else cluster number even */
  else {

    buf[0] = value;
    buf[1] &= 0xf0;
    buf[1] |= (value >> 8) & 0x0f;

  }

  /* Write entry */
  if (cbioBytesRW(devId, secNum, secOff, (char *) buf, nBytes,
		  CBIO_WRITE, &cookie) != OK)
    return ERROR;

  /* If only one byte written */
  if (nBytes == 1) {

    if (cbioBytesRW(devId, secNum + 1, 0, (char *) &buf[1], 1,
		    CBIO_WRITE, pCookie) != OK)
      return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * dosFatEmpty - Empty function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFatEmpty(DOS_VOL_DESC_ID volId, ARG arg)
{
  printf("Warning - FAT empty function.\n");

  return OK;
}

