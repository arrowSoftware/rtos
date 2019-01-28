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

/* dosSuperLib.c - Dos bootsector library */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <os/logLib.h>
#include <io/blkIo.h>
#include <io/cbioLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/private/dosDirLibP.h>

/* Defines */

/* Imports */

/* Locals */
LOCAL int dosSuperFatTypeGet(unsigned char *pBuf);

/* Globals */

/* Functions */

/*******************************************************************************
 * dosSuperLibInit - Initialize boot sector library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosSuperLibInit(void)
{
  return OK;
}

/*******************************************************************************
 * dosSuperFatTypeGet - Determine if filesystem is FAT12 or FAT16
 *
 * RETURNS: TRUE if FAT12, FALSE is FAT16 or ERROR if invalid
 ******************************************************************************/

LOCAL int dosSuperFatTypeGet(unsigned char *pBuf)
{
  unsigned int dirEntSize, nRootEnts;
  unsigned int bytesPerSec, rootDirSecs, reservedSecs;
  unsigned int totalSecs, dataRngSecs, nFats, secsPerFat;
  unsigned int secsPerClust, clusterCount, n;

  if (pBuf == NULL)
    return ERROR;

  /* Get number of sectors */
  n = DISK_TO_16BIT(&pBuf[DOS_BOOT_NSECTORS]);
  if (n == 0)
    n = DISK_TO_32BIT(&pBuf[DOS_BOOT_LONG_NSECTORS]);
  totalSecs = n;
  if (totalSecs == 0)
    return ERROR;

  /* Get bytes per sector */
  bytesPerSec = DISK_TO_16BIT(&pBuf[DOS_BOOT_BYTES_PER_SEC]);
  if (bytesPerSec == 0)
    return ERROR;

  /* Get dir entry size */
  dirEntSize = DOS_DIRENT_STD_LEN;

  /* Get number of root entries */
  nRootEnts = DISK_TO_16BIT(&pBuf[DOS_BOOT_MAX_ROOT_ENTS]);
  if (nRootEnts == 0)
    return ERROR;

  /* Get number of sectors per cluster */
  secsPerClust = pBuf[DOS_BOOT_SEC_PER_CLUST];
  if (secsPerClust == 0)
    return ERROR;

  /* Get number of fat copies */
  nFats = pBuf[DOS_BOOT_NFATS];
  if (nFats == 0)
    return ERROR;

  /* Get number of reserved sectors */
  reservedSecs = DISK_TO_16BIT(&pBuf[DOS_BOOT_NRESRVD_SECS]);

  /* Get number of sectors per fat */
  secsPerFat = DISK_TO_16BIT(&pBuf[DOS_BOOT_SEC_PER_FAT]);
  if (secsPerFat == 0)
    return ERROR;

  /* Calculate number of sectors used by root directory */
  rootDirSecs = ( (nRootEnts * dirEntSize) +
		  (bytesPerSec - 1) ) / bytesPerSec;

  /* Calculate numner of sectors in data region */
  dataRngSecs = totalSecs - (reservedSecs + (nFats * secsPerFat) +
			     rootDirSecs);

  /* Calulcate cluster count */
  clusterCount = dataRngSecs / secsPerClust;

  /* If FAT12 */
  if (clusterCount < DOS_FAT_12BIT_MAX)
    return TRUE;

  return FALSE;
}

/*******************************************************************************
 * dosSuperGet - Get boot sector parameters
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosSuperGet(DOS_VOL_DESC_ID volId)
{
  CBIO_PARAMS params;
  unsigned int n;
  unsigned char bootSec[512];
  unsigned char type[DOS_BOOT_FSTYPE_LEN + 1];
  unsigned char pass;

  /* Initialize boot sector number */
  volId->bootSecNum = DOS_BOOT_SEC_NUM;

bootGet:

  pass = 0;

  /* Get parameters */
  if (cbioParamsGet(volId->pCbio, &params) != OK)
    return ERROR;

  /* Check parameters */
  if (params.bytesPerBlk == 0) {

    logMsg("Boot sector error: Bytes per block can't be zero.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    if (cbioRdyChgdGet(volId->pCbio) == TRUE)
      return ERROR;

    goto bootGetError;

  }

  /* Read boot data */
  n = min(params.bytesPerBlk, sizeof(bootSec));
  if ( cbioBytesRW(volId->pCbio, volId->bootSecNum, 0,
		   (char *) bootSec, n, CBIO_READ, NULL) == ERROR ) {

    logMsg("Boot sector error: Median not formatted or not present.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    if (cbioRdyChgdGet(volId->pCbio) == TRUE)
      return ERROR;

    goto bootGetError;

  }

  /* Check boot data */
  if ( (bootSec[DOS_BOOT_JMP] != 0xe9) &&
       (bootSec[DOS_BOOT_JMP] != 0xeb) ) {

    logMsg("Boot sector error: Invalid 'jmp' code %#x.\n",
	   (ARG) (unsigned int) bootSec[DOS_BOOT_JMP],
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  /* Read last 2 bytes of sector */
  if (cbioBytesRW(volId->pCbio, volId->bootSecNum,
		  params.bytesPerBlk - 2, (char *) &n,
		  2, CBIO_READ, NULL) == ERROR) {

    logMsg("Boot sector error: Error reading last 2 bytes.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    if (cbioRdyChgdGet(volId->pCbio) == TRUE)
      return ERROR;

    goto bootGetError;

  }

  /* Check value */
  n = DISK_TO_16BIT(&n);
  if (n != 0xaa55) {

    if ( (bootSec[510] != 0x55) ||
	 (bootSec[511] != 0xaa) ) {

    logMsg("Boot sector error: At the end of boot sector: %#x.\n",
	   (ARG) params.bytesPerBlk,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

      }

      goto bootGetError;

  }

  /* Check bytes per sector */
  n = DISK_TO_16BIT(&bootSec[DOS_BOOT_BYTES_PER_SEC]);
  volId->bytesPerSec = n;
  if (n == 0) {

    logMsg("Boot sector error: Bytes per sector can't be zero.\n",
	   (ARG) 0,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  if (n != params.bytesPerBlk) {

    logMsg("Boot sector error: Bytes per sector must match device's"
	   " bytes per block.\n",
	   (ARG) 0,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  /* Calculate sector byte shift */
  for (volId->secSizeShift = 0, n = 5; n < 16; n++) {

    if ( (1 << n) == volId->bytesPerSec ) {

      volId->secSizeShift = n;
      break;

    }

  }
  if (volId->secSizeShift == 0) {

    logMsg("Boot sector error: Bytes per sector can't be zero.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  /* Check number of sectors on volume */
  n = DISK_TO_16BIT(&bootSec[DOS_BOOT_NSECTORS]);
  if (n == 0) {

    /* Might be a large disk */
    n = DISK_TO_32BIT(&bootSec[DOS_BOOT_LONG_NSECTORS]);
    if (n == 0) {

      logMsg("Boot sector error: Invalid number of sectors.\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

      goto bootGetError;

    }

  }

  /* Set number of sectors */
  volId->totalSec = n;
  if (n != params.nBlocks) {

    if (n < params.nBlocks) {

      logMsg("Boot sector warning: Number of sectors less than number"
	     " of blocks on device.\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

    }

    else {

      logMsg("Boot sector error: Number of sectors greater than number"
	     " of blocks on device.\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

      goto bootGetError;

    }

  }

  /* Get number of sectors per cluster */
  volId->secPerClust = bootSec[DOS_BOOT_SEC_PER_CLUST];
  if (volId->secPerClust == 0) {

    logMsg("Boot sector error: Sectors per cluster can't be zero.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  /* Get number of fat copies */
  volId->nFats = bootSec[DOS_BOOT_NFATS];
  if (volId->nFats == 0) {

    logMsg("Boot sector error: Number of fat copies can't be zero.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  /* Get number of hidden sectors */
  volId->nHiddenSecs = DISK_TO_16BIT(&bootSec[DOS_BOOT_NHIDDEN_SECS]);

  /* Get number of reserved sectors */
  volId->nReservedSecs = DISK_TO_16BIT(&bootSec[DOS_BOOT_NRESRVD_SECS]);
  if (volId->nReservedSecs == 0) {

    logMsg("Boot sector error: Number of reserved sectors can't be zero.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    goto bootGetError;

  }

  /* Get numner of sectors for fat table */
  volId->secPerFat = DISK_TO_16BIT(&bootSec[DOS_BOOT_SEC_PER_FAT]);

  /* If FAT12 or FAT16 */
  if (volId->secPerFat != 0) {

    if (volId->secPerFat > (unsigned long) 2 * 0x10000 / volId->bytesPerSec) {

      logMsg("Boot sector error: Invalid number of sectors per fat %#x.\n",
	     (ARG) volId->secPerFat,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

      goto bootGetError;

    }

    /* Get dos fstype */
    memcpy(type, &bootSec[DOS_BOOT_FSTYPE_ID], DOS_BOOT_FSTYPE_LEN);

    /* Determine if FAT12 or FAT16 */
    n = dosSuperFatTypeGet(bootSec);
    if (n == ERROR) {

      logMsg("Boot sector error: Unable to determine FAT type.\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

      goto bootGetError;

    }

    /* If FAT12 calculated */
    if (n) {

      if (strcmp(type, DOS_BOOT_FSTYPE_FAT16) == 0) {

        volId->fatType = FAT16;
        logMsg("Boot sector warning: FAT16 boot string but FAT12 calculated.\n",
	       (ARG) 1,
	       (ARG) 2,
	       (ARG) 3,
	       (ARG) 4,
	       (ARG) 5,
	       (ARG) 6);

      }

      else {

        volId->fatType = FAT12;

      }

    }

    /* Else FAT16 calculated */
    else {

      if (strcmp(type, DOS_BOOT_FSTYPE_FAT12) == 0) {

        volId->fatType = FAT12;
        logMsg("Boot sector warning: FAT12 boot string but FAT16 calculated.\n",
	       (ARG) 1,
	       (ARG) 2,
	       (ARG) 3,
	       (ARG) 4,
	       (ARG) 5,
	       (ARG) 6);

      }

      else {

        volId->fatType = FAT16;

      }

    }

    /* Volume id and label */
    volId->volIdOff = DOS_BOOT_VOL_ID;
    volId->volLabOff = DOS_BOOT_VOL_LABEL;

  }

  /* Else FAT32 */
  else {

    volId->fatType = FAT32;

    /* Get number of sectors per fat */
    volId->secPerFat = DISK_TO_32BIT(&bootSec[DOS32_BOOT_SEC_PER_FAT]);
    if (volId->secPerFat == 0) {

      logMsg("Boot sector error: Number of sectors per fat can't be zero.\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

      goto bootGetError;

    }

    /* Volume id and label */
    volId->volIdOff = DOS32_BOOT_VOL_ID;
    volId->volLabOff = DOS32_BOOT_VOL_LABEL;

  }

  /* Calculate data area start cluster */
  volId->dataStartSec = volId->nReservedSecs +
			volId->secPerFat * volId->nFats;

  /* Get id and volume label */
  volId->volId = DISK_TO_32BIT(&bootSec[volId->volIdOff]);
  memcpy(volId->bootVolLab, &bootSec[volId->volLabOff], DOS_VOL_LABEL_LEN);
  volId->bootVolLab[DOS_VOL_LABEL_LEN] = EOS;

  /* Restore boot sector */
  if (volId->bootSecNum != DOS_BOOT_SEC_NUM) {

    logMsg("Boot sector warning: Try to reclaim original copy of boot sector\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    if ( (!pass) &&
	 (cbioBlkCopy(volId->pCbio, volId->bootSecNum,
		      DOS_BOOT_SEC_NUM, 1) == OK) &&
	 (cbioIoctl(volId->pCbio, CBIO_CACHE_FLUSH, (ARG) -1) == OK) ) {

      pass++;
      volId->bootSecNum = DOS_BOOT_SEC_NUM;
      goto bootGet;

    }

  }

  return OK;

bootGetError:

  volId->bootSecNum++;
  if (volId->bootSecNum < DOS_BOOT_SEC_NUM + params.blocksPerTrack) {

    if (volId->bootSecNum == DOS_BOOT_SEC_NUM + 1) {

      logMsg("Boot sector warning: Problem fiding volume data, trying to"
	     " to use the next block as boot block.\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

       goto bootGet;

    }

    logMsg("Boot sector error: Ensure this device is formatted and partitions"
	   " are properly handled.\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);
  }

  return ERROR;
}

