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

/* dosFsLib.c - Dos filesystem library */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <rtos/semLib.h>
#include <os/logLib.h>
#include <io/iosLib.h>
#include <io/blkIo.h>
#include <io/cbioLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/private/dosSuperLibP.h>
#include <oldfs/private/dosDirLibP.h>
#include <oldfs/private/dosFatLibP.h>
#include <oldfs/private/dosFmtLibP.h>
#include <string.h>

/* Defines */

/* Imports */

/* Locals */
LOCAL BOOL dosFsLibInstalled = FALSE;
LOCAL int dosFsDrvNum;

LOCAL STATUS dosFsVolMount(DOS_VOL_DESC_ID volId);

LOCAL DOS_FILE_DESC* dosFsFdGet(DOS_VOL_DESC_ID volId);
LOCAL STATUS dosFsFdSemTake(DOS_FILE_DESC *pFd, int timeout);
LOCAL STATUS dosFsFdSemGive(DOS_FILE_DESC *pFd);
LOCAL void dosFsFdFree(DOS_FILE_DESC *pFd);
LOCAL void dosFsHdlDeref(DOS_FILE_DESC *pFd);

LOCAL STATUS dosFsTrunc(DOS_FILE_DESC *pFd, fsize_t len);
LOCAL STATUS dosFsSeek(DOS_FILE_DESC *pFd, fsize_t pos);
LOCAL STATUS dosFsSeekDir(DOS_FILE_DESC *pFd, struct dirent *pDir);
LOCAL STATUS dosFsFileExpand(DOS_FILE_DESC *pFd,
			     unsigned int fatFlags);
LOCAL int dosFsFileRW(DOS_FILE_DESC *pFd,
		      char *pBuf,
		      int maxBytes,
		      unsigned int op);
LOCAL STATUS dosFsStatGet(DOS_FILE_DESC *pFd,
			  struct stat *pStat);

LOCAL DOS_FILE_DESC* dosFsCreate(DOS_VOL_DESC_ID volId,
				 char *name,
				 int flags);
LOCAL STATUS dosFsDelete(DOS_VOL_DESC_ID volId,
			 char *name);
LOCAL DOS_FILE_DESC* dosFsOpen(DOS_VOL_DESC_ID volId,
				 char *name,
				 int flags,
				 int mode);
LOCAL STATUS dosFsClose(DOS_FILE_DESC *pFd);
LOCAL int dosFsRead(DOS_FILE_DESC *pFd,
		    char *pBuf,
		    int maxBytes);
LOCAL int dosFsWrite(DOS_FILE_DESC *pFd,
		     char *pBuf,
		     int maxBytes);
LOCAL STATUS dosFsIoctl(DOS_FILE_DESC *pFd,
		        int cmd,
			ARG arg);
/* Globals */
int dosFsDevSemOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);
int dosFsShortSemOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);
int dosFsFileSemOptions = (SEM_Q_PRIORITY | SEM_DELETE_SAFE);

DOS_HDLR_DESC dosDirHdlrsList[DOS_MAX_HDLRS];
DOS_HDLR_DESC dosFatHdlrsList[DOS_MAX_HDLRS];

FUNCPTR dosFsVolFormatFunc = NULL;;

/* Functions */

/*******************************************************************************
 * dosFsLibInit - Initialize dos filesystem library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosFsLibInit(void)
{
  /* Check if installed */
  if (dosFsLibInstalled)
    return OK;

  /* Clear hander lists */
  memset(dosDirHdlrsList, 0, DOS_MAX_HDLRS * sizeof(DOS_HDLR_DESC));
  memset(dosFatHdlrsList, 0, DOS_MAX_HDLRS * sizeof(DOS_HDLR_DESC));

  if (dosSuperLibInit() != OK)
    return ERROR;

  if (dosDirLibInit() != OK)
    return ERROR;

  if (dosFatLibInit() != OK)
    return ERROR;

  dosFmtLibInit();

  /* Install driver */
  dosFsDrvNum = iosDrvInstall((FUNCPTR) dosFsCreate,	/* Create */
			      (FUNCPTR) dosFsDelete,	/* Delete */
			      (FUNCPTR) dosFsOpen,	/* Open */
			      (FUNCPTR) dosFsClose,	/* Close */
			      (FUNCPTR) dosFsRead,	/* Read */
			      (FUNCPTR) dosFsWrite,	/* Write */
			      (FUNCPTR) dosFsIoctl);	/* Ioctl */
  if (dosFsDrvNum == ERROR)
    return ERROR;

  /* Mark as installed */
  dosFsLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * dosFsHdlrInstall - Install handlers
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosFsHdlrInstall(DOS_HDLR_DESC *hdlrList, DOS_HDLR_DESC *hdlr)
{
  DOS_HDLR_DESC hdlr1, hdlr2;
  int i;
  STATUS status;

  /* Initialize locals */
  status = ERROR;

  if (hdlr == NULL)
    return ERROR;

  hdlr1 = *hdlr;

  /* Install handlers */
  for (i = 0; i < DOS_MAX_HDLRS; i++) {

    if ( (hdlrList[i].id == 0) ||
	 (hdlrList[i].id > hdlr1.id) ) {

      status = OK;
      hdlr2 = hdlrList[i];
      hdlrList[i] = hdlr1;
      hdlr1 = hdlr2;

    }

  }

  return status;
}

/*******************************************************************************
 * dosFsDevCreate - Create dos device
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

DOS_VOL_DESC_ID dosFsDevInit(char *name,
		             BLK_DEV *pDev,
		             int maxFiles,
		             int autoCheck)
{
  DOS_VOL_DESC_ID volId;
  SEM_ID semId;
  CBIO_DEV_ID devId;
  int i;

  /* Check max files range */
  if (maxFiles <= 0)
    maxFiles = DOS_NFILES_DEFAULT;
  maxFiles += 2;

  if (cbioDevVerify((CBIO_DEV_ID) pDev) != OK)
    devId = cbioWrapBlkDev(pDev);
  else
    devId = (CBIO_DEV_ID) pDev;

  if (devId == NULL)
    return NULL;

  /* Allocate memory for volume descriptor */
  volId = (DOS_VOL_DESC_ID) malloc( sizeof(DOS_VOL_DESC) );
  if (volId == NULL)
    return NULL;

  /* Clear */
  memset(volId, 0, sizeof(DOS_VOL_DESC));

  /* Setup struct */
  volId->magic = DOS_FS_MAGIC;
  volId->mounted = FALSE;
  volId->updateLastAccessDate = FALSE;
  volId->volIsCaseSens = FALSE;
  volId->pCbio = devId;

  /* Initailize semaphores */
  volId->devSem = semMCreate(dosFsDevSemOptions);
  if (volId->devSem == NULL) {

    free(volId);
    return NULL;

  }

  /* Initailize semaphores */
  volId->shortSem = semMCreate(dosFsShortSemOptions);
  if (volId->shortSem == NULL) {

    semDelete(volId->devSem);
    free(volId);
    return NULL;

  }

  /* Setup struct */
  volId->maxFiles = maxFiles;
  volId->nBusyFd = 0;

  /* Allocate file descriptor list */
  volId->pFdList = (DOS_FILE_DESC *) malloc( sizeof(DOS_FILE_DESC) *
					     maxFiles );
  if (volId->pFdList == NULL) {

    semDelete(volId->devSem);
    semDelete(volId->shortSem);
    free(volId);
    return NULL;

  }

  /* Clear */
  memset(volId->pFdList, 0, sizeof(DOS_FILE_DESC) * maxFiles);

  /* Allocate file handle list */
  volId->pFhdlList = (DOS_FILE_HDL *) malloc( sizeof(DOS_FILE_HDL) *
					      maxFiles );
  if (volId->pFhdlList == NULL) {

    free(volId->pFdList);
    semDelete(volId->devSem);
    semDelete(volId->shortSem);
    free(volId);
    return NULL;

  }

  /* Clear */
  memset(volId->pFhdlList, 0, sizeof(DOS_FILE_HDL) * maxFiles);

  /* Allocate file semaphores list */
  volId->pFsemList = (SEM_ID *) malloc( sizeof(SEM_ID *) *
				        maxFiles );
  if (volId->pFsemList == NULL) {

    free(volId->pFhdlList);
    free(volId->pFdList);
    semDelete(volId->devSem);
    semDelete(volId->shortSem);
    free(volId);
    return NULL;

  }

  /* Clear */
  memset(volId->pFsemList, 0, sizeof(SEM_ID *) * maxFiles);

  /* For all files */
  for (i = 0; i < maxFiles; i++) {

    semId = semMCreate(dosFsFileSemOptions);
    if (volId->pFsemList == NULL) {

      free(volId->pFsemList);
      free(volId->pFhdlList);
      free(volId->pFdList);
      semDelete(volId->devSem);
      semDelete(volId->shortSem);
      free(volId);
      return NULL;

    }

    volId->pFsemList[i] = semId;

  }

  /* Add device */
  if (iosDevAdd((DEV_HEADER *) volId, name, dosFsDrvNum) != OK) {

    free(volId->pFsemList);
    free(volId->pFhdlList);
    free(volId->pFdList);
    semDelete(volId->devSem);
    semDelete(volId->shortSem);
    free(volId);
    return NULL;

  }

  return volId;
}

/*******************************************************************************
 * dosFsVolDescGet - Get volume descriptor from device name
 *
 * RETURNS: Volume descriptor id
 ******************************************************************************/

DOS_VOL_DESC_ID dosFsVolDescGet(void *idOrName, char **ppTail)
{
  DOS_VOL_DESC_ID volId;
  char *devName, *nameTail;

  if (idOrName == NULL)
    devName = "";
  else
    devName = idOrName;

  if (ppTail == NULL)
    ppTail = &nameTail;

  **ppTail = NULL;

  volId = idOrName;
  if (volId == NULL)
    return NULL;

  /* Check magic number */
  if (volId->magic == DOS_FS_MAGIC)
    return volId;

  /* Get device from name */
  volId = (DOS_VOL_DESC_ID) iosDevFind(devName, ppTail);
  if ( (volId == NULL) ||
       (*ppTail == devName) )
    return NULL;

  /* Check magic number */
  if (volId->magic == DOS_FS_MAGIC)
    return volId;

  return NULL;
}

/*******************************************************************************
 * dosFsVolLabelGet - Get volume label
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosFsVolLabelGet(DOS_VOL_DESC_ID volId, char *name)
{
  DOS_FILE_DESC *pFd;
  STATUS status;

  if ( (volId == NULL) ||
       (volId->magic != DOS_FS_MAGIC) )
    return ERROR;

  /* Mount volume */
  if ( (volId->mounted == FALSE) ||
       (cbioIoctl(volId->pCbio, CBIO_STATUS_CHK, 0) == ERROR) ||
       (cbioRdyChgdGet(volId->pCbio) == TRUE) ) {

    if (dosFsVolMount(volId) != OK)
      return ERROR;

  }

  pFd = dosFsFdGet(volId);
  if (pFd == NULL)
    return ERROR;

  /* Call ioctl to get label */
  status = dosFsIoctl(pFd, FIOLABELGET, (ARG) name);

  dosFsFdFree(pFd);

  return status;
}

/*******************************************************************************
 * dosFsVolMount - Mount volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsVolMount(DOS_VOL_DESC_ID volId)
{
  DOS_FILE_DESC *pFd;
  int i;

  if ( (volId == NULL) ||
       (volId->magic != DOS_FS_MAGIC) )
    return ERROR;

  semTake(volId->devSem, WAIT_FOREVER);

  /* If mounted */
  if (volId->mounted) {

    if (cbioRdyChgdGet(volId->pCbio) == FALSE) {

      semGive(volId->devSem);
      return ERROR;

    }

    dosFsVolUnmount(volId);

  }

  /* Reset device */
  if (cbioIoctl(volId->pCbio, CBIO_RESET, NULL) != OK) {

    semGive(volId->devSem);
    return ERROR;

  }

  /* Read boot sector */
  dosSuperGet(volId);

  /* Install directory handlers */
  for (i = 0; i < NELEMENTS(dosDirHdlrsList); i++) {

    if (dosDirHdlrsList[i].mountFunc != NULL)  {

      if ( (( *dosDirHdlrsList[i].mountFunc)
		(volId, dosDirHdlrsList[i].arg) == OK) )
        break;

    }

  }

  /* Install fat handlers */
  for (i = 0; i < NELEMENTS(dosFatHdlrsList); i++) {

    if (dosFatHdlrsList[i].mountFunc != NULL)  {

      if ( (( *dosFatHdlrsList[i].mountFunc)
		(volId, dosFatHdlrsList[i].arg) == OK) )
        break;

    }

  }

  /* Mark as mounted */
  volId->mounted = TRUE;

  semGive(volId->devSem);

  return OK;
}

/*******************************************************************************
 * dosFsVolUnmount - Unmount volume
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosFsVolUnmount(DOS_VOL_DESC_ID volId)
{
  volId->mounted = FALSE;

  return OK;
}

/*******************************************************************************
 * dosFsFdGet - Get file descriptor
 *
 * RETURNS: Pointer to filedescriptor
 ******************************************************************************/

LOCAL DOS_FILE_DESC* dosFsFdGet(DOS_VOL_DESC_ID volId)
{
  DOS_FILE_DESC *pFd, *pFdNew;
  DOS_FILE_HDL *pFileHdl, *pFileHdlNew;

  /* Initialize locals */
  pFd = volId->pFdList;
  pFdNew = NULL;
  pFileHdl = volId->pFhdlList;
  pFileHdlNew = NULL;

  semTake(volId->devSem, WAIT_FOREVER);

  /* Find first free filedescriptor */
  for (pFd = volId->pFdList;
       pFd < (volId->pFdList + volId->maxFiles);
       pFd++) {

    if (!pFd->busy) {

      pFdNew = pFd;
      break;

    }

  }

  /* If no free filedescriptor found */
  if (pFdNew == NULL) {

    semGive(volId->devSem);
    return NULL;

  }

  /* Clear struct */
  memset(pFdNew, 0, sizeof(DOS_FILE_DESC));

  /* Setup struct */
  pFdNew->pVolDesc = volId;
  pFdNew->busy = TRUE;
  pFdNew->pVolDesc->nBusyFd++;

  /* Get filehandle */
  for (pFileHdl = volId->pFhdlList;
       pFileHdl < (volId->pFhdlList + volId->maxFiles);
       pFileHdl++) {

    if (pFileHdl->nRef == 0) {

      pFileHdlNew = pFileHdl;
      break;

    }

  }

  assert(pFileHdlNew != NULL);

  /* Clear struct */
  memset(pFileHdlNew, 0, sizeof(DOS_FILE_HDL));

  /* Setup struct */
  pFileHdlNew->nRef = 1;

  /* Assign filehandle filedescriptor */
  pFdNew->pFileHdl = pFileHdlNew;

  semGive(volId->devSem);

  return pFdNew;
}

/*******************************************************************************
 * dosFsFdSemTake - Take file semaphore
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsFdSemTake(DOS_FILE_DESC *pFd, int timeout)
{
return OK;
  STATUS status;

  assert(pFd - pFd->pVolDesc->pFdList < pFd->pVolDesc->maxFiles);
  assert(pFd->pFileHdl - pFd->pVolDesc->pFhdlList < pFd->pVolDesc->maxFiles);

  status = semTake( *(pFd->pVolDesc->pFsemList +
		      (pFd->pFileHdl - pFd->pVolDesc->pFhdlList)), timeout);
  assert(status == OK);

  return status;
}

/*******************************************************************************
 * dosFsFdSemTake - Take file semaphore
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsFdSemGive(DOS_FILE_DESC *pFd)
{
return OK;
  STATUS status;

  assert(pFd - pFd->pVolDesc->pFdList < pFd->pVolDesc->maxFiles);
  assert(pFd->pFileHdl - pFd->pVolDesc->pFhdlList < pFd->pVolDesc->maxFiles);

  status = semGive( *(pFd->pVolDesc->pFsemList +
		      (pFd->pFileHdl - pFd->pVolDesc->pFhdlList)));
  assert(status == OK);

  return status;
}

/*******************************************************************************
 * dosFsFdFree - Free file descriptor
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosFsFdFree(DOS_FILE_DESC *pFd)
{
  DOS_VOL_DESC_ID volId;

  /* Initialize locals */
  volId = pFd->pVolDesc;

  assert(pFd != NULL);

  semTake(volId->devSem, WAIT_FOREVER);

  assert(pFd->pFileHdl->nRef != 0);

  pFd->pFileHdl->nRef--;
  pFd->busy = 0;

  semGive(volId->devSem);
}

/*******************************************************************************
 * dosFsHdlDeref - Unify filedescriptors to the same file
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosFsHdlDeref(DOS_FILE_DESC *pFd)
{
  DOS_VOL_DESC_ID volId;
  DOS_DIR_HDL *pDirHdlFd, *pDirHdl;
  DOS_FILE_HDL *pFhdl;
  int i;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pDirHdlFd = &pFd->pFileHdl->dirHdl;
  pDirHdl = NULL;
  pFhdl = volId->pFhdlList;

  semTake(volId->devSem, WAIT_FOREVER);

  /* For all filehandles */
  for (i = 0; i < volId->maxFiles; i++, pFhdl++) {

    if ( (pFhdl->nRef == 0) ||
	 (pFhdl == pFd->pFileHdl) ||
	 (pFhdl->deleted) ||
	 (pFhdl->obsolete) )
      continue;

    /* Check directory handles */
    pDirHdl = &pFhdl->dirHdl;
    if ( (pDirHdl->sector == pDirHdlFd->sector) &&
	 (pDirHdl->offset == pDirHdlFd->offset) ) {

      assert(pDirHdl->parDirStartCluster == pDirHdlFd->parDirStartCluster);
      assert(pFd->pFileHdl->nRef == 1);

      /* Clear struct */
      memset(pFd->pFileHdl, 0, sizeof(DOS_FILE_HDL));

      /* Setup struct */
      pFd->pFileHdl = pFhdl;
      pFhdl->nRef++;
      break;

    }

  }

  semGive(volId->devSem);

}

/*******************************************************************************
 * dosFsTrunc -  Truncate file (crete new or change existing file size)
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsTrunc(DOS_FILE_DESC *pFd, fsize_t len)
{
  DOS_VOL_DESC_ID volId;
  DOS_FILE_HDL *pFileHdl;

  /* Initialize locals */
  volId = pFd->pVolDesc;
  pFileHdl = pFd->pFileHdl;

  assert(volId->magic == DOS_FS_MAGIC);
  assert(pFd - pFd->pVolDesc->pFdList < pFd->pVolDesc->maxFiles);
  assert(pFd->pFileHdl - pFd->pVolDesc->pFhdlList < pFd->pVolDesc->maxFiles);

  /* If directory */
  if (pFileHdl->attrib & DOS_ATTR_DIRECTORY)
    return ERROR;

  /* If same lenght */
  if (pFileHdl->size == len)
    return OK;

  /* Check lenght */
  if (pFileHdl->size < len)
    return ERROR;

  /* Check mode */
  if (pFd->openMode == O_RDONLY)
    return ERROR;

  /* Update file */
  pFileHdl->size = len;
  if ( ( *volId->pDirDesc->updateEntry) (pFd,
				         DH_TIME_MODIFY | DH_TIME_ACCESS,
				         (time_t) 0) != OK)
    return ERROR;

  /* Mark as changed */
  pFd->pFileHdl->changed = 1;

  return OK;
}

/*******************************************************************************
 * dosFsSeek - Seek in file
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsSeek(DOS_FILE_DESC *pFd, fsize_t pos)
{
  fsize_t bufSize;
  unsigned int startSec, nSec;
  STATUS status;

  /* Inititialze locals */
  bufSize = (fsize_t) pFd->pFileHdl->size;
  status = ERROR;

  /* Reset seek out position */
  pFd->seekOutPos = 0;

  /* If already there */
  if (pos == pFd->pos)
    return OK;

  /* If directory */
  if (pFd->pFileHdl->attrib & DOS_ATTR_DIRECTORY)
    pFd->pFileHdl->size = (fsize_t) DOS_MAX_FSIZE;

  /* If postition past end-of-file */
  if (pos > pFd->pFileHdl->size) {

    pFd->seekOutPos = pos;
    return OK;

  }

  if (pFd->curSec > 0) {

    if (pFd->nSec == 0) {

      pFd->nSec = 1;
      pFd->curSec--;
      pFd->pos -= (fsize_t) (pFd->nSec << pFd->pVolDesc->secSizeShift);

    }

    else {

      pFd->pos -= (fsize_t) (OFFSET_IN_SEC(pFd->pVolDesc, pFd->pos));

    }

  }

  /* If move from start position */
  if ( (pos < pFd->pos) ||
       (pFd->curSec == 0) ) {

    nSec = NSECTORS(pFd->pVolDesc, min(pos, pFd->pFileHdl->size - (fsize_t) 1));

    /* Get start sector */
    startSec = FH_FILE_START;

  }

  /* Else move from current position */
  else {

    nSec = NSECTORS(pFd->pVolDesc,
		    (min(pos, pFd->pFileHdl->size - 1) - pFd->pos));
    if (nSec < pFd->nSec) {

      pFd->nSec -= nSec;
      pFd->curSec += nSec;
      goto seekRet;

    }

    /* Get start sector */
    startSec = pFd->curSec;

  }

  if (( *pFd->pVolDesc->pFatDesc->seek) (pFd, startSec, nSec) != OK)
    goto seekError;

seekRet:

  /* Update position */
  pFd->pos = pos;

  status = OK;

  if ( (pos == pFd->pFileHdl->size) &&
       (OFFSET_IN_SEC(pFd->pVolDesc, pos) == 0) ) {

    pFd->nSec--;
    pFd->curSec++;

  }

seekError:

  pFd->pFileHdl->size = (fsize_t) bufSize;

  return status;
}

/*******************************************************************************
 * dosFsSeekDir - Seek in directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsSeekDir(DOS_FILE_DESC *pFd, struct dirent *pDir)
{
  fsize_t off;

  /* Inititalize locals */
  off = (fsize_t) DIRENT_TO_POS(pDir);

  if (pFd->pos == off)
    return OK;

  /* If root dir */
  if (IS_ROOT(pFd)) {

    if (off >=
	(pFd->pVolDesc->pDirDesc->rootNSec << pFd->pVolDesc->secSizeShift) )
      return ERROR;

    pFd->pos = off;
    off = NSECTORS(pFd->pVolDesc, off);
    pFd->curSec = pFd->pVolDesc->pDirDesc->rootStartSec + off;
    pFd->nSec = pFd->pVolDesc->pDirDesc->rootNSec - off;

    return OK;

  } /* End if root */

  /* Else sub-directory as file */
  return dosFsSeek(pFd, (fsize_t) off);
}

/*******************************************************************************
 * dosFsFileExpand - Expand file size
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsFileExpand(DOS_FILE_DESC *pFd,
			     unsigned int fatFlags)
{
  DOS_VOL_DESC_ID volId;
  CBIO_DEV_ID devId;
  DOS_FILE_HDL *pFileHdl;
  fsize_t pos;

  /* Inititalize locals */
  volId = pFd->pVolDesc;
  devId = volId->pCbio;
  pFileHdl = pFd->pFileHdl;
  pos = (fsize_t) pFd->seekOutPos;

  assert(pos != 0);

  /* Seek to end-of-file */
  if (dosFsSeek(pFd, pFileHdl->size) == ERROR)
    return ERROR;

  /* If no need to expand file */
  if ( ((fsize_t) (pos - pFd->pos)) <
	(fsize_t) ((fsize_t) (volId->bytesPerSec) -
	(fsize_t) (OFFSET_IN_SEC(volId, pFd->pos))) ) {

    pFileHdl->size = pos;
    pFd->pos = pos;
    return OK;

  }

  /* This file already has a size */
  if ( (pFileHdl->size > 0) &&
       (OFFSET_IN_SEC(volId, pFd->pos) != 0) ) {

    if (pFd->nSec != 0) {

      pFd->nSec--;
      pFd->curSec++;
      pFd->pos += (fsize_t) (min(pos - pFd->pos,
				 volId->bytesPerSec -
				 OFFSET_IN_SEC(volId, pFd->pos)));
      pFileHdl->size = pFd->pos;

    }

  }

  /* While size less than requested */
  while (pFd->pos < pos) {

    /* Get block */
    if (pFd->nSec == 0) {

      if ( ( *volId->pFatDesc->getNext) (pFd, fatFlags) == ERROR)
        return ERROR;

    }

    /* Fill space */
    if (cbioIoctl(devId,
		  CBIO_CACHE_NEWBLK,
		  (ARG) pFd->curSec) == ERROR)
      return ERROR;

    /* Advance */
    pFd->pos += (fsize_t) (min(pos - pFd->pos, volId->bytesPerSec));
    pFileHdl->size = pFd->pos;

    if (OFFSET_IN_SEC(volId, pFd->pos) == 0) {

      pFd->curSec++;
      pFd->nSec--;

    }

  } /* End while size less than requested */

  return OK;
}

/*******************************************************************************
 * dosFsFileRW - Read/write to/from a file
 *
 * RETURNS: Number of bytes written, EOF or ERROR
 ******************************************************************************/

LOCAL int dosFsFileRW(DOS_FILE_DESC *pFd,
		      char *pBuf,
		      int maxBytes,
		      unsigned int op)
{
  DOS_VOL_DESC_ID volId;
  CBIO_DEV_ID devId;
  DOS_FILE_HDL *pFileHdl;
  DOS_DIR_PDESC *pDirDesc;
  CBIO_RW rw;
  size_t n, bytesLeft;
  off_t i;
  unsigned int fatFlags;
  STATUS status;

  /* Check if valid */
  if ( (pFd == NULL) ||
       (pFd->pVolDesc->magic != DOS_FS_MAGIC) )
    return ERROR;

  /* Check size */
  if (maxBytes == 0)
    return EOF;

  /* Check operation */
  assert( (op == _FREAD) || (op == _FWRITE) );

  /* Check fd number */
  assert(pFd - pFd->pVolDesc->pFdList < pFd->pVolDesc->maxFiles);
  assert(pFd->pFileHdl - pFd->pVolDesc->pFhdlList < pFd->pVolDesc->maxFiles);

  /* Check ready state */
  if ( (pFd->pVolDesc->mounted == FALSE) ||
       (cbioRdyChgdGet(pFd->pVolDesc->pCbio) == TRUE) )
    return ERROR;

  /* Inititalize locals */
  volId = pFd->pVolDesc;
  devId = volId->pCbio;
  pFileHdl = pFd->pFileHdl;
  n = 0;
  fatFlags = 0;
  status = ERROR;

  dosFsFdSemTake(pFd, WAIT_FOREVER);

  /* If directory */
  if (pFileHdl->attrib & DOS_ATTR_DIRECTORY)
    goto dosFsFileRWRet;

  /* If file state invalid */
  if (!pFd->busy)
    goto dosFsFileRWRet;

  /* If obsolete */
  if (pFileHdl->obsolete)
    goto dosFsFileRWRet;

  /* If deleted */
  if (pFileHdl->deleted)
    goto dosFsFileRWRet;

  /* Seek */
  if ( (pFd->seekOutPos != 0) &&
       (pFd->seekOutPos <= pFileHdl->size) ) {

    if (dosFsSeek(pFd, pFd->seekOutPos) == ERROR)
      goto dosFsFileRWRet;

  }

  /* Select operation */
  switch(op) {

    case _FREAD:

      if (pFd->openMode == O_WRONLY)
        goto dosFsFileRWRet;

      if ( (pFd->seekOutPos != 0) ||
	   (pFd->pos > pFileHdl->size) ) {

        /* Return EOF in this case */
        status = OK;
        goto dosFsFileRWRet;

      }

      if (pFd->pos == pFileHdl->size) {

        /* Return EOF in this case */
        status = OK;
        goto dosFsFileRWRet;

      }

      /* Set parameters */
      rw = CBIO_READ;
      fatFlags = FAT_NOT_ALLOC;

      /* Mark as accessed */
      pFd->accessed = 1;

      /* Verify range */
      maxBytes = min(maxBytes, pFileHdl->size - pFd->pos);

    break;

    case _FWRITE:

      if (pFd->openMode == O_RDONLY)
        goto dosFsFileRWRet;

      /* Get directory descriptor */
      pDirDesc = (DOS_DIR_PDESC *) volId->pDirDesc;

      if ((0xffffffff - pFd->pos) < maxBytes) {

        maxBytes = min(maxBytes,
		       ((fsize_t)(0xffffffff) - (fsize_t) (pFd->pos)));
	if (maxBytes < 2) {

          /* Return EOF in this case */
          status = OK;
          goto dosFsFileRWRet;

        }

      }

      /* Get alloc policy */
      if (maxBytes <= (volId->secPerClust << volId->secSizeShift) )
        fatFlags = FAT_ALLOC_ONE;
      else
        fatFlags = FAT_ALLOC;

      /* Fill space if needed */
      if (pFd->seekOutPos != 0) {

        if (dosFsFileExpand(pFd, fatFlags) == ERROR)
          goto dosFsFileRWRet;

      }

      /* Set parameters */
      rw = CBIO_WRITE;

      /* Mark as accessed and changed */
      pFileHdl->changed = 1;
      pFd->changed = 1;
      pFd->accessed = 1;

      /* Ok so far */
      status = OK;

    break;

  }

  /* Get start position */
  i = pFileHdl->startClust;

  /* If allocation needed */
  if (pFd->nSec == 0) {

    if ( ( *volId->pFatDesc->getNext) (pFd, fatFlags) == ERROR) {

      status = ERROR;
      goto dosFsFileRWRet;

    }

  }

  assert(pFd->nSec != 0);

  if (i == 0) {

    if ( ( *volId->pDirDesc->updateEntry) (pFd, 0, (time_t) 0) == ERROR) {

      pFileHdl->obsolete = 1;
      pFd->pVolDesc->nBusyFd--;

      goto dosFsFileRWRet;

    }

  }

  assert(pFileHdl->startClust != 0);

  /* Init I/O */
  bytesLeft = (size_t) maxBytes;
  i = OFFSET_IN_SEC(volId, pFd->pos);

  if (i != 0) {

    n = min(bytesLeft, (size_t) volId->bytesPerSec - i);
    if (cbioBytesRW(devId, pFd->curSec,
		    i, pBuf, n, rw,
		    &pFd->cbioCookie) == ERROR)
      goto dosFsFileRWRet;

    /* Advance */
    bytesLeft -= n;
    pBuf += n;
    pFd->pos += (fsize_t) n;

    /* Check sector */
    if (OFFSET_IN_SEC(volId, pFd->pos) == 0) {

      pFd->cbioCookie = (cookie_t) NULL;
      pFd->curSec++;
      pFd->nSec--;

    }

  }

  /* For all sectors */
  while (bytesLeft >= volId->bytesPerSec) {

    if (pFd->nSec == 0) {

      if ( ( *volId->pFatDesc->getNext) (pFd, fatFlags) == ERROR)
        goto dosFsFileRWRet;

    }

    n = min(NSECTORS(volId, bytesLeft), pFd->nSec);

    assert(n > 0);

    /* RW block */
    if (cbioBlkRW(devId,
		  pFd->curSec, n, pBuf, rw,
		  NULL) == ERROR)
      goto dosFsFileRWRet;

    /* Advance */
    i = n << volId->secSizeShift;
    bytesLeft -= i; 
    pBuf += i;
    pFd->pos += (fsize_t) i;
    pFd->curSec += n;
    pFd->nSec -= n;

  } /* End for all sectors */

  assert(bytesLeft < volId->bytesPerSec);

  n = bytesLeft;
  if (n > 0) {

    if (pFd->nSec == 0) {

      if ( ( *volId->pFatDesc->getNext) (pFd, fatFlags) == ERROR)
        goto dosFsFileRWRet;

    }

    if (pFd->pos >= pFileHdl->size) {

      assert(rw == CBIO_WRITE);

      if (cbioIoctl(devId,
		    CBIO_CACHE_NEWBLK,
		    (ARG) pFd->curSec) == ERROR)
        goto dosFsFileRWRet;

    }

    if (cbioBytesRW(devId,
		    pFd->curSec, 0, pBuf, n, rw,
		    &pFd->cbioCookie) == ERROR)
      goto dosFsFileRWRet;

    /* Advance */
    bytesLeft -= n;
    pBuf += n;
    pFd->pos += (fsize_t) n;

  }

  /* Congratulations */
  status = OK;

dosFsFileRWRet:

  if ( (rw == CBIO_WRITE) &&
       ((size_t) bytesLeft < (size_t) maxBytes) ) {

    if (pFd->pos > pFileHdl->size)
      pFileHdl->size = pFd->pos;

    ( *volId->pDirDesc->updateEntry) (pFd,
				      DH_TIME_ACCESS | DH_TIME_MODIFY,
				      (time_t) 0);
    pFd->changed = 0;

  }

  dosFsFdSemGive(pFd);

  if (status == ERROR)
    return ERROR;

  return (maxBytes - bytesLeft);
}

/*******************************************************************************
 * dosFsStatGet - Get file statistics
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsStatGet(DOS_FILE_DESC *pFd,
			  struct stat *pStat)
{
  DOS_VOL_DESC_ID volId;
  DOS_FILE_HDL *pFileHdl;

  /* Inititalize locals */
  volId = pFd->pVolDesc;
  pFileHdl = pFd->pFileHdl;

  /* Inititalize arguments */
  memset(pStat, 0, sizeof(struct stat));

  pStat->st_dev = (dev_t) volId;
  pStat->st_nlink = (nlink_t) 1;
  pStat->st_size = (off_t) pFileHdl->size;
  pStat->st_blksize = (blksize_t) (volId->secPerClust << volId->secSizeShift);
  if (pStat->st_blksize == 0)
    return ERROR;

  pStat->st_blocks = (pStat->st_size + (pStat->st_blksize - 1)) /
		     pStat->st_blksize;

  /* Only one group for dos files */
  pStat->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;

  if ((pFileHdl->attrib & DOS_ATTR_RDONLY) != 0)
    pStat->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);

  if (pFileHdl->attrib & DOS_ATTR_DIRECTORY) {

    pStat->st_mode |= S_IFDIR;
    pStat->st_size = pStat->st_blksize; /* Pretend to use one block */

  }

  else {

    assert((pFileHdl->attrib & DOS_ATTR_VOL_LABEL) == 0);

    pStat->st_mode |= S_IFREG;

  }

  /* Get time/date */
  if ( ( *volId->pDirDesc->dateGet) (pFd, pStat) == ERROR )
    return ERROR;

  return OK;
}

/*******************************************************************************
 * dosFsCreate - Create function
 *
 * RETURNS: File descriptor id or ERROR
 ******************************************************************************/

LOCAL DOS_FILE_DESC* dosFsCreate(DOS_VOL_DESC_ID volId,
				 char *name,
				 int flags)
{
  DOS_FILE_DESC *pFd;
  time_t currTime;
  int mode;

  /* Setup mode for creation */
  mode = flags | O_CREAT;
  if ( S_ISREG(flags))
    mode |= O_TRUNC;

  /* Create file using open function */
  pFd = dosFsOpen(volId, name, mode, flags & S_IFMT);
  if (pFd == (DOS_FILE_DESC *) ERROR)
    return (DOS_FILE_DESC *) ERROR;

  /* Get current time */
  currTime = time(&currTime);

  /* Set creation date */
  dosFsFdSemTake(pFd, WAIT_FOREVER);

  if ( (!pFd->busy) ||
       (pFd->pFileHdl->deleted) ||
       (pFd->pFileHdl->obsolete) ) {

    assert(FALSE);
    dosFsFdSemGive(pFd);
    return (DOS_FILE_DESC *) ERROR;

  }

  /* Update direntry */
  ( *volId->pDirDesc->updateEntry) (pFd,
				    DH_TIME_CREAT |
				    DH_TIME_ACCESS |
				    DH_TIME_MODIFY,
				    currTime);

  dosFsFdSemGive(pFd);

  return pFd;
}

/*******************************************************************************
 * dosFsDelete - Delete function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsDelete(DOS_VOL_DESC_ID volId,
			 char *name)
{
  return OK;
}

/*******************************************************************************
 * dosFsOpen - Open function
 *
 * RETURNS: File descriptor id or ERROR
 ******************************************************************************/

LOCAL DOS_FILE_DESC* dosFsOpen(DOS_VOL_DESC_ID volId,
				 char *name,
				 int flags,
				 int mode)
{
  DOS_FILE_DESC* pFd;
  BOOL devSem;
  BOOL fileSem;
  STATUS status;
  unsigned int oldOpenMode;

  /* Initialize locals */
  pFd = NULL;
  devSem = FALSE;
  fileSem = FALSE;
  status = ERROR;

  if ( (volId == NULL) ||
       (volId->magic != DOS_FS_MAGIC) )
    return (DOS_FILE_DESC *) ERROR;

  if ( (name == NULL) ||
       (strlen(name) > PATH_MAX) )
    return (DOS_FILE_DESC *) ERROR;

  semTake(volId->devSem, WAIT_FOREVER);
  devSem = TRUE;

  if ( (volId->mounted == FALSE) ||
       (cbioIoctl(volId->pCbio, CBIO_STATUS_CHK, 0) == ERROR) ||
       (cbioRdyChgdGet(volId->pCbio) == TRUE) ) {

    if (dosFsVolMount(volId) != OK)
      goto openRet;

  }

  /* If not directory */
  if ( !S_ISDIR(mode) )
    mode = S_IFREG;

  /* Check mode resonability */
  if (cbioModeGet(volId->pCbio) == O_RDONLY) {

    if ( (flags & O_CREAT) ||
	 (flags & O_TRUNC) ||
	 ((flags & 0x3) != 0) )
      goto openRet;

  }

  /* If create and truncate file */
  if ( (flags & O_CREAT) &&
       (!S_ISDIR(mode)) &&
       (flags & O_TRUNC) ) {

    if (dosFsDelete(volId, name) != OK)
      goto openRet;

  }

  /* Get a new filedescriptor */
  pFd = dosFsFdGet(volId);
  if (pFd == NULL)
    goto openRet;

  /* Search if filename exists */
  if ( (( *volId->pDirDesc->pathLkup)
		(pFd, name,
		 (flags & DOS_O_CASENS) | (flags & 0xf00) | mode) == ERROR) )
    goto openRet;

  /* Share filehandles */
  dosFsHdlDeref(pFd);

  /* Check attributes */
  if ( (pFd->pFileHdl->attrib & DOS_ATTR_RDONLY) &&
       ((flags & 0x0f) != O_RDONLY) ) {

    status = ERROR;
    goto openRet;

  }

  /* Set open mode */
  pFd->openMode = flags & 0x0f;

  semGive(volId->devSem);
  devSem = FALSE;

  if ( ((pFd->pFileHdl->attrib & DOS_ATTR_DIRECTORY) == 0) &&
       ((flags & O_CREAT) != 0) &&
       S_ISDIR(mode) ) {

    status = ERROR;
    goto openRet;

  }

  if (flags & (O_TRUNC | DOS_O_CONTIG_CHK) ) {

    dosFsFdSemTake(pFd, WAIT_FOREVER);
    fileSem = TRUE;

    /* If file busy */
    if (!pFd->busy) {

      assert(pFd->busy);
      goto openRet;

    }

    /* If obsolete or deleted */
    if ( (pFd->pFileHdl->deleted) ||
         (pFd->pFileHdl->obsolete) ) {

      status = ERROR;
      goto openRet;

    }

    /* If truncate */
    if (flags & O_TRUNC) {

      /* Save old open mode */
      oldOpenMode = pFd->openMode;

      pFd->openMode = O_RDWR;
      status = dosFsTrunc(pFd, 0);
      pFd->openMode = oldOpenMode;
      if (status == ERROR)
        goto openRet;

    }

    if (flags & DOS_O_CONTIG_CHK)
      ( *volId->pFatDesc->contigChk) (pFd);

  }

  status = OK;

openRet:

  if (devSem)
    semGive(volId->devSem);

  if (fileSem)
    dosFsFdSemGive(pFd);

  if (status == ERROR) {

    if (pFd != NULL)
      dosFsFdFree(pFd);

    return (DOS_FILE_DESC *) ERROR;

  }

  return pFd;
}

/*******************************************************************************
 * iosFsClose - Close function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsClose(DOS_FILE_DESC *pFd)
{
  DOS_VOL_DESC_ID volId;
  DOS_FILE_HDL *pFileHdl;
  BOOL flush;
  STATUS status;
  fsize_t i;
  time_t currTime;
  unsigned int flags;

  /* Inititalize locals */
  volId = pFd->pVolDesc;
  pFileHdl = pFd->pFileHdl;
  flush = FALSE;
  status = ERROR;
  i = (fsize_t) 0;

  /* Check file descritptor */
  if ( (pFd == NULL) ||
       (pFd == (DOS_FILE_DESC *) ERROR) ||
       !pFd->busy ||
       (volId->magic != DOS_FS_MAGIC) ) {

    assert(FALSE);
    return ERROR;

  }

  assert(pFd - pFd->pVolDesc->pFdList < pFd->pVolDesc->maxFiles);
  assert(pFd->pFileHdl - pFd->pVolDesc->pFhdlList < pFd->pVolDesc->maxFiles);

  /* Lock file */
  if (dosFsFdSemTake(pFd, WAIT_FOREVER) != OK)
    return ERROR;

  /* If file obsolete */
  if (pFileHdl->obsolete) {

    status = ERROR;
    goto closeRet;

  }

  /* Decrease file count */
  volId->nBusyFd--;

  /* If file deleted */
  if (pFileHdl->deleted) {

    flush = TRUE;
    status = ERROR;
    goto closeRet;

  }

  /* If directory */
  if (pFileHdl->attrib & DOS_ATTR_DIRECTORY) {

    flush = TRUE;
    status = OK;
    goto closeRet;

  }

  /* If update date */
  if (volId->updateLastAccessDate)
    pFd->accessed = 0;

  /* If file changed */
  if (pFd->accessed || pFd->changed) {

    /* Get flags */
    if (pFd->accessed)
      flags = DH_TIME_ACCESS;
    else
      flags = DH_TIME_ACCESS | DH_TIME_MODIFY;

    /* Get current time */
    currTime = time(&currTime);

    /* If not read only */
    if (cbioModeGet(volId->pCbio) != O_RDONLY) {

      ( *volId->pDirDesc->updateEntry) (pFd, flags, currTime);
      pFd->accessed = 0;

    }

    /* Else read only */
    else {

      status = ERROR;
      goto closeRet;

    }

  }

  /* Flush buffers if changed */
  if (pFd->changed ||
      (pFileHdl->changed && pFileHdl->nRef == 1) ) {

    if (pFileHdl->nRef == 1) {

      /* Get last position */
      if (pFileHdl->size == 0)
        i = 0;
      else
        i = pFileHdl->size - (fsize_t) 1;

      /* Seek to last position */
      dosFsSeek(pFd, i);

      /* Give back free clusters */
      ( *volId->pFatDesc->truncate) (pFd, pFd->curSec, FH_EXCLUDE);

      /* Reset change flag */
      pFileHdl->changed = 0;

    }

    /* Should flush */
    flush = TRUE;
    pFd->changed = 0;

  }

  status = OK;

closeRet:

  if (flush)
    ( *volId->pFatDesc->flush) (pFd);

  dosFsFdSemGive(pFd);

  dosFsFdFree(pFd);

  if ( (volId->nBusyFd == 0) || flush ) {

    cbioIoctl(volId->pCbio, CBIO_CACHE_FLUSH, 0);
    flush = FALSE;

  }

  return status;
}

/*******************************************************************************
 * dosFsRead - Read function
 *
 * RETURNS: Bytes read, EOF or ERROR
 ******************************************************************************/

LOCAL int dosFsRead(DOS_FILE_DESC *pFd,
		    char *pBuf,
		    int maxBytes)
{
  if (cbioModeGet(pFd->pVolDesc->pCbio) == O_WRONLY)
    return ERROR;

  return dosFsFileRW(pFd, pBuf, maxBytes, _FREAD);
}

/*******************************************************************************
 * dosFsWrite - Write function
 *
 * RETURNS: Bytes written, EOF or ERROR
 ******************************************************************************/

LOCAL int dosFsWrite(DOS_FILE_DESC *pFd,
		     char *pBuf,
		     int maxBytes)
{
  if (cbioModeGet(pFd->pVolDesc->pCbio) == O_RDONLY)
    return ERROR;

  return dosFsFileRW(pFd, pBuf, maxBytes, _FWRITE);
}

/*******************************************************************************
 * dosFsIoctl - Ioctl function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFsIoctl(DOS_FILE_DESC *pFd,
		        int cmd,
			ARG arg)
{
  DOS_VOL_DESC_ID volId;
  STATUS status;

  /* Inititalize locals */
  volId = pFd->pVolDesc;
  status = ERROR;

  /* Select command */
  switch(cmd) {

    case FIODISKINIT:

      if (dosFsVolFormatFunc != NULL)
        status = ( *dosFsVolFormatFunc) (volId->pCbio, arg, NULL);

    break;

    case FIOSEEK:

      status = dosFsSeek(pFd, (fsize_t) arg);

    break;

    case FIOLABELGET:

    status = ( *volId->pDirDesc->volLabel) (volId,
					    (unsigned char *) arg,
					    FIOLABELGET);

    break;

    case FIOLABELSET:

      if (dosFsFdSemTake(pFd, WAIT_FOREVER) == ERROR)
        break;

      status = ( *volId->pDirDesc->volLabel) (volId,
					      (unsigned char *) arg,
					      FIOLABELSET);

      dosFsFdSemGive(pFd);

    break;

    case FIOREADDIR:

      pFd->pos = DIRENT_TO_POS((struct dirent *) arg);
      if ( (pFd->pos) ||
	   dosFsSeekDir(pFd, (struct dirent *) arg) == OK) {

          status = ( *pFd->pVolDesc->pDirDesc->readDir)
		   (pFd, (struct dirent *) arg, NULL);

      }

    break;

    case FIOSTATGET:

      status = dosFsStatGet(pFd, (struct stat *) arg);

    break;

    default:

      status = cbioIoctl(volId->pCbio, cmd, arg);

    break;

  } /* End select command */

  return status;
}

