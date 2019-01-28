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

/* ramDrv.c - Ramdisk driver */

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <rtos.h>
#include <io/blkIo.h>
#include <io/ioLib.h>
#include <drv/disk/ramDrv.h>

/* Locals */
LOCAL STATUS ramBlkRd(RAM_DEV *pRamDev,
		      int startBlk,
		      int numBlks,
		      char *pData);
LOCAL STATUS ramBlkWrt(RAM_DEV *pRamDev,
		       int startBlk,
		       int numBlks,
		       char *pData);
LOCAL STATUS ramIoctl(RAM_DEV *pRamDev,
		      int func,
		      int arg);

/*******************************************************************************
 * ramDevCreate - Create ramdisk device
 *
 * RETURNS: Ramdisk block-device
 ******************************************************************************/

BLK_DEV *ramDevCreate(char *ramAddr,
		      int bytesPerBlk,
		      int blksPerTrack,
		      int nBlocks,
		      int blkOffset)
{
  RAM_DEV *pRamDev;
  BLK_DEV *pBlkDev;

  /* Check value */
  if (bytesPerBlk <= 0)
    bytesPerBlk = RAMDRV_DEFAULT_SEC_SIZE;

  if (nBlocks <= 0)
    nBlocks = RAMDRV_DEFAULT_DISK_SIZE / bytesPerBlk;

  if (blksPerTrack <= 0)
    blksPerTrack = nBlocks;

  /* Allocate structure */
  pRamDev = (RAM_DEV *) malloc(sizeof(RAM_DEV));
  if (pRamDev == NULL)
    return NULL;

  /* Initialize device */
  pRamDev->blkOffset = blkOffset;

  pBlkDev = &pRamDev->blkDev;

  /* Initialize block device */
  pBlkDev->bd_bytesPerBlk = bytesPerBlk;
  pBlkDev->bd_nBlocks = nBlocks;
  pBlkDev->bd_blksPerTrack = blksPerTrack;
  pBlkDev->bd_nHeads = 1;
  pBlkDev->bd_removable = FALSE;
  pBlkDev->bd_retry = 1;
  pBlkDev->bd_mode = O_RDWR;
  pBlkDev->bd_readyChanged = TRUE;
  pBlkDev->bd_blkRd = (FUNCPTR) ramBlkRd;
  pBlkDev->bd_blkWrt = (FUNCPTR) ramBlkWrt;
  pBlkDev->bd_ioctl = (FUNCPTR) ramIoctl;
  pBlkDev->bd_reset = (FUNCPTR) NULL;
  pBlkDev->bd_statusChk = NULL;

  /* Initialize memory and allocate memory if needed */
  if (ramAddr == NULL) {
    pRamDev->ramAddr = (char *) malloc(bytesPerBlk * nBlocks);
    if (pRamDev->ramAddr == NULL) {
      free(pRamDev);
      return NULL;
    }
  }
  else
    pRamDev->ramAddr = ramAddr;

  return &pRamDev->blkDev;
}

/*******************************************************************************
 * ramBlkRd - Read block
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS ramBlkRd(RAM_DEV *pRamDev,
		      int startBlk,
		      int numBlks,
		      char *pData)
{
  int bytesPerBlk;

  /* Calculate bytes per block */
  bytesPerBlk = pRamDev->blkDev.bd_bytesPerBlk;

  /* Add block offset to start block */
  startBlk += pRamDev->blkOffset;

  /* Write data */
  memcpy(pData,
	 pRamDev->ramAddr + (startBlk * bytesPerBlk),
	 bytesPerBlk * numBlks);

  return OK;
}

/*******************************************************************************
 * ramBlkWrt - Write block
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS ramBlkWrt(RAM_DEV *pRamDev,
		       int startBlk,
		       int numBlks,
		       char *pData)
{
  int bytesPerBlk;

  /* Calculate bytes per block */
  bytesPerBlk = pRamDev->blkDev.bd_bytesPerBlk;

  /* Add block offset to start block */
  startBlk += pRamDev->blkOffset;

  /* Write data */
  memcpy(pRamDev->ramAddr + (startBlk * bytesPerBlk),
	 pData,
	 bytesPerBlk * numBlks);

  return OK;
}

/*******************************************************************************
 * ramBlkIoctl - Control
 *
 * RETURNS:
 ******************************************************************************/

LOCAL STATUS ramIoctl(RAM_DEV *pRamDev,
		      int func,
		      int arg)
{
  switch(func) {

    case FIODISKFORMAT:
    memset(pRamDev->ramAddr, 0,
	pRamDev->blkDev.bd_bytesPerBlk * pRamDev->blkDev.bd_nBlocks);
    break;

    default: return ERROR;
  }

  return OK;
}

