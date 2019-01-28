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

/* schedLib.c - Portable scheduling library */

#include <sys/types.h>
#include <rtos.h>
#include <rtos/kernQLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <util/qLib.h>

#ifdef schedLib_PORTABLE

/* Imports */
IMPORT Q_HEAD kernReadyQ;
IMPORT TCB_ID taskIdCurrent;
IMPORT BOOL kernelState;
IMPORT volatile BOOL kernQEmpty;
IMPORT FUNCPTR taskSwitchHooks[];
IMPORT FUNCPTR taskSwapHooks[];

IMPORT void kernTaskLoadContext(void);

/******************************************************************************
* kernTaskReschedule - Reshedule running task
*
* RETURNS: N/A
******************************************************************************/

void kernTaskReschedule(void)
{
  TCB_ID taskIdPrevious;
  int i, level;
  u_int16_t mask;

kernTaskRescheduleLoop:

  /* Store current task */
  taskIdPrevious = taskIdCurrent;

  /* Do kernel work fist */
  kernQDoWork();

  /* Idle loop */
  while (Q_FIRST(&kernReadyQ) == NULL) {

    /* Do kernel work if needed */
    kernQDoWork();

    /* SHOULD ENABLE INTERRPUTS HERE */

  } /* End idle loop */

  /* Fall tru if there are any ready tasks */

  /* Get next ready task */
  taskIdCurrent = (TCB_ID) Q_FIRST(&kernReadyQ);

  /* Do switch/swap hooks */
  if (taskIdCurrent != taskIdPrevious) {

    /* Swap hooks */
    mask = taskIdCurrent->swapInMask | taskIdPrevious->swapOutMask;
    for (i = 0; mask != 0; i++, mask = mask << 1) {
      if (mask & 0x8000)
        (*taskSwapHooks[i]) (taskIdPrevious, taskIdCurrent);

    } /* End swap hooks */

    /* Switch hooks */
    for (i = 0;
	 i < (MAX_KERNEL_SWITCH_HOOKS) && (taskSwitchHooks[i] != NULL);
	 ++i) {
      (*taskSwitchHooks[i]) (taskIdPrevious, taskIdCurrent);

    } /* End switch hooks */

  } /* End to swap/switch hooks */

  /* Lock interrupts */
  INT_LOCK(level);

  /* Check if kernel work has been added */
  if (!kernQEmpty) {

    /* Lock interrupts */
    INT_UNLOCK(level);
    goto kernTaskRescheduleLoop;

  } /* End kernel work got added */

  else {

    /* Exit kernel mode */
    kernelState = FALSE;

    /* Switch context */
    kernTaskLoadContext();
  } /* End context switch */

}

#endif /* schedLib_PORTABLE */

