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

/* dosFmtLib.c - Dos filesystem format utitlites */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <io/cbioLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/dosFsShow.h>
#include <oldfs/private/dosDirLibP.h>
#include <oldfs/private/dosFmtLibP.h>

/* Defines */
#define DOS_BOOT_SEC_NUM	0		/* Boot sector number */
#define DOS_MIN_CLUST		2		/* Minimum clusters */

/* Imports */
IMPORT FUNCPTR dosFsVolFormatFunc;

/* Locals */
LOCAL STATUS dosFmtAutoParams(DOS_VOL_CONFIG *pConf,
			      unsigned long nBlocks,
			      int bytesPerBlk);
LOCAL STATUS dosFmtVolFormat(void *device, int opt, FUNCPTR promptFunc);

/* Globals */

/* Functions */

/*******************************************************************************
 * dosFmtLibInit - Inititalize format library
 *
 * RETURNS: N/A
 ******************************************************************************/

void dosFmtLibInit(void)
{
  dosFsVolFormatFunc = (FUNCPTR) dosFmtVolFormat;
}

/*******************************************************************************
 * dosFmtAutoParams - Default calculation for format
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFmtAutoParams(DOS_VOL_CONFIG *pConf,
			      unsigned long nBlocks,
			      int bytesPerBlk)
{
  unsigned long nClust, maxClust, minClust;
  int rootSecs, fatBytes;
  int dirEntrySize;

  /* Inititalize locals */
  dirEntrySize = DOS_DIRENT_STD_LEN;

  /* Verify bytes per block */
  if ( (bytesPerBlk < (dirEntrySize * 3)) ||
       ((bytesPerBlk < 64) && (nBlocks > 0xfffe)) ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  /* If bytes per block lt. normal */
  if (bytesPerBlk < 512) {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = 0xfd;

    if (pConf->nFats == 0)
      pConf->nFats = 1;

    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 1;

  } /* End if bytes per block lt. normal */

  /* Else if 630Kb 5.25 */
  else if (nBlocks <= 720) {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = 0xfd;

    if (pConf->fatType == _AUTO)
      pConf->fatType = _FAT12;

    pConf->maxRootEnts = DEFAULT_ROOT_ENTS;
    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 2;

  } /* End else if 630Kb 5.25 */

  /* Else if 720Kb 3.5 */
  else if (nBlocks <= 1440) {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = 0xf9;

    if (pConf->fatType == _AUTO)
      pConf->fatType = _FAT12;

    pConf->maxRootEnts = DEFAULT_ROOT_ENTS;
    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 2;

  } /* End else if 720Kb 3.5 */

  /* Else if 1.2Mb 5.25 */
  else if (nBlocks <= 2400) {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = 0xf9;

    if (pConf->fatType == _AUTO)
      pConf->fatType = _FAT12;

    pConf->maxRootEnts = DEFAULT_ROOT_ENTS;
    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 2;

  } /* End else if 1.2Mb 5.25 */

  /* Else if 1.44Mb 3.5 */
  else if (nBlocks <= 2880) {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = 0xf0;

    if (pConf->fatType == _AUTO)
      pConf->fatType = _FAT12;

    pConf->maxRootEnts = DEFAULT_ROOT_ENTS;
    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 2;

  } /* End else if 1.44Mb 3.5 */

  /* Else if harddisk etc. */
  else if (nBlocks <= 4000) {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = DEFAULT_MEDIA_BYTE;

    if (pConf->fatType == _AUTO)
      pConf->fatType = _FAT12;

    pConf->maxRootEnts = DEFAULT_ROOT_ENTS;
    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 1;

  } /* End else if harddisk etc. */

  /* Else unknown */
  else {

    if (pConf->secPerClust == 0)
      pConf->secPerClust = 1;

    if (pConf->mediaByte == 0)
      pConf->mediaByte = DEFAULT_MEDIA_BYTE;

    pConf->maxRootEnts = DEFAULT_ROOT_ENTS;
    rootSecs = ((DEFAULT_ROOT_ENTS * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk;
    minClust = 2;

  } /* End else unknown */

  if (pConf->maxRootEnts != 0)
    rootSecs = (((pConf->maxRootEnts * dirEntrySize) + (bytesPerBlk - 1)) /
	       bytesPerBlk);

  if (pConf->nFats == 0)
    pConf->nFats = DEFAULT_NFATS;
  pConf->nFats = min(pConf->nFats, MAX_NFATS);

  nClust = nBlocks / pConf->secPerClust;

  /* If auto fat type */
  if (pConf->fatType == _AUTO) {

    if (nClust < DOS_FAT_12BIT_MAX)
      pConf->fatType = _FAT12;

    else if (nClust < 0x400000)
      pConf->fatType = _FAT16;

   else
     pConf->fatType = _FAT32;

  } /* End if auto fat type */

  /* Select fat type */
  switch (pConf->fatType) {

    case _FAT12:
      if (pConf->nReserved == 0)
        pConf->nReserved = 1;
      maxClust = DOS_FAT_12BIT_MAX;
      minClust = 1;
    break;

    default:
    case _FAT16:
      if (pConf->nReserved == 0)
        pConf->nReserved = 1;
      maxClust = DOS_FAT_16BIT_MAX;
      minClust = 2;
    break;

    case _FAT32:
      if (pConf->nReserved == 0)
        pConf->nReserved = 32;
      maxClust = DOS_FAT_32BIT_MAX;
      minClust = 8;
    break;

  } /* End select fat type */

  /* Calculate sectors per cluster */
  while ( (nClust > maxClust) ||
	  ((unsigned long) pConf->secPerClust < minClust) ) {

    pConf->secPerClust <<= 1;
    nClust = nBlocks / pConf->secPerClust;

  }

  pConf->secPerClust = min(pConf->secPerClust, 254);
  nClust = nBlocks / pConf->secPerClust;

  /* Select fat type */
  switch (pConf->fatType) {

    case _FAT12:
      fatBytes = 3 * nClust / 2;
      strcpy(pConf->sysId, "rtosDOS12");
    break;

    default:
    case _FAT16:
      fatBytes = 2 * nClust;
      strcpy(pConf->sysId, "rtosDOS16");
    break;

    case _FAT32:
      fatBytes = 4 * nClust;
      rootSecs = 0;
      strcpy(pConf->sysId, "rtosDOS32");
    break;

  } /* End select fat type */

  /* Calculate sectors per fat */
  pConf->secPerFat = (fatBytes + bytesPerBlk - 1) / bytesPerBlk;

  /* Calculate root dir size */
  pConf->maxRootEnts = rootSecs * bytesPerBlk / dirEntrySize;
  if ( (pConf->fatType != _FAT32) &&
       (pConf->maxRootEnts > DEFAULT_MAX_ROOT_ENTS) )
    pConf->maxRootEnts = DEFAULT_MAX_ROOT_ENTS;

  /* Store number of clusters */
  pConf->nClust = (nBlocks - pConf->nReserved - rootSecs - pConf->nHidden -
	           (pConf->secPerFat * pConf->nFats)) / pConf->secPerClust;
  if ( (pConf->nHidden + pConf->nReserved + rootSecs +
	(pConf->secPerFat * pConf->nFats) +
	(pConf->nClust * pConf->secPerClust)) > nBlocks ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  if (pConf->secPerClust > 255) {

    errnoSet(EINVAL);
    return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * dosFmtVolFormat - Format function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosFmtVolFormat(void *device, int opt, FUNCPTR promptFunc)
{
  CBIO_DEV_ID devId;
  CBIO_PARAMS params;
  DOS_VOL_CONFIG conf;
  STATUS status;

  /* Inititalize locals */
  status = ERROR;

  if (device == NULL)
    return ERROR;

  /* Try to get device */
  if (cbioDevVerify((CBIO_DEV_ID) device) == OK) {

    devId = (CBIO_DEV_ID) device;

  }

  else if ( ((DOS_VOL_DESC_ID) device)->magic == DOS_FS_MAGIC ) {

    devId = ((DOS_VOL_DESC_ID) device)->pCbio;

  }

  /* Clear config */
  memset(&conf, 0, sizeof(DOS_VOL_CONFIG));

  /* Lock */
  if (cbioLock(devId, WAIT_FOREVER) == ERROR)
    return ERROR;

  /* Reset device */
  if (cbioIoctl(devId, CBIO_RESET, (ARG) 0) == ERROR)
    goto dosFmtRet;

  /* Must be write enabled */
  if (cbioModeGet(devId) == O_RDONLY)
    goto dosFmtRet;

  /* Get parameters */
  if (cbioParamsGet(devId, &params) == ERROR)
    goto dosFmtRet;

  /* Get auto params */
  status = dosFmtAutoParams(&conf, params.nBlocks, params.bytesPerBlk);

  /* Show parameters */
  dosFmtShow(&conf);

dosFmtRet:

  cbioUnlock(devId);
  return status;
}

