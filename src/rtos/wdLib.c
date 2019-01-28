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

/* wdLib.c - Watchdog library */

/* Includes */
#include <stdlib.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/classLib.h>
#include <rtos/rtosLib.h>
#include <rtos/kernQLib.h>
#include <rtos/taskLib.h>
#include <rtos/memPartLib.h>
#include <rtos/wdLib.h>

/* Defines */

/* Imports */
IMPORT BOOL kernelState;
IMPORT STATUS kernExit(void);

/* Locals */
LOCAL BOOL wdLibInstalled = FALSE;
LOCAL OBJ_CLASS wdClass;

/* Globals */
CLASS_ID wdClassId = &wdClass;

/* Functions */

/*******************************************************************************
 * wdLibInit - Initialize wachdog library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdLibInit(void)
{
  /* Check if installed */
  if (wdLibInstalled)
    return OK;

  /* Initialize class */
  if (classInit(wdClassId, sizeof(WDOG), OFFSET(WDOG, objCore),
		memSysPartId,
		(FUNCPTR) wdCreate,
		(FUNCPTR) wdInit,
		(FUNCPTR) wdDestroy) != OK)
    return ERROR;

  /* Mark as installed */
  wdLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * wdCreate - Create watchdog timer
 *
 * RETURNS: Whatchdog timer id
 ******************************************************************************/

WDOG_ID wdCreate(void)
{
  WDOG_ID wdId;

  if (!wdLibInstalled)
    return NULL;

  /* Allocate struct */
  wdId = (WDOG_ID) objAlloc(wdClassId);
  if (wdId == NULL)
    return NULL;

  /* Initialize object */
  if ( wdInit(wdId) != OK ) {

    objFree(wdClassId, wdId);
    return NULL;

  }

  return wdId;
}

/*******************************************************************************
 * wdInit - Inititalize watchdog timer
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdInit(WDOG_ID wdId)
{
  if (!wdLibInstalled)
    return ERROR;

  wdId->status = WDOG_OUT_OF_Q;
  wdId->dfrStartCount = 0;

  objCoreInit(&wdId->objCore, wdClassId);

  return OK;
}

/*******************************************************************************
 * wdDestroy - Free watchdog timer
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdDestroy(WDOG_ID wdId, BOOL dealloc)
{
  int level;

  /* Not callable from interrupts */
  if ( INT_RESTRICT() != OK )
    return ERROR;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify object */
  if ( OBJ_VERIFY(wdId, wdClassId) != OK ) {

    INT_UNLOCK(level);
    return ERROR;

  }

  /* Terminate object */
  objCoreTerminate(&wdId->objCore);

  /* Enter kernel */
  kernelState = TRUE;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  /* Cancel watchdog timer */
  rtosWdCancel(wdId);
  wdId->status = WDOG_DEAD;

  taskSafe();

  /* Exit kernel */
  kernExit();

  /* Deallocate if requested */
  if (dealloc)
    objFree(wdClassId, wdId);

  taskUnsafe();

  return OK;
}

/*******************************************************************************
 * wdDelete - Terminate and deallocate watchdog timer
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdDelete(WDOG_ID wdId)
{
  return wdDestroy(wdId, TRUE);
}

/*******************************************************************************
 * wdTerminate - Terminate watchdog timer
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdTerminate(WDOG_ID wdId)
{
  return wdDestroy(wdId, FALSE);
}

/*******************************************************************************
 * wdStart - Start watchdog timer
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdStart(WDOG_ID wdId, int delay, FUNCPTR func, ARG arg)
{
  int level;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify object */
  if ( OBJ_VERIFY(wdId, wdClassId) != OK ) {

    INT_UNLOCK(level);
    return ERROR;

  }

  /* If in kernel mode */
  if (kernelState) {

    wdId->dfrStartCount++;
    wdId->wdFunc = func;
    wdId->wdArg = arg;

    /* Unlock interrupts */
    INT_UNLOCK(level);

    /* Add to kernel queue */
    kernQAdd2((FUNCPTR) rtosWdStart, (ARG) wdId, (ARG) delay);

  }

  /* Else not in kernel mode */
  else {

    wdId->dfrStartCount++;
    wdId->wdFunc = func;
    wdId->wdArg = arg;

    /* Enter kernel */
    kernelState = TRUE;

    /* Unlock interrupts */
    INT_UNLOCK(level);

    /* Start watchdog timer */
    if ( rtosWdStart(wdId, delay) != OK ) {

      kernExit();
      return ERROR;

    }

    /* Exit kernel */
    kernExit();

  }

  return OK;
}

/*******************************************************************************
 * wdCancel - Cancel watchdog timer
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS wdCancel(WDOG_ID wdId)
{
  int level;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify object */
  if ( OBJ_VERIFY(wdId, wdClassId) != OK ) {

    INT_UNLOCK(level);
    return ERROR;

  }

  /* If in kernel mode */
  if (kernelState) {

    /* Unlock interrupts */
    INT_UNLOCK(level);

    /* Add to kernel queue */
    kernQAdd1((FUNCPTR) rtosWdCancel, (ARG) wdId);

  }

  /* Else not in kernel mode */
  else {

    /* Enter kernel */
    kernelState = TRUE;

    /* Unlock interrupts */
    INT_UNLOCK(level);

    rtosWdCancel(wdId);

    /* Exit kernel */
    kernExit();

  }

  return OK;
}

