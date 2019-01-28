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

/* debugLib.c - Debug library */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/taskInfo.h>
#include <io/ioLib.h>
#include <io/ttyLib.h>
#include <os/excLib.h>
#include <tools/debugLib.h>

/* Defines */

/* Imports */
IMPORT int shellTaskId;

/* Locals */
LOCAL BOOL debugLibInstalled = FALSE;

LOCAL void debugTtyAbort(void);
LOCAL void debugTaskTtyAbort(void);

/* Globals */

/* Functions */

/*******************************************************************************
 * debugLibInit - Initialize debug library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS debugLibInit(void)
{
  /* Check if already installed */
  if (debugLibInstalled)
    return OK;

  /* Install */
  ttyAbortFuncSet( (FUNCPTR) debugTtyAbort );

  /* Mark as installed */
  debugLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * debugTtyAbort - Abort shell with terminal abort key
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void debugTtyAbort(void)
{
  /* Generate exception to restart shell */
  excJobAdd( (VOIDFUNCPTR) debugTaskTtyAbort,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0 );
}

/*******************************************************************************
 * debugTaskTtyAbort - Task shell abort function
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void debugTaskTtyAbort(void)
{
  /* Temprary remove abort function */
  ttyAbortFuncSet( (FUNCPTR) NULL );

  /* Flush standard I/O */
  ioctl(STDIN_FILENO, FIOFLUSH, 0);
  ioctl(STDOUT_FILENO, FIOFLUSH, 0);
  ioctl(STDERR_FILENO, FIOFLUSH, 0);

  /* If shell restart fails */
  if ( taskRestart(shellTaskId) != OK ) {

    fprintf(stderr, "spawning new shell.\n");

    /* Start a new shell */
    if ( shellSpawn(0, (ARG) TRUE) != OK)
      fprintf(stderr, "shell spawn failed.\n");

  } /* End if shell restart fails */

  /* Else task restart succeded */
  else {

    fprintf(stderr, "%s restarted.\n", taskName(shellTaskId) );

  } /* End else task restart succeded */

  /* Restore shell abort function */
  ttyAbortFuncSet( (FUNCPTR) debugTtyAbort );
}

