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

/* pcConsole.c - PC console */

#include <sys/types.h>
#include <rtos.h>
#include <io/iosLib.h>
#include <io/ioLib.h>
#include <io/ttyLib.h>
#include <drv/input/i8042Kbd.h>
#include <drv/video/m6845Vga.h>
#include <drv/serial/pcConsole.h>
#include <stdlib.h>

/* Locals */
LOCAL int pcNumber = 0;

LOCAL void pcConDrvHrdInit(void);
LOCAL int pcConDrvOpen(PC_CON_DEV *pc, char *name, int mode);
LOCAL STATUS pcConDrvIoctl(PC_CON_DEV *pc, int req, int arg);

/* Imports */
IMPORT int vgaWriteString(PC_CON_DEV *pc);

/*******************************************************************************
* pcConDrvInit - Initialize console
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS pcConDrvInit(void)
{
  if (pcNumber > 0)
    return(OK);

  pcConDrvHrdInit();

  /* Install driver */
  pcNumber = iosDrvInstall(pcConDrvOpen,
			   (FUNCPTR) NULL,
			   pcConDrvOpen,
			   (FUNCPTR) NULL,
			   ttyRead,
			   ttyWrite,
			   pcConDrvIoctl);

  if (pcNumber == ERROR)
    return(ERROR);

  return(OK);
}

/*******************************************************************************
* pcConDevCreate - Create device
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS pcConDevCreate(char *name,
		      int channel,
		      int readBufferSize,
		      int writeBufferSize)
{
  PC_CON_DEV *pc;

  if (pcNumber <= 0)
    return(ERROR);

  if (channel < 0  || channel >= N_VIRTUAL_CONSOLES)
    return(ERROR);

  pc = &pcConDev[channel];

  if (pc->created)
    return(ERROR);

  if (ttyDevInit(&pc->ttyDev, readBufferSize, writeBufferSize,
		 vgaWriteString) != OK)
    return(ERROR);

  /* Enable interrupt level for keyboard */
  sysIntEnablePIC(KBD_INT_LVL);

  pc->created = TRUE;

  return(iosDevAdd(&pc->ttyDev.devHeader, name, pcNumber));
}

/*******************************************************************************
* pcConDrvNumber - Get console number
*
* RETURNS: Console number
*******************************************************************************/

int pcConDrvNumber(void)
{
  return(pcNumber);
}

/*******************************************************************************
* pcConDrvHrdInit - Initalize hardware
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void pcConDrvHrdInit(void)
{
  int level;

  INT_LOCK(level);

  kbdHrdInit();
  vgaHrdInit();

  INT_UNLOCK(level);
}

/*******************************************************************************
* pcConDrvOpen - Open a file to console
*
* RETURNS: Device number
*******************************************************************************/

LOCAL int pcConDrvOpen(PC_CON_DEV *pc, char *name, int mode)
{
  return((int) pc);
}

/*******************************************************************************
* pcConDrvIoctl - I/O control
*
* RETURNS: OR or ERROR
*******************************************************************************/

LOCAL STATUS pcConDrvIoctl(PC_CON_DEV *pc, int req, int arg)
{
  STATUS status = OK;

  switch(req) {
    default: status = ttyIoctl(&pc->ttyDev, req, arg); break;
  }

  return(status);
}

