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

/* sysLib.c - System dependent C code */

#include "config.h"

#include <sys/types.h>
#include <rtos.h>

#include <arch/sh/intArchLib.c>
#include <arch/sh/excArchLib.c>
#include <arch/sh/taskArchLib.c>

/* Kernel */
#include <rtos/rtosLib.c>
#include <rtos/taskLib.c>
#include <rtos/schedLib.c>
#include <rtos/kernTickLib.c>
#include <rtos/kernQLib.c>
#include <rtos/kernHookLib.c>
#include <rtos/kernLib.c>

/* Os */
#include <rtos/errnoLib.c>
#include <rtos/logLib.c>
#include <rtos/listLib.c>
#include <rtos/ringLib.c>
#include <util/objLib.c>
#include <util/classLib.c>
#include <rtos/memPartitionLib.c>
#include <rtos/qLib.c>
#include <rtos/qPrioLib.c>
#include <rtos/qFifoLib.c>
#include <rtos/qMsgLib.c>
#include <rtos/eventLib.c>
#include <rtos/semLib.c>
#include <rtos/semBLib.c>
#include <rtos/semMLib.c>
#include <rtos/semCLib.c>
#include <rtos/semEvLib.c>
#include <rtos/msgQLib.c>
#include <rtos/msgQEvLib.c>
#include <rtos/ioLib.c>
#include <rtos/iosLib.c>
#include <rtos/selectLib.c>
#include <rtos/ttyLib.c>

/* New kernel */
#include <vx2/dlist.c>

#ifdef	 INCLUDE_VIDEO
#include <drv/video/pvr.c>
#include <drv/bios/dreamcast/biosfont.c>
#include <drv/output/dreamcast/dcScrText.c>
//#include <drv/output/dreamcast/console.c>
#endif

#include <drv/timer/dreamcast/timer.c>

#ifdef   INCLUDE_LIBC
//#include <libc/stdio/stdio.c>
#include <libc/stdlib/stdlib.c>
#include <libc/string/string.c>
#endif

/* Globals */
u_int32_t intEvtAdrs = INTEVT_ADRS;
//DEV_HEADER devStdout, devStderr;

/* Imports */
IMPORT void kernVMATable();

/******************************************************************************
* sysHwInit0 - Initialize hardware , stage 0
*
* RETURNS: N/A
******************************************************************************/

void sysHwInit0(void)
{
#ifdef	 INCLUDE_VIDEO
  dcScrTextInit();
#endif
  intVecBaseSet((FUNCPTR *) kernVMATable);
}

/******************************************************************************
* sysHwInit1 - Initialize hardware, stage 1
*
* RETURNS: N/A
******************************************************************************/
void sysHwInit1(void)
{
  int i;

  //taskLock();
#ifdef	 INCLUDE_VIDEO
  /* Init stdout */
  /*
  i = iosDrvInstall(consoleCreate,
		    NULL,
		    consoleOpen,
		    NULL,
		    NULL,
		    consoleWrite,
		    NULL);
  iosDevAdd(&devStdout, "/dev/stdout", i);
  i = iosFdNew(&devStdout, "/dev/stdout", 0);
  ioGlobalStdSet(1, i);
  iosCreate(&devStdout, "/dev/stdout", 0);
  iosOpen(&devStdout, "/dev/stdout", 0, 0);
  */
#endif

  //taskUnlock();
}

