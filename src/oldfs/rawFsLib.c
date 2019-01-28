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

/* rawFsLib.c - Raw filesystem */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <rtos.h>
#include <util/listLib.h>
#include <rtos/semLib.h>
#include <io/blkIo.h>
#include <io/cbioLib.h>
#include <oldfs/rawFsLib.h>

/* Defines */

/* Imports */

/* Locals */
BOOL rawFsLibInstalled = FALSE;
int rawFsMaxFiles = 0;
LOCAL SEM_ID rawFsFdListSemId;
LOCAL LIST rawFsFdFreeList;
LOCAL LIST rawFsFdActiveList;

LOCAL STATUS rawFsVolReset(RAW_VOL_DESC *pVol);
LOCAL STATUS rawFVolFlush(RAW_VOL_DESC *pVol);
LOCAL STATUS rawFsVolBlkRW(RAW_VOL_DESC *pVol,
			   block_t blkNum,
			   int numBlks,
			   char *buffer,
			   CBIO_RW rw);
LOCAL STATUS rawFsVolBytesRW(RAW_VOL_DESC *pVol,
			     block_t blkNum,
			     off_t offset,
			     char *buffer,
			     size_t numBytes,
			     CBIO_RW rw,
			     cookie_t *pCookie);
LOCAL RAW_FILE_DESC* rawFsFdGet(void);
LOCAL STATUS rawFsFdSeek(RAW_FILE_DESC *pFd, fsize_t pos);
LOCAL fsize_t rawFsFdWhere(RAW_FILE_DESC *pFd);
LOCAL int rawFsFdRW(RAW_FILE_DESC * pFd,
		    char *buffer,
		    int maxBytes,
		    CBIO_RW rw);
LOCAL STATUS rawFsFdFlush(RAW_FILE_DESC *pFd);
LOCAL void rawFsFdFree(RAW_FILE_DESC *pFd);
LOCAL RAW_FILE_DESC* rawFsOpen(RAW_VOL_DESC *pVol,
			       char *name,
			       int flags);
LOCAL int rawFsRead(RAW_FILE_DESC *pFd,
		    char *buffer,
		    int maxBytes);
LOCAL int rawFsWrite(RAW_FILE_DESC *pFd,
		     char *buffer,
		     int maxBytes);
LOCAL STATUS rawFsClose(RAW_FILE_DESC *pFd);
LOCAL STATUS rawFsIoctl(RAW_FILE_DESC *pFd,
			int cmd,
			ARG arg);

/* Globals */
int rawFsDrvNum = 0;
int rawFsVolMutexOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);
int rawFsFdListMutextOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);
int rawFsFdMutexOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);

/* Functions */

/*******************************************************************************
 * rawFsLibInit - Initialize raw filesystem library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS rawFsLibInit(int maxFiles)
{
  RAW_FILE_DESC *pFd;
  int i;

  /* Check if installed /*
  if (rawFsLibInstalled)
    return OK;

  /* Initialize lists */
  listInit(&rawFsFdFreeList);
  listInit(&rawFsFdActiveList);

  /* Check max files */
  if (maxFiles <= 0)
    maxFiles = RAWFS_DEFAULT_MAX_FILES;

  /* Create semaphore for filedescriptor list */
  rawFsFdListSemId = semMCreate(rawFsFdListMutextOptions);
  if (rawFsFdListSemId == NULL)
    return ERROR;

  /* Lock list */
  semTake(rawFsFdListSemId, WAIT_FOREVER);

  /* Allocate list */
  pFd = (RAW_FILE_DESC *) malloc( sizeof(RAW_FILE_DESC) * maxFiles );
  if (pFd == NULL) {

    semGive(rawFsFdListSemId);
    return ERROR;

  }

  /* Clear */
  memset(pFd, 0, sizeof(RAW_FILE_DESC) * maxFiles);

  /* Create list of file descriptors */
  for (i = 0; i < maxFiles; i++) {

    pFd->rawFdStatus = RAWFD_AVAIL;

    /* Create semaphore */
    pFd->rawFdSemId = semMCreate(rawFsFdMutexOptions);
    if (pFd->rawFdSemId == NULL)
      return NULL;

    /* Add to list */
    listAdd(&rawFsFdFreeList, &pFd->rawFdNode);

    /* Advance */
    pFd++;

  }

  /* Unlock */
  semGive(rawFsFdListSemId);

  /* Set global max files */
  rawFsMaxFiles = maxFiles;

  /* Install driver */
  rawFsDrvNum = iosDrvInstall((FUNCPTR) rawFsOpen,	/* Create */
			      (FUNCPTR) NULL,		/* Delete */
			      (FUNCPTR) rawFsOpen,	/* Open */
			      (FUNCPTR) rawFsClose,	/* Close */
			      (FUNCPTR) rawFsRead,	/* Read */
			      (FUNCPTR) rawFsWrite,	/* Write */
			      (FUNCPTR) rawFsIoctl);	/* Ioctl */
  if(rawFsDrvNum == ERROR)
    return ERROR;

  /* Mark as installed */
  rawFsLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * rawFsLibInit - Initialize raw filesystem library
 *
 * RETURNS: Pointer to volume descriptor
 ******************************************************************************/

RAW_VOL_DESC* rawFsDevInit(char *name, BLK_DEV *pDev)
{
  RAW_VOL_DESC *pVol;
  CBIO_DEV_ID devId;

  /* If device needs to be initialized */
  if (cbioDevVerify((CBIO_DEV_ID) pDev) != OK)
    devId = cbioWrapBlkDev(pDev);
  else
    devId = (CBIO_DEV_ID) pDev;

  if (devId == NULL)
    return NULL;

  /* Allocate memory for volume descriptor */
  pVol = (RAW_VOL_DESC *) malloc( sizeof(RAW_VOL_DESC) );
  if (pVol == NULL)
    return NULL;

  /* Clear memory */
  memset(pVol, 0, sizeof(RAW_VOL_DESC));

  /* Add device */
  if (iosDevAdd((DEV_HEADER *) pVol, name, rawFsDrvNum) != OK) {

    free(pVol);
    return NULL;

  }

  /* Setup struct */
  pVol->rawVdCbio = devId;
  pVol->rawVdStatus = ERROR;
  pVol->rawVdState = RAW_VD_READY_CHANGED;

  /* Create semaphore */
  pVol->rawVdSemId = semMCreate(rawFsVolMutexOptions);
  if (pVol->rawVdSemId == NULL) {

    free(pVol);
    return NULL;

  }

  return pVol;
}

/*******************************************************************************
 * rawFsVolReset - Reset volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsVolReset(RAW_VOL_DESC *pVol)
{
  if (cbioIoctl(pVol->rawVdCbio, CBIO_RESET, NULL) != OK) {

    pVol->rawVdState = RAW_VD_CANT_RESET;
    return ERROR;

  }

  pVol->rawVdState = RAW_VD_RESET;

  return OK;
}

/*******************************************************************************
 * rawFsVolFlush - Flush volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsVolFlush(RAW_VOL_DESC *pVol)
{
  RAW_FILE_DESC *pFd;

  if (pVol->rawVdState != RAW_VD_MOUNTED)
    return ERROR;

  /* Lock fd list */
  semTake(rawFsFdListSemId, WAIT_FOREVER);

  /* For all active file descriptors */
  for (pFd = (RAW_FILE_DESC *) LIST_HEAD(&rawFsFdActiveList);
       pFd != NULL;
       pFd = (RAW_FILE_DESC *) LIST_NEXT(&pFd->rawFdNode) ) {

    /* Flush file descriptor */
    if (rawFsFdFlush(pFd) != OK)
      return ERROR;

  }

  /* Unlock fd list */
  semGive(rawFsFdListSemId);

  return OK;
}

/*******************************************************************************
 * rawFsVolBlkRW - Read/write blocks from volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsVolBlkRW(RAW_VOL_DESC *pVol,
			   block_t blkNum,
			   int numBlks,
			   char *buffer,
			   CBIO_RW rw)
{
  return cbioBlkRW(pVol->rawVdCbio, blkNum, numBlks, buffer, rw, NULL);
}

/*******************************************************************************
 * rawFsVolBytesRW - Read/write bytes from volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsVolBytesRW(RAW_VOL_DESC *pVol,
			     block_t blkNum,
			     off_t offset,
			     char *buffer,
			     size_t numBytes,
			     CBIO_RW rw,
			     cookie_t *pCookie)
{
  return cbioBytesRW(pVol->rawVdCbio, blkNum, offset, buffer,
		     numBytes, rw, pCookie);
}

/*******************************************************************************
 * rawFsFdGet - Get filedescriptor
 *
 * RETURNS: Pointer to filedescriptor
 ******************************************************************************/

LOCAL RAW_FILE_DESC* rawFsFdGet(void)
{
  RAW_FILE_DESC *pFd;

  /* Lock fd list */
  semTake(rawFsFdListSemId, WAIT_FOREVER);

  /* Get fd from free list */
  pFd = (RAW_FILE_DESC *) listGet(&rawFsFdFreeList);
  if (pFd == NULL)
    return NULL;

  /* Set as used */
  pFd->rawFdStatus = RAWFD_USED;
  listAdd(&rawFsFdActiveList, &pFd->rawFdNode);

  /* Unlock fd list */
  semGive(rawFsFdListSemId);

  return pFd;
}

/*******************************************************************************
 * rawFsFdSeek - Goto position in file
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsFdSeek(RAW_FILE_DESC *pFd, fsize_t pos)
{
  if (pos < 0)
    return ERROR;

  /* Update pointer */
  pFd->rawFdNewPtr = pos;

  return OK;
}

/*******************************************************************************
 * rawFsFdWhere - Get current position in file
 *
 * RETURNS: Position in file
 ******************************************************************************/

LOCAL fsize_t rawFsFdWhere(RAW_FILE_DESC *pFd)
{
  return pFd->rawFdNewPtr;
}

/*******************************************************************************
 * rawFsFdRW - Read/write operations for raw filesystem
 *
 * RETURNS: Bytes read/written
 ******************************************************************************/

LOCAL int rawFsFdRW(RAW_FILE_DESC * pFd,
		    char *buffer,
		    int maxBytes,
		    CBIO_RW rw)
{
  RAW_VOL_DESC *pVol;
  CBIO_DEV_ID devId;
  CBIO_PARAMS params;
  int offs, bytesLeft, nBytes;
  block_t blkNum, numBlks;

  /* Initialize locals */
  pVol = pFd->pRawFdVd;
  devId = pFd->pRawFdVd->rawVdCbio;
  if (cbioParamsGet(devId, &params) != OK)
    return ERROR;

  /* Lock fd */
  semTake(pFd->rawFdSemId, WAIT_FOREVER);

  /* Check obsolete */
  if (pFd->rawFdStatus == RAWFD_OBSOLETE) {

    semGive(pFd->rawFdSemId);
    return ERROR;

  }

  /* Check access */
  if ( (rw == CBIO_WRITE) &&
       (pFd->rawFdMode == O_RDONLY) ) {

    semGive(pFd->rawFdSemId);
    return ERROR;

  }

  /* Check size */
  if (maxBytes <= 0) {

    semGive(pFd->rawFdSemId);
    return ERROR;

  }

  /* Lock volume */
  semTake(pVol->rawVdSemId, WAIT_FOREVER);

  /* For blocks read/write */
  for (bytesLeft = maxBytes,
       blkNum = pFd->rawFdNewPtr / params.bytesPerBlk;
       (bytesLeft > 0) && (blkNum < params.nBlocks);
       bytesLeft -= nBytes, buffer += nBytes,
       pFd->rawFdNewPtr += nBytes, blkNum += numBlks) {

    /* Reset counter */
    nBytes = 0;

    /* If block wise read/write possible */
    if ( ((unsigned long) bytesLeft >= params.bytesPerBlk) &&
	 (pFd->rawFdNewPtr % params.bytesPerBlk == 0) ) {

      numBlks = bytesLeft / params.bytesPerBlk;
      nBytes = numBlks * params.bytesPerBlk;

      /* Adjust end */
      numBlks = min(numBlks, params.nBlocks - blkNum);

      /* Do block read/write */
      if (rawFsVolBlkRW(pVol, blkNum, numBlks, buffer, rw) != OK) {

        /* If no bytes processed */
        if (bytesLeft == maxBytes) {

          semGive(pVol->rawVdSemId);
          semGive(pFd->rawFdSemId);
          return ERROR;

        }

        else {

          break;

        }

      }

      pFd->rawFdCookie = (cookie_t) NULL;
      continue;

    } /* End if block wise read/write possible */

    /* Else block wise read/write not possible */
    else {

      numBlks = 1;
      offs = pFd->rawFdNewPtr % params.bytesPerBlk;
      nBytes = min(bytesLeft, params.bytesPerBlk - offs);

      /* Read/write bytes */
      if (rawFsVolBytesRW(pVol, blkNum, offs, buffer,
			  nBytes, rw, &pFd->rawFdCookie) != OK) {

        /* If no bytes processed */
        if (bytesLeft == maxBytes) {

          semGive(pVol->rawVdSemId);
          semGive(pFd->rawFdSemId);
          return ERROR;

        }

        else {

          break;

        }

      }

    } /* End else block wise read/write not possible */

  } /* End for bytes read/write */

  /* Unlock volume */
  semGive(pVol->rawVdSemId);

  /* Unlock fd */
  semGive(pFd->rawFdSemId);

  return (maxBytes - bytesLeft);
}

/*******************************************************************************
 * rawFsFdFlush - Flush filedescriptor
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL STATUS rawFsFdFlush(RAW_FILE_DESC *pFd)
{
  return cbioIoctl(pFd->pRawFdVd->rawVdCbio, CBIO_CACHE_FLUSH, (ARG) -1);
}

/*******************************************************************************
 * rawFsFdFree - Release filedescriptor
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void rawFsFdFree(RAW_FILE_DESC *pFd)
{
  /* Lock fd list */
  semTake(rawFsFdListSemId, WAIT_FOREVER);

  /* Update status */
  pFd->rawFdStatus = RAWFD_AVAIL;

  /* Remove from active list */
  listRemove(&rawFsFdActiveList, &pFd->rawFdNode);

  /* Put back on free list */
  listAdd(&rawFsFdFreeList, &pFd->rawFdNode);

  /* Unlock fd list */
  semGive(rawFsFdListSemId);
}

/*******************************************************************************
 * rawFsVolMount - Mount volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS rawFsVolMount(RAW_VOL_DESC *pVol)
{
  /* Lock volume */
  semTake(pVol->rawVdSemId, WAIT_FOREVER);

  /* If aleady mounted */
  if (pVol->rawVdStatus == OK)
    rawFsVolUmount(pVol);

  /* Reset volume */
  if (rawFsVolReset(pVol) == OK) {

    pVol->rawVdStatus = OK;
    pVol->rawVdState = RAW_VD_MOUNTED;

  }

  /* Unlock volume */
  semGive(pVol->rawVdSemId);

  return pVol->rawVdStatus;
}

/*******************************************************************************
 * rawFsVolVerify - Verify raw volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS rawFsVolVerify(RAW_VOL_DESC *pVol)
{
  STATUS status;

  /* Initialize locals */
  status = OK;

  /* Check ready change */
  if ( (cbioIoctl(pVol->rawVdCbio, CBIO_STATUS_CHK, NULL) != OK) ||
       (cbioRdyChgdGet(pVol->rawVdCbio) == TRUE) )
    pVol->rawVdState = RAW_VD_READY_CHANGED;

  /* Check status */
  switch (pVol->rawVdState) {

    case RAW_VD_MOUNTED:
    break;

    case RAW_VD_CANT_RESET:
    break;

    case RAW_VD_CANT_MOUNT:
    break;

    case RAW_VD_RESET:
    break;

    case RAW_VD_READY_CHANGED:

      /* Re-mount */
      if (rawFsVolMount(pVol) != OK) {

        pVol->rawVdState = RAW_VD_CANT_MOUNT;
        status = ERROR;
        break;

      }

      pVol->rawVdState = RAW_VD_MOUNTED;

    break;
  }

  return status;
}

/*******************************************************************************
 * rawFsReadyChange - Change ready state
 *
 * RETURNS: N/A
 ******************************************************************************/

void rawFsReadyChange(RAW_VOL_DESC *pVol)
{
  pVol->rawVdState = RAW_VD_READY_CHANGED;
}

/*******************************************************************************
 * rawFsVolUmount - Unmount volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS rawFsVolUmount(RAW_VOL_DESC *pVol)
{
  RAW_FILE_DESC *pFd;

  /* Lock volume */
  semTake(pVol->rawVdSemId, WAIT_FOREVER);

  /* If invalid status */
  if (pVol->rawVdStatus) {

    semGive(pVol->rawVdSemId);
    return OK;

  }

  /* Flush volume */
  rawFsVolFlush(pVol);

  /* Lock fd list */
  semTake(rawFsFdListSemId, WAIT_FOREVER);

  /* For all active file descriptors */
  for (pFd = (RAW_FILE_DESC *) LIST_HEAD(&rawFsFdActiveList);
       pFd != NULL;
       pFd = (RAW_FILE_DESC *) LIST_NEXT(&pFd->rawFdNode) ) {

    /* Obsolete file descriptor */
    pFd->rawFdStatus = RAWFD_OBSOLETE;

  }

  /* Unlock fd list */
  semGive(rawFsFdListSemId);

  /* Mark as unmounted */
  rawFsReadyChange(pVol);
  pVol->rawVdStatus = ERROR;

  /* Unlock volume */
  semGive(pVol->rawVdSemId);

  return OK;
}

/*******************************************************************************
 * rawFsOpen - Open raw filesystem volume
 *
 * RETURNS: Pointer to filedescriptor
 ******************************************************************************/

LOCAL RAW_FILE_DESC* rawFsOpen(RAW_VOL_DESC *pVol,
			       char *name,
			       int flags)
{
  RAW_FILE_DESC *pFd;
  CBIO_DEV_ID devId;
  CBIO_PARAMS params;

  /* Initialize locals */
  devId = pVol->rawVdCbio;
  if (cbioParamsGet(devId, &params) != OK)
    return (RAW_FILE_DESC *) ERROR;

  /* Name must be empty */
  if (name[0] != EOS)
    return (RAW_FILE_DESC *) ERROR;

  /* Fix flags */
  flags &= ~(O_CREAT | O_TRUNC);

  /* Get a file descriptor */
  pFd = rawFsFdGet();
  if (pFd == NULL)
    return (RAW_FILE_DESC *) ERROR;

  /* Lock file descriptor */
  semTake(pFd->rawFdSemId, WAIT_FOREVER);

  /* Lock volume */
  semTake(pVol->rawVdSemId, WAIT_FOREVER);

  /* Verify volume */
  if (rawFsVolVerify(pVol) != OK) {

    semGive(pVol->rawVdSemId);
    rawFsFdFree(pFd);
    semGive(pFd->rawFdSemId);
    return (RAW_FILE_DESC *) ERROR;

  }

  /* Check access */
  if ( (cbioModeGet(pVol->rawVdCbio) == O_RDONLY) &&
       (flags == O_WRONLY || O_RDWR) ) {

    semGive(pVol->rawVdSemId);
    rawFsFdFree(pFd);
    semGive(pFd->rawFdSemId);
    return (RAW_FILE_DESC *) ERROR;

  }

  /* Unlock volume */
  semGive(pVol->rawVdSemId);

  /* Setup fd struct */
  pFd->rawFdMode = flags;
  pFd->pRawFdVd = pVol;
  pFd->rawFdNewPtr = NULL;
  pFd->rawFdEndPtr = (fsize_t) ( (fsize_t) params.nBlocks *
				 (fsize_t) params.bytesPerBlk );

  /* Unlock fd */
  semGive(pFd->rawFdSemId);

  return pFd;
}

/*******************************************************************************
 * rawFsRead - Read from raw filesystem
 *
 * RETURNS: Bytes read
 ******************************************************************************/

LOCAL int rawFsRead(RAW_FILE_DESC *pFd,
		    char *buffer,
		    int maxBytes)
{
  return rawFsFdRW(pFd, buffer, maxBytes, CBIO_READ);
}

/*******************************************************************************
 * rawFsWrite - Write to raw filesystem
 *
 * RETURNS: Bytes written
 ******************************************************************************/

LOCAL int rawFsWrite(RAW_FILE_DESC *pFd,
		     char *buffer,
		     int maxBytes)
{
  return rawFsFdRW(pFd, buffer, maxBytes, CBIO_WRITE);
}

/*******************************************************************************
 * rawFsClose - Close raw filesystem volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsClose(RAW_FILE_DESC *pFd)
{
  RAW_VOL_DESC *pVol;
  STATUS status;

  /* Initialize locals */
  pVol = pFd->pRawFdVd;

  /* Lock fd */
  semTake(pFd->rawFdSemId, WAIT_FOREVER);

  /* Check fd */
  if (pFd->rawFdStatus == RAWFD_OBSOLETE) {

    rawFsFdFree(pFd);
    semGive(pFd->rawFdSemId);
    return ERROR;

  }

  /* Lock volume */
  semTake(pVol->rawVdSemId, WAIT_FOREVER);

  /* Flush */
  status = rawFsFdFlush(pFd);

  /* Unlock volume */
  semGive(pVol->rawVdSemId);

  /* Free fd */
  rawFsFdFree(pFd);

  /* Unlock fd */
  semGive(pFd->rawFdSemId);

  return status;
}

/*******************************************************************************
 * rawFsIoctl - Perform ioctl on raw filesystem
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS rawFsIoctl(RAW_FILE_DESC *pFd,
			int cmd,
			ARG arg)
{
  STATUS status;
  fsize_t pos;

  /* Initialize locals */
  status = OK;
  pos = 0;

  /* Lock fd */
  semTake(pFd->rawFdSemId, WAIT_FOREVER);

  /* Check fd */
  if (pFd->rawFdStatus == RAWFD_OBSOLETE) {

    semGive(pFd->rawFdSemId);
    return ERROR;

  }

  /* Select command */
  switch (cmd) {

    case FIODISKINIT:
    break;

    case FIOSEEK:
      pos = (fsize_t) arg;
      status = rawFsFdSeek(pFd, pos);
    break;

    case FIOWHERE:
      pos = rawFsFdWhere(pFd);
      *(fsize_t *) arg = pos;
    break;

    case FIOFLUSH:
    case FIOSYNC:
      status = rawFsVolFlush(pFd->pRawFdVd);
    break;

    case FIONREAD:
      pos = pFd->rawFdEndPtr - (rawFsFdWhere(pFd));
      if (arg == (ARG) 0)
        return ERROR;

      *(fsize_t *) arg = pos;
    break;

    case FIODISKCHANGE:
      rawFsReadyChange(pFd->pRawFdVd);
    break;

    default:
      status = cbioIoctl(pFd->pRawFdVd->rawVdCbio, cmd, arg);
    break;

  }

  /* Unlock fd */
  semGive(pFd->rawFdSemId);

  return status;
}

