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

/* console.c - Dremcast console */

#include <string.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <io/ioLib.h>
#include <io/iosLib.h>
#include <drv/output/dreamcast/dcScrText.h>
#include <drv/output/dreamcast/console.h>

/* LOCALS */
LOCAL SEMAPHORE consoleSem;

STATUS consoleCreate(DEV_HEADER *pDevHeader,
		     char *filename,
		     int mode)
{
  semBInit(&consoleSem, SEM_Q_PRIORITY, SEM_FULL);
  dcScrTextInit();
}

STATUS consoleOpen(DEV_HEADER *pDevHeader,
		   char *filename,
		   int flags,
		   int mode)
{
  dcScrTextClear();
}

int consoleWrite(int value,
		 void *buffer,
		 int nBytes)
{
  int i;
  char *pChar = (char *) buffer;

  semTake(&consoleSem, WAIT_FOREVER);
  for (i = 0; i < nBytes; i++)
    dcScrTextPutch(pChar[i]);
  semGive(&consoleSem);

  return (i);
}

