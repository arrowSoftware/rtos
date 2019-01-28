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

/* cbioShow.c - Block i/o show routines */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <rtos.h>
#include <util/classLib.h>
#include <rtos/errnoLib.h>
#include <io/cbioLib.h>
#include <io/private/cbioLibP.h>
#include <io/cbioShow.h>

/* Defines */

/* Imports */
IMPORT CBIO_DEV_ID cbioRecentDev;

/* Locals */

/* Globals */

/* Functions */

/*******************************************************************************
 * cbioShowInit - Initialize block i/o show routines
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cbioShowInit(void)
{
  return classShowConnect(cbioClassId, (FUNCPTR) cbioShow);
}

/*******************************************************************************
 * cbioRdyChk - Print ready change
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS cbioRdyChk(CBIO_DEV_ID devId)
{
  if (cbioDevVerify(devId) != OK) {

    printf("\tCBIO_DEV_ID 0x%08lx is invalid per cbioVerify.\n",
	   (unsigned) devId);
    return ERROR;

  }

  printf("\tCBIO_DEV_ID 0x%08lx ready changed is %d.\n",
	 (unsigned) devId, CBIO_READYCHANGED(devId));

  if ( (devId->blkSubDev != NULL) ||
       (devId->cbioSubDev != NULL) ) {

    if (devId->isDriver == TRUE) {

      if (devId->blkSubDev == NULL) {

        return OK;

      }

      else {

        printf("\tBLK_DEV ptr 0x%08lx ready changed is %d.\n",
	       (unsigned) devId->blkSubDev, devId->blkSubDev->bd_readyChanged);

        return OK;

      }

    }

    else {

      return (cbioRdyChk(devId->cbioSubDev));

    }

  }

  return ERROR;

}

/*******************************************************************************
 * cbioShow - Show information about block device
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cbioShow(CBIO_DEV_ID devId)
{
  unsigned long size, factor;
  char str[8], lstr[12];

  /* Initialize locals */
  factor = 1;

  if (devId == NULL)
    devId = cbioRecentDev;

  if (devId == NULL) {

    errnoSet(S_cbioLib_INVALID_DEV_ID);
    return ERROR;

  }

  if (cbioDevVerify(devId) != OK)
    return OK;

  if (devId->cbioParams.bytesPerBlk == 0) {

    errnoSet(EINVAL);
    return ERROR;

  }

  printf("Cached Block I/O Device, handle=%#lx\n",
	 (unsigned) devId);
  printf("\tDescription: %s\n", devId->cbioDesc);

  if (devId->cbioParams.bytesPerBlk < 1024) {

    factor = 1024 / devId->cbioParams.bytesPerBlk;
    size = devId->cbioParams.nBlocks / factor;
    strcpy(str, "Kbytes");

  }

  else {

    factor = devId->cbioParams.bytesPerBlk / 1024;
    size = (devId->cbioParams.nBlocks / 1024) * factor;
    strcpy(str, "Mbytes");

  }

  if ( (size > 10000) &&
       (strcmp(str, "Kbytes") == 0) ) {

    strcpy(str, "Mbytes");
    size /= 1024;

  }

  if ( (size > 10000) &&
       (strcmp(str, "Mbytes") == 0) ) {

    strcpy(str, "Gbytes");
    size /= 1024;

  }

  /* Print sizes */
  printf("\tDisk size %ld %s, RAM Size %d bytes\n",
	 size, str, (int) devId->cbioMemSize);

  printf("\tBlock size %d, heads %d, blocks/track %d, # of blocks %ld\n",
	 (int) devId->cbioParams.bytesPerBlk,
	 devId->cbioParams.nHeads,
	 devId->cbioParams.blocksPerTrack,
         (block_t) devId->cbioParams.nBlocks);

  if (devId->cbioParams.cbioRemovable)
    strcpy(lstr, "Removable");
  else
    strcpy(lstr, "Fixed");

  if (CBIO_READYCHANGED(devId))
    strcpy(str, "Yes");
  else
    strcpy(str, "No");

  printf("\tpartition offset %ld blocks, type %s, Media changed %s\n",
	 (block_t) devId->cbioParams.blockOffset,
	 lstr, str);

  if (devId->cbioParams.lastErrno != 0)
    printf("\tLast error errno=%x, block=%ld\n",
	   devId->cbioParams.lastErrno,
	   devId->cbioParams.lastErrBlk);

  printf("\n   Current ready changed bit for this layer"
         "and all subordinated:\n");
  cbioRdyChk(devId);

  return OK;
}

