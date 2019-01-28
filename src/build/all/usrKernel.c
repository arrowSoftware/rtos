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

/* usrKernel.c - User kernel initialize */

/* Includes */
#include <rtos.h>
#include <util/classLib.h>
#include <util/classShow.h>
#include <util/qLib.h>
#include <util/qFifoLib.h>
#include <util/qPrioLib.h>
#include <util/qPriBmpLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/taskHookShow.h>
#include <rtos/semLib.h>
#include <rtos/semShow.h>
#include <rtos/eventLib.h>
#include <rtos/msgQLib.h>
#include <rtos/msgQShow.h>
#include <rtos/taskShow.h>
#include <rtos/kernQLib.h>
#include <rtos/kernTickLib.h>

#include "config.h"

/* Defines */

/* Imports */
IMPORT Q_HEAD kernActiveQ;
IMPORT Q_HEAD kernTickQ;
IMPORT Q_HEAD kernReadyQ;

/* Locals */

/* Globals */
#ifdef INCLUDE_CONSTANT_RDY_Q
DL_LIST kernReadyLst[256];
unsigned kernReadyBmp[8];
#endif /* INCLUDE_CONSTANT_RDY_Q */

/* Functions */

/*******************************************************************************
 * userKernelInit - Initialize user kernel
 *
 * RETURNS: N/A
 ******************************************************************************/

void usrKernelInit(void)
{
  classLibInit();
  taskLibInit();

#ifdef INCLUDE_TASK_HOOKS

  taskHookLibInit();

#ifdef INCLUDE_SHOW_ROUTINES

  taskHookShowInit();

#endif /* INCLUDE_SHOW_ROUTINES */

#endif /* INCLUDE_TASK_HOOK */

#ifdef INCLUDE_SEM_BINARY

  semLibInit();
  semBLibInit();

#endif /* INCLUDE_SEM_BINARY */

#ifdef INCLUDE_SEM_MUTEX

  semLibInit();
  semMLibInit();

#endif /* INCLUDE_SEM_MUTEX */

#ifdef INCLUDE_SEM_COUNTING

  semLibInit();
  semCLibInit();

#endif /* INCLUDE_SEM_COUNTING */

#ifdef INCLUDE_SEM_RW

  semLibInit();
  semRWLibInit();

#endif /* INCLUDE_SEM_RW */

#ifdef INCLUDE_rtos_EVENTS

  eventLibInit();

#endif /* INCLUDE_rtos_EVENTS */

#ifdef INCLUDE_MSG_Q

  msgQLibInit();

#ifdef INCLUDE_SHOW_ROUTINES

  msgQShowInit();

#endif /* INCLUDE_SHOW_ROUTINES */

#endif /* INCLUDE_MSG_Q */

#ifdef INCLUDE_SHOW_ROUTINES

  classShowInit();
  taskShowInit();
  semShowInit();

#endif /* INCLUDE_SHOW_ROUTINES */

  /* Initialize kernel queues */
  qInit(&kernActiveQ, qFifoClassId);
  qInit(&kernTickQ, qPrioClassId);

#ifdef INCLUDE_CONSTANT_RDY_Q
  qInit(&kernReadyQ, qPriBmpClassId, 256, kernReadyLst, kernReadyBmp);
#else
  qInit(&kernReadyQ, qPrioClassId);
#endif /* INCLUDE_CONSTANT_RDY_Q */

  /* Initialize kernel work queue */
  kernQLibInit();

  /* Initialize kernel clock to zero */
  kernTickLibInit();
}

