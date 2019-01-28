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

/* cbioLib.c - Cached block I/O */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <rtos.h>
#include <util/classLib.h>
#include <util/objLib.h>
#include <rtos/errnoLib.h>
#include <rtos/semLib.h>
#include <rtos/kernTickLib.h>
#include <rtos/memPartLib.h>
#include <io/ioLib.h>
#include <io/cbioLib.h>
#include <io/private/cbioLibP.h>

/* Defines */
#define cbioBlkShift			cbioPriv0
#define cbioDirtyMask			cbioPriv1
#define cbioBlkNo			cbioPriv2

/* Imports */
IMPORT int sysClockRateGet(void);

/* Locals */
LOCAL BOOL cbioLibInstalled = FALSE;
LOCAL OBJ_CLASS cbioClass;
LOCAL int cbioMutexSemOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);

LOCAL int cbioShiftCalc(unsigned long mask);
LOCAL STATUS cbioWrapStatusOk(BLK_DEV *pBd);
LOCAL STATUS cbioBlkWrapBufferCreate(CBIO_DEV_ID devId);
LOCAL STATUS blkWrapBlkRWbuf(CBIO_DEV_ID devId,
			     block_t startBlock,
			     block_t numBlocks,
			     char *buffer,
			     CBIO_RW rw,
			     cookie_t *pCookie);
LOCAL STATUS blkWrapBlkRW(CBIO_DEV_ID devId,
			  block_t startBlock,
			  block_t numBlocks,
			  char *buffer,
			  CBIO_RW rw,
			  cookie_t *pCookie);
LOCAL STATUS blkWrapBytesRW(CBIO_DEV_ID devId,
			    block_t startBlock,
			    off_t offset,
			    char *buffer,
			    size_t nBytes,
			    CBIO_RW rw,
			    cookie_t *pCookie);
LOCAL STATUS blkWrapBlkCopy(CBIO_DEV_ID devId,
			    block_t srcBlock,
			    block_t destBlock,
			    block_t numBlocks);
LOCAL STATUS blkWrapIoctl(CBIO_DEV_ID devId,
			  int command,
			  ARG arg);

LOCAL CBIO_FUNCS cbioFuncs = {
  (FUNCPTR) blkWrapBlkRW,
  (FUNCPTR) blkWrapBytesRW,
  (FUNCPTR) blkWrapBlkCopy,
  (FUNCPTR) blkWrapIoctl
};

/* Globals */
CLASS_ID cbioClassId = &cbioClass;
CBIO_DEV_ID cbioRecentDev = NULL;

/******************************************************************************
* cbioLibInit - Initialize cached block I/O library
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioLibInit(void)
{
  /* Check if already installed */
  if (cbioLibInstalled == TRUE)
    return OK;

  /* Initalize class */
  if (classInit(cbioClassId, sizeof(CBIO_DEV), OFFSET(CBIO_DEV, objCore),
		memSysPartId,
		(FUNCPTR ) cbioDevCreate,
		(FUNCPTR ) NULL,
		(FUNCPTR ) NULL) != OK) {
    /* errno set by classInit() */
    return ERROR;
  }

  /* Mark as installed */
  cbioLibInstalled = TRUE;

  return OK;
}

/******************************************************************************
* cbioDevCreate - Create device
*
* RETURNS: CBIO_DEV_ID
******************************************************************************/

CBIO_DEV_ID cbioDevCreate(char *ramAddr, size_t ramSize)
{
  CBIO_DEV *pDev;
  char *pBase;
  int alloced;

  /* Initialize locals */
  pDev = NULL;
  pBase = NULL;
  alloced = 0;

  if (!cbioLibInstalled) {
    errnoSet (S_cbioLib_NOT_INSTALLED);
    return NULL;
  }

  /* Allocate ram if needed */
  if (ramSize != 0) {

    if (ramAddr == NULL) {

      pBase = (char *) malloc(ramSize);

      if (pBase == NULL)
        return NULL;

      alloced = 1;

    }

    else {

      pBase = ramAddr;

    }

  }

  /* Allocate memory for devicee */
  pDev = (CBIO_DEV *) objAlloc(cbioClassId);
  if (pDev == NULL) {

    /* errno set by objAlloc() */
    if (alloced)
      free(pBase);

    return NULL;

  }

  /* Create mutex */
  pDev->cbioMutex = semMCreate(cbioMutexSemOptions);
  if (pDev->cbioMutex == NULL) {

    /* errno set by semMCreate() */
    if (alloced)
      free(pBase);

    objFree(cbioClassId, pDev);

    return NULL;

  }

  /* Init fields */
  pDev->readyChanged			= FALSE;
  pDev->cbioMemBase			= pBase;
  pDev->cbioMemSize			= ramSize;

  /* Init params */
  pDev->cbioParams.lastErrBlk		= NONE;
  pDev->cbioParams.lastErrno		= 0;

  /* Init functions */
  pDev->pFuncs				= NULL;

  /* Initalize object */
  objCoreInit(&pDev->objCore, cbioClassId);

  cbioRecentDev = pDev;

  return (CBIO_DEV_ID) pDev;
}

/******************************************************************************
* cbioDevVerify - Verify device
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioDevVerify(CBIO_DEV_ID devId)
{
  if (devId == NULL) {
    errnoSet (S_cbioLib_INVALID_DEV_ID);
    return ERROR;
  }

  if (!cbioLibInstalled) {
    errnoSet (S_cbioLib_NOT_INSTALLED);
    return ERROR;
  }

  if (OBJ_VERIFY(devId, cbioClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return ERROR;
  }

  return OK;
}

/******************************************************************************
* cbioLock - Lock device
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioLock(CBIO_DEV_ID devId, int timeout)
{
  if (cbioDevVerify(devId) != OK) {
    /* errno set by cbioDevVerify() */
    return ERROR;
  }

  return semTake(devId->cbioMutex, timeout);
}

/******************************************************************************
* cbioUnlock - Unlock device
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioUnlock(CBIO_DEV_ID devId)
{
  if (cbioDevVerify(devId) != OK) {
    /* errno set by cbioDevVerify() */
    return ERROR;
  }

  return semGive(devId->cbioMutex);
}

/******************************************************************************
* cbioBlkRW - Read or write blocks
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioBlkRW(CBIO_DEV_ID devId,
	         block_t startBlock,
		 block_t numBlocks,
		 void *buffer,
		 CBIO_RW rw,
		 cookie_t *pCookie)
{
  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* Call strategy function */
  return (* devId->pFuncs->cbioDevBlkRW) (devId,
				          startBlock,
				          numBlocks,
				          buffer,
				          rw,
				          pCookie);
}

/******************************************************************************
* cbioBytesRW - Read or write bytes
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioBytesRW(CBIO_DEV_ID devId,
	           block_t startBlock,
		   off_t offset,
		   void *buffer,
		   size_t nBytes,
		   CBIO_RW rw,
		   cookie_t *pCookie)
{
  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* Call strategy function */
  return (* devId->pFuncs->cbioDevBytesRW) (devId,
					    startBlock,
					    offset,
					    buffer,
					    nBytes,
					    rw,
					    pCookie);
}

/******************************************************************************
* cbioBlkCopy - Copy block
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioBlkCopy(CBIO_DEV_ID devId,
		   block_t src,
		   block_t dest,
		   block_t numBlocks)
{
  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* Call strategy function */
  return (* devId->pFuncs->cbioDevBlkCopy) (devId,
					    src,
					    dest,
					    numBlocks);
}

/******************************************************************************
* cbioIoctl - Ioctl for block device
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioIoctl(CBIO_DEV_ID devId,
		 int command,
		 ARG arg)
{
  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* Call strategy function */
  return (* devId->pFuncs->cbioDevIoctl) (devId,
				          command,
				          arg);
}

/******************************************************************************
* cbioModeSet - Set mode for device
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioModeSet(CBIO_DEV_ID devId, int mode)
{
  BLK_DEV *pBd;

  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* If subdevice set */
  if ( (devId->blkSubDev != NULL) ||
       (devId->cbioSubDev != NULL) ) {

    /* If driver */
    if (devId->isDriver == TRUE) {

      if (devId->blkSubDev == NULL) {

        CBIO_MODE(devId) = mode;
        return OK;

      }

      pBd = devId->blkSubDev;
      pBd->bd_mode = mode;
      CBIO_MODE(devId) = mode;

      return OK;

    } /* End if driver */

    /* Else not driver */
    else {

      return cbioModeSet(devId->cbioSubDev, mode);

    } /* End else not driver */

  } /* End if subdevice set */

  return ERROR;
}

/******************************************************************************
* cbioModeGet - Get mode settings for device
*
* RETURNS: Mode
******************************************************************************/

int cbioModeGet(CBIO_DEV_ID devId)
{
  BLK_DEV *pBd;

  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* If subdevice set */
  if ( (devId->blkSubDev != NULL) ||
       (devId->cbioSubDev != NULL) ) {

    /* If driver */
    if (devId->isDriver == TRUE) {

      if (devId->blkSubDev == NULL)
        return CBIO_MODE(devId);

      pBd = devId->blkSubDev;
      return pBd->bd_mode;

    } /* End if driver */

    /* Else not driver */
    else {

      return cbioModeGet(devId->cbioSubDev);

    } /* End else not driver */

  } /* End if subdevice set */

  return ERROR;
}

/******************************************************************************
* cbioRdyChgdSet - Set ready status for device
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioRdyChgdSet(CBIO_DEV_ID devId, BOOL status)
{
  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* Verify if valid status value */
  if ( (status != TRUE) &&
       (status != FALSE) )
    return ERROR;

  /* Change mode */
  CBIO_READYCHANGED(devId) = status;

  return OK;
}

/******************************************************************************
* cbioRdyChgdGet - Get ready status change for device
*
* RETURNS: TRUE or FALSE
******************************************************************************/

int cbioRdyChgdGet(CBIO_DEV_ID devId)
{
  BLK_DEV *pBd;

  /* Verify object */
  if (cbioDevVerify(devId) != OK)
    return ERROR;

  /* Check ready changed flag */
  if (CBIO_READYCHANGED(devId) == TRUE)
    return TRUE;

  /* If subdevice set */
  if ( (devId->blkSubDev != NULL) ||
       (devId->cbioSubDev != NULL) ) {

    /* If driver */
    if (devId->isDriver == TRUE) {

      if (devId->blkSubDev == NULL)
        return FALSE;

      pBd = devId->blkSubDev;

      return pBd->bd_readyChanged;

    } /* End if driver */

    /* Else not driver */
    else {

      return (cbioRdyChgdGet(devId->cbioSubDev));

   } /* End else not driver */

  } /* End if subdevice set */

  return ERROR;
}

/******************************************************************************
* cbioParamsGet - Get parameters
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS cbioParamsGet(CBIO_DEV_ID devId, CBIO_PARAMS *pCbioParams)
{
  if (cbioDevVerify(devId) != OK) {
    /* errno set by cbioDevVerify() */
    return ERROR;
  }

  *pCbioParams = devId->cbioParams;

  return OK;
}

/******************************************************************************
* cbioWrapBlkDev - Create block i/o wrapper for block device
*
* RETURNS: Block device id
******************************************************************************/

CBIO_DEV_ID cbioWrapBlkDev(BLK_DEV *pBlkDev)
{
  CBIO_DEV *pDev;
  BLK_DEV *pBd;

  /* Initialize locals */
  pDev = NULL;
  pBd = pBlkDev;

  if (pBd == NULL)
    return NULL;

  /* If already a cbio device */
  if (cbioDevVerify((CBIO_DEV_ID) pBd) == OK)
    return (CBIO_DEV_ID) pBd;

  /* Verify block device */
  if ( (pBd->bd_blkRd == NULL) ||
       (pBd->bd_blkWrt == NULL) ||
       (pBd->bd_ioctl == NULL) )
    return NULL;

  /* Calculate shift value */

  /* If no status check installed */
  if (pBd->bd_statusChk == NULL)
    pBd->bd_statusChk = (FUNCPTR) cbioWrapStatusOk;

  /* Create device */
  pDev = cbioDevCreate(NULL, 0);
  if (pDev == NULL)
    return NULL;

  /* Setup struct */
  strcpy(pDev->cbioDesc, "CBIO to BLK_DEV Wrapper");
  pDev->pDc = pBd;
  pDev->cbioParams.cbioRemovable = pBd->bd_removable;
  pDev->readyChanged = pBd->bd_readyChanged;
  pDev->cbioMode = pBd->bd_mode;
  pDev->cbioBlkNo = NONE;
  pDev->cbioDirtyMask = 0;
  pDev->cbioBlkShift = cbioShiftCalc(pBd->bd_bytesPerBlk);
  pDev->pFuncs = &cbioFuncs;

  pDev->blkSubDev = pBd;
  pDev->cbioSubDev = NULL;
  pDev->isDriver = TRUE;

  /* Setup params */
  pDev->cbioParams.nBlocks = pBd->bd_nBlocks;
  pDev->cbioParams.bytesPerBlk = pBd->bd_bytesPerBlk;
  pDev->cbioParams.blocksPerTrack = pBd->bd_blksPerTrack;
  pDev->cbioParams.nHeads = pBd->bd_nHeads;
  pDev->cbioParams.blockOffset = 0;
  pDev->cbioParams.lastErrBlk = NONE;
  pDev->cbioParams.lastErrno = 0;

  return (CBIO_DEV_ID) pDev;
}

/******************************************************************************
* cbioShiftCalc - Calculate number of shift bits
*
* RETURNS: Number of shift bits
******************************************************************************/

LOCAL int cbioShiftCalc(unsigned long mask)
{
  int i;

  /* Assume max 32 bits */
  for (i = 0; i < 32; i++) {

    if (mask & 1)
      break;

    mask = mask >> 1;

  }

  return i;
}

/******************************************************************************
* cbioWrapStatusOk - Function which returns status OK
*
* RETURNS: OK
******************************************************************************/

LOCAL STATUS cbioWrapStatusOk(BLK_DEV *pBd)
{
  return OK;
}

/******************************************************************************
* cbioWrapBufferCreate - Create buffer for blk wrapper
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS cbioBlkWrapBufferCreate(CBIO_DEV_ID devId)
{
  /* Free buffer if needed */
  if ( (devId->cbioMemBase != NULL) &&
       (devId->cbioMemSize != devId->cbioParams.bytesPerBlk) ) {

    free(devId->cbioMemBase);
    devId->cbioMemBase = NULL;
    devId->cbioMemSize = 0;

  }

  /* If buffer null and block size zero */
  if ( (devId->cbioMemBase == NULL) &&
       (devId->cbioMemSize == 0) ) {

    /* Get new size and alloc buffer */
    devId->cbioMemSize = devId->cbioParams.bytesPerBlk;
    devId->cbioMemBase = malloc(devId->cbioMemSize);
    if (devId->cbioMemBase == NULL)
      return ERROR;

    /* Set block r/w function */
    devId->pFuncs->cbioDevBlkRW = (FUNCPTR) blkWrapBlkRWbuf;

    /* Block is empty */
    devId->cbioBlkNo = NONE;
    devId->cbioDirtyMask = 0;

    return OK;

  }

  errnoSet(EINVAL);
  return ERROR;
}

/******************************************************************************
* blkWrapBlkRWbuf - Wrapper block i/o cache coherency
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS blkWrapBlkRWbuf(CBIO_DEV_ID devId,
			     block_t startBlock,
			     block_t numBlocks,
			     char *buffer,
			     CBIO_RW rw,
			     cookie_t *pCookie)
{
  CBIO_DEV *pDev;
  STATUS status;
  block_t cb;

  /* Initialize locals */
  pDev = (CBIO_DEV *) devId;
  status = OK;

  if (cbioLock(pDev, WAIT_FOREVER) != OK)
    return ERROR;

  /* Allocate buffer if needed */
  if (pDev->cbioMemBase == NULL) {

    if (cbioBlkWrapBufferCreate(pDev) != OK) {

      cbioUnlock(pDev);
      return ERROR;

    }

  }

  if (pDev->cbioBlkNo != NONE) {


    if ( (pDev->cbioBlkNo >= startBlock) &&
	 (pDev->cbioBlkNo < startBlock + numBlocks) ) {

      if (pDev->cbioDirtyMask != 0) {

        cb = pDev->cbioBlkNo;
        pDev->cbioBlkNo = NONE;
        pDev->cbioDirtyMask = 0;
        status = blkWrapBlkRW(pDev, cb, 1, pDev->cbioMemBase,
			      CBIO_WRITE, NULL);

      }

      else {

        pDev->cbioBlkNo = NONE;

      }

    }

  }

  if (status != OK) {

    cbioUnlock(pDev);
    return status;

  }

  status = blkWrapBlkRW(devId, startBlock, numBlocks, buffer, rw, pCookie);

  cbioUnlock(pDev);
  return status;
}

/******************************************************************************
* blkWrapBlkRW - Block device wrapper block read/write function
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS blkWrapBlkRW(CBIO_DEV_ID devId,
			  block_t startBlock,
			  block_t numBlocks,
			  char *buffer,
			  CBIO_RW rw,
			  cookie_t *pCookie)
{
  CBIO_DEV *pDev;
  BLK_DEV *pBd;
  STATUS status;
  int count, tick;

  /* Initialize locals */
  pDev = (CBIO_DEV *) devId;
  pBd = pDev->blkSubDev;
  status = ERROR;
  count = 0;

blkWrapBlkRWLoop:

  if ( (pBd->bd_readyChanged == TRUE) ||
       (CBIO_READYCHANGED(devId) == TRUE) ) {

    errnoSet(S_ioLib_DISK_NOT_PRESENT);
    return ERROR;

  }

  if ( (startBlock > pDev->cbioParams.nBlocks) ||
       ((startBlock + numBlocks) > pDev->cbioParams.nBlocks) ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  /* Select read or write */
  switch (rw) {

    case CBIO_READ:
      status = pBd->bd_blkRd(pBd, startBlock, numBlocks, buffer);
    break;

    case CBIO_WRITE:
      status = pBd->bd_blkWrt(pBd, startBlock, numBlocks, buffer);
    break;

    default:
      errnoSet(S_ioLib_UNKNOWN_REQUEST);
      return ERROR;

  }

  /* Check error */
  if (status != OK) {

    pDev->cbioParams.lastErrBlk = startBlock;
    pDev->cbioParams.lastErrno = errnoGet();

    if (errnoGet() == S_ioLib_DISK_NOT_PRESENT)
      cbioRdyChgdSet(devId, TRUE);

    if ( (blkWrapIoctl(pDev, CBIO_STATUS_CHK, 0) == OK) &&
	 (count++ < pBd->bd_retry) ) {

      /* Get time */
      tick = tickGet();

      if ( !((pBd->bd_readyChanged == TRUE) ||
	     (CBIO_READYCHANGED(devId) == TRUE)) ) {

        /* Try to reset device */
        if (pBd->bd_reset != NULL)
          ( *pBd->bd_reset) (pBd);

        /* Sleep, check ready status and retry */
        do {

          if (blkWrapIoctl(pDev, CBIO_STATUS_CHK, 0) == OK)
            goto blkWrapBlkRWLoop;

          taskDelay(5);

        } while ( tickGet() < (unsigned long) tick + sysClockRateGet() );

      }

    }

  }

  return status;

}

/******************************************************************************
* blkWrapBytesRW - Wrapper for block device read/write bytes
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS blkWrapBytesRW(CBIO_DEV_ID devId,
			    block_t startBlock,
			    off_t offset,
			    char *buffer,
			    size_t nBytes,
			    CBIO_RW rw,
			    cookie_t *pCookie)
{
  CBIO_DEV *pDev;
  BLK_DEV *pBd;
  STATUS status;
  char *pMem;
  block_t cb;

  /* Initialize locals */
  pDev = (CBIO_DEV *) devId;
  pBd = pDev->blkSubDev;
  status = OK;

  if ( (pBd->bd_readyChanged == TRUE) ||
       (CBIO_READYCHANGED(devId) == TRUE) ) {

    cbioRdyChgdSet(devId, TRUE);
    errnoSet(S_ioLib_DISK_NOT_PRESENT);
    return ERROR;

  }

  if (startBlock >= pDev->cbioParams.nBlocks) {

    errnoSet(EINVAL);
    return ERROR;

  }

  /* If request if outside block range */
  if ( ((offset + nBytes) > pDev->cbioParams.bytesPerBlk) ||
       (offset < 0) ||
       (nBytes <= 0) ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  cbioLock(pDev, WAIT_FOREVER);

  /* Create buffer if needed */
  if (pDev->cbioMemBase == NULL) {

    if (cbioBlkWrapBufferCreate(pDev) != OK) {

      cbioUnlock(pDev);
      return ERROR;

    }

  }

  /* If flush needed */
  if ( (pDev->cbioBlkNo != NONE) &&
       (pDev->cbioBlkNo != startBlock) &&
       (pDev->cbioDirtyMask != 0) ) {

    cb = pDev->cbioBlkNo;
    pDev->cbioBlkNo = NONE;
    pDev->cbioDirtyMask = 0;
    status = blkWrapBlkRW(pDev, cb, 1, pDev->cbioMemBase,
			  CBIO_WRITE, NULL);
    if (status != OK) {

      cbioUnlock(pDev);
      return status;

    }

  }

  /* If store new block in cache */
  if ( (startBlock != pDev->cbioBlkNo) &&
       (pDev->cbioDirtyMask == 0) ) {

    status = blkWrapBlkRW(pDev, startBlock, 1, pDev->cbioMemBase,
			  CBIO_READ, NULL);
    if (status != OK) {

      cbioUnlock(pDev);
      return status;

    }

    pDev->cbioBlkNo = startBlock;
    pDev->cbioDirtyMask = 0;

  }

  assert(startBlock == pDev->cbioBlkNo);

  /* Find memory address */
  pMem = pDev->cbioMemBase + offset;

  /* Select read or write */
  switch (rw) {

    case CBIO_READ:
      memcpy(buffer, pMem, nBytes);
    break;

    case CBIO_WRITE:
      memcpy(pMem, buffer, nBytes);
      pDev->cbioDirtyMask = 1;
    break;

  }

  cbioUnlock(pDev);
  return OK;
}

/******************************************************************************
* blkWrapBlkCopy - Wrapper for block device copy block
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS blkWrapBlkCopy(CBIO_DEV_ID devId,
			    block_t srcBlock,
			    block_t destBlock,
			    block_t numBlocks)
{
  CBIO_DEV *pDev;
  BLK_DEV *pBd;
  STATUS status;
  block_t cb;

  /* Initialize locals */
  pDev = (CBIO_DEV *) devId;
  pBd = pDev->blkSubDev;
  status = OK;

  if ( (pBd->bd_readyChanged == TRUE) ||
       (CBIO_READYCHANGED(devId) == TRUE) ) {

    cbioRdyChgdSet(devId, TRUE);
    errnoSet(S_ioLib_DISK_NOT_PRESENT);
    return ERROR;

  }

  /* If blocks out of range */
  if ( ((srcBlock) > pDev->cbioParams.nBlocks) ||
       ((destBlock) > pDev->cbioParams.nBlocks) ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  if ( ((srcBlock + numBlocks) > pDev->cbioParams.nBlocks) ||
       ((destBlock + numBlocks) > pDev->cbioParams.nBlocks) ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  if (cbioLock(pDev, WAIT_FOREVER) != OK)
    return ERROR;

  /* Create block buffer if needed */
  if (pDev->cbioMemBase == NULL) {

    if (cbioBlkWrapBufferCreate(pDev) != OK) {

      cbioUnlock(pDev);
      return ERROR;

    }

  }

  /* If cache flush is needed */
  if ( (pDev->cbioBlkNo != NONE) &&
       (pDev->cbioDirtyMask != 0) ) {

    cb = pDev->cbioBlkNo;
    status = blkWrapBlkRW(pDev, cb, 1, pDev->cbioMemBase,
			  CBIO_WRITE, NULL);
    if (status != OK) {

      cbioUnlock(pDev);
      return status;

    }

  }

  pDev->cbioBlkNo = NONE;
  pDev->cbioDirtyMask = 0;

  /* Block copy loop */
  for (; numBlocks > 0; numBlocks--) {

    /* Read block */
    status = blkWrapBlkRW(pDev, srcBlock, 1, pDev->cbioMemBase,
			  CBIO_READ, NULL);
    if (status != OK)
      break;

    /* Write block */
    status = blkWrapBlkRW(pDev, destBlock, 1, pDev->cbioMemBase,
			  CBIO_WRITE, NULL);
    if (status != OK)
      break;

    /* Advance */
    srcBlock++;
    destBlock++;

  }

  cbioUnlock(pDev);
  return status;
}

/******************************************************************************
* blkWrapIoctl - Perform ioctl on block device wrapper
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS blkWrapIoctl(CBIO_DEV_ID devId,
			  int command,
			  ARG arg)
{
  CBIO_DEV *pDev;
  BLK_DEV *pBd;
  STATUS status;
  int i;
  block_t cb;

  /* Initialize locals */
  pDev = (CBIO_DEV *) devId;
  pBd = pDev->blkSubDev;
  status = OK;
  
  if (cbioLock(pDev, WAIT_FOREVER) != OK)
    return ERROR;

  /* If reset with device argument */
  if ( (command == CBIO_RESET) &&
       (arg != NULL) ) {

    pBd = (BLK_DEV *) arg;

    if (cbioDevVerify((CBIO_DEV_ID) pBd) == OK) {

      cbioUnlock(pDev);
      return ERROR;

    }

    /* Verify block device */
    else if ( (pBd->bd_blkRd == NULL) ||
	      (pBd->bd_blkWrt == NULL) ||
              (pBd->bd_ioctl == NULL) ) {

      cbioUnlock(pDev);
      errnoSet(EINVAL);
      return ERROR;

    }

    else {

      i = cbioShiftCalc(pBd->bd_bytesPerBlk);
      if ( pBd->bd_bytesPerBlk != (unsigned long) (1 << i) ) {

        cbioUnlock(pDev);
        errnoSet(EINVAL);
        return ERROR;

      }

      /* If no status given use default */
      if (pBd->bd_statusChk == NULL)
        pBd->bd_statusChk = (FUNCPTR) cbioWrapStatusOk;

    }

    cbioRdyChgdSet(devId, TRUE);

    devId->blkSubDev = pBd;
    devId->pDc = pBd;

    cbioUnlock(pDev);
    return OK;

  }

  /* If reset without device argument */
  if (command == CBIO_RESET) {

    /* Call reset function */
    if (pBd->bd_reset != NULL)
      ( *pBd->bd_reset) (pBd);

    /* If no status given use default */
    if (pBd->bd_statusChk == NULL)
      pBd->bd_statusChk = (FUNCPTR) cbioWrapStatusOk;

    status = (* pBd->bd_statusChk) (pBd);

    if (status != OK)
      cbioRdyChgdSet(devId, TRUE);
    else
      cbioRdyChgdSet(devId, FALSE);

    /* If needed to reinitialize */
    if (CBIO_READYCHANGED(devId) == FALSE) {

      pBd->bd_readyChanged = FALSE;
      pDev->cbioMode = pBd->bd_mode;
      pDev->cbioParams.nBlocks = pBd->bd_nBlocks;
      pDev->cbioParams.bytesPerBlk = pBd->bd_bytesPerBlk;
      pDev->cbioParams.blocksPerTrack = pBd->bd_blksPerTrack;
      pDev->cbioParams.nHeads = pBd->bd_nHeads;
      pDev->cbioParams.blockOffset = 0;

      if ( (pDev->cbioMemBase != NULL) &&
	   (pDev->cbioMemSize != pBd->bd_bytesPerBlk) ) {

        free(pDev->cbioMemBase);
        pDev->cbioMemBase = NULL;
        pDev->cbioMemSize = 0;
        pDev->pFuncs->cbioDevBlkRW = (FUNCPTR) blkWrapBlkRW;

      }

      pDev->cbioParams.lastErrBlk = NONE;
      pDev->cbioParams.lastErrno = 0;

    }

    if (status != OK)
      errnoSet(S_ioLib_DISK_NOT_PRESENT);

    cbioUnlock(pDev);
    return status;

  }

  /* Check if device is ready */
  if ( (pBd->bd_readyChanged == TRUE) ||
       (CBIO_READYCHANGED(devId) == TRUE) ) {

    cbioRdyChgdSet(devId, TRUE);
    errnoSet(S_ioLib_DISK_NOT_PRESENT);
    cbioUnlock(pDev);
    return ERROR;

  }

  /* Select command */
  switch (command) {

    case CBIO_STATUS_CHK:

      status = ( *pBd->bd_statusChk) (pBd);

      if ( (pBd->bd_readyChanged == TRUE) ||
	   (CBIO_READYCHANGED(devId) == TRUE) ) {

        pDev->cbioMode = pBd->bd_mode;
        pDev->cbioParams.nBlocks = pBd->bd_nBlocks;
        pDev->cbioParams.bytesPerBlk = pBd->bd_bytesPerBlk;
        pDev->cbioParams.blocksPerTrack = pBd->bd_blksPerTrack;
        pDev->cbioParams.nHeads = pBd->bd_nHeads;
        pDev->cbioParams.blockOffset = 0;
        pDev->cbioParams.cbioRemovable = pBd->bd_removable;

        if ( (pDev->cbioMemBase != NULL) &&
	     (pDev->cbioMemSize != pBd->bd_bytesPerBlk) ) {

          free(pDev->cbioMemBase);
          pDev->cbioMemBase = NULL;
          pDev->cbioMemSize = 0;
          pDev->pFuncs->cbioDevBlkRW = (FUNCPTR) blkWrapBlkRW;

        }

      }

     break;

     case CBIO_CACHE_FLUSH:
     case CBIO_CACHE_INVAL:
     case CBIO_CACHE_NEWBLK:

       if ( (pDev->cbioMemBase != NULL) &&
	    (pDev->cbioBlkNo != NONE) &&
	    (pDev->cbioDirtyMask != 0) ) {

         cb = pDev->cbioBlkNo;
         status = blkWrapBlkRW(pDev, cb, 1, pDev->cbioMemBase,
			       CBIO_WRITE, NULL);

       }

       /* If new block clear memory */
       if ( (command == CBIO_CACHE_NEWBLK) &&
	    (pDev->cbioMemBase != NULL) ) {

         memset(pDev->cbioMemBase, 0, pDev->cbioMemSize);
         pDev->cbioBlkNo = (block_t) arg;
         pDev->cbioDirtyMask = 1;

       }

       else {

         pDev->cbioBlkNo = NONE;
         pDev->cbioDirtyMask = 0;

       }

     break;

     case CBIO_RESET:
     break;

     case CBIO_DEVICE_LOCK:
     case CBIO_DEVICE_UNLOCK:
     case CBIO_DEVICE_EJECT:

       errnoSet(S_ioLib_UNKNOWN_REQUEST);
       status = ERROR;

     break;

     default:

       status = ( *pBd->bd_ioctl) (pBd, command, arg);

     break;

  }

  cbioUnlock(pDev);
  return status;
}

