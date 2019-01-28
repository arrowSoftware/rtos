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

/* semBLib.c - Binary sempahore library */

#include <rtos.h>
#include <arch/intArchLib.h>
#include <stdlib.h>

#include <rtos/errnoLib.h>
#include <rtos/semLib.h>
#include <arch/intArchLib.h>
#include <util/classLib.h>
#include <rtos/taskLib.h>
#include <rtos/rtosLib.h>
#include <rtos/eventLib.h>
#include <os/sigLib.h>

/* Imports */
IMPORT BOOL kernelState;
IMPORT STATUS kernExit(void);
IMPORT TCB_ID taskIdCurrent;
IMPORT CLASS_ID	semClassId;
IMPORT FUNCPTR semFlushTable[];
IMPORT FUNCPTR semGiveDeferTable[];
IMPORT FUNCPTR semFlushDeferTable[];
IMPORT int errno;

/* Locals */
LOCAL BOOL semBLibInstalled = FALSE;

/******************************************************************************
* semBLibInit - Initialize binary semaphore library
*
* RETURNS: OK
******************************************************************************/

STATUS semBLibInit(void)
{
  /* Instatiate class */
  if (semBLibInstalled)
    return(OK);

  /* Install call methods */
  semGiveTable[SEM_TYPE_BINARY] = (FUNCPTR) semBGive;
  semTakeTable[SEM_TYPE_BINARY] = (FUNCPTR) semBTake;
  semFlushTable[SEM_TYPE_BINARY] = (FUNCPTR) semQFlush;
  semGiveDeferTable[SEM_TYPE_BINARY] = (FUNCPTR) semBGiveDefer;
  semFlushDeferTable[SEM_TYPE_BINARY] = (FUNCPTR) semQFlushDefer;

  /* Mark library as installed */
  semBLibInstalled = TRUE;

  return(OK);
}

/******************************************************************************
* semBCreate - Allocate and init semaphore
*
* RETURNS: SEM_ID
******************************************************************************/

SEM_ID semBCreate(int options, SEM_B_STATE state)
{
  SEM_ID semId;

  /* Check if lib is installed */
  if (semBLibInstalled != TRUE)
  {
    errnoSet (S_semLib_NOT_INSTALLED);
    return(NULL);
  }

  /* Allocate memory */
  semId = (SEM_ID) objAlloc(semClassId);
  if (semId == NULL)
  {
    /* errno handled by objAlloc */
    return(NULL);
  }

  /* Initialze structure */
  if (semBInit(semId, options, state) != OK)
  {                                  /* errno set by semBInit() */

    objFree(semClassId, semId);
    return(NULL);
  }

  return(semId);
}
  
/******************************************************************************
* semBInit - Init semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semBInit(SEM_ID semId, int options, SEM_B_STATE state)
{
  /* Check if lib is installed */
  if (semBLibInstalled != TRUE)
  {
    errnoSet (S_semLib_NOT_INSTALLED);
    return(ERROR);
  }

  if (options & SEM_DELETE_SAFE)
  {
    errnoSet (S_semLib_INVALID_OPTION);
    return(ERROR);
  }

  /* Initialize semaphore queue */
  if (semQInit(&semId->qHead, options) != OK)
    return(ERROR);     /* errno set by semQInit() */

  /* Initialize semaphore queue */
  if (semBCoreInit(semId, options, state) != OK)
    return(ERROR);     /* errno set by semBCoreInit() */

  return(OK);
}

/******************************************************************************
* semBCoreInit - Init semaphore object core
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semBCoreInit(SEM_ID semId, int options, SEM_B_STATE state)
{
  if (options & (SEM_DELETE_SAFE | SEM_INVERSION_SAFE))
  {
      errnoSet (S_semLib_INVALID_OPTION);
      return (ERROR);
  }

  /* Setup state */
  switch(state)
  {
    case SEM_EMPTY:
      SEM_OWNER_SET(semId, taskIdCurrent);
      break;

    case SEM_FULL:
      SEM_OWNER_SET(semId, NULL);
      break;

    default: 
      errnoSet (S_semLib_INVALID_OPTION);
      return(ERROR);
  }

  /* Initialize variables */
  semId->recurse = 0;
  semId->options = options;
  semId->semType = SEM_TYPE_BINARY;

  /* Initialize events */
  eventResourceInit(&semId->events);

  /* Initialize object core */
  objCoreInit(&semId->objCore, semClassId);

  return(OK);
}

/******************************************************************************
* semBGive - Give up semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semBGive(SEM_ID semId)
{
  int level;
  TCB_ID ownerId;
  int oldErrno;
  STATUS eventStatus, returnStatus = OK;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify class */
  if (OBJ_VERIFY(semId, semClassId) != OK)
  {

    INT_UNLOCK(level);    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Get owner */
  ownerId = SEM_OWNER_GET(semId);

  /* Get next listening task from queue */
  SEM_OWNER_SET(semId, Q_FIRST(&semId->qHead));

  /* Check if no more tasks are waiting for this semaphore */
  if (SEM_OWNER_GET(semId) == NULL)
  {
    /* Check if any events should be sent */
    if ((semId->events.taskId != NULL) && (ownerId != NULL))
    {

      /* Enter kernel mode */
      kernelState = TRUE;

      /* Unlock interrupts */
      INT_UNLOCK(level);

      /* Store old errno */
      oldErrno = errno;

      /* No do the events sending */
      eventStatus = eventResourceSend(semId->events.taskId,
				      semId->events.registered);

      /* Check if it was sucessfull */
      if (eventStatus != OK)
      {

        /* Check if error should be returned */
        if ((semId->options & SEM_EVENTSEND_ERROR_NOTIFY) != 0x00)
        {
	  errnoSet (S_eventLib_SEND_ERROR);
          returnStatus = ERROR;
	}

        /* Stop listening for this event */
        semId->events.taskId = NULL;
      }
      else if ((semId->events.options & EVENTS_SEND_ONCE) != 0x00)
        semId->events.taskId = NULL;

      /* Exit trough kernel */
      kernExit();

      /* Restore errno and return */
      errno = oldErrno;
      return(returnStatus);
    }
    else
    {
      /* Don't send events */
      INT_UNLOCK(level);
    }
  } 
  else
  {
    /* Enter kernel mode */
    kernelState = TRUE;
    INT_UNLOCK(level);

    /* Unblock next task waiting */
    rtosPendQGet(&semId->qHead);

    /* Exit kernel mode */
    kernExit();
  }

  return(OK);
}

/******************************************************************************
* semBTake - Take hold of semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semBTake(SEM_ID semId, unsigned timeout)
{
  int level;
  STATUS status;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

semBTakeLoop:

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify class */
  if (OBJ_VERIFY(semId, semClassId) != OK)
  {

    INT_UNLOCK(level);    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Check if it is already given back */
  if (SEM_OWNER_GET(semId) == NULL)
  {
    /* Then take it */
    SEM_OWNER_SET(semId, taskIdCurrent);

    /* Unlock interrupts */
    INT_UNLOCK(level);

    return(OK);
  }
  
  if (timeout == WAIT_NONE)
  {
    INT_UNLOCK (level);
    errnoSet (S_objLib_UNAVAILABLE);
    return (ERROR);
  }

  /* Enter kernel mode */
  kernelState = TRUE;
  INT_UNLOCK(level);

  /* Put on pending queue */
  rtosPendQPut(&semId->qHead, timeout);

  /* Exit through kernel */
  status = kernExit();

  if (status == SIG_RESTART)
  {
    goto semBTakeLoop;
  }

  return(status);
}

/******************************************************************************
* semBGiveDefer - Give semaphore as defered work
*
* RETURNS: N/A
******************************************************************************/

void semBGiveDefer(SEM_ID semId)
{
  TCB_ID pOwner;

  /* Verify class */
  if (OBJ_VERIFY(semId, semClassId) != OK)
    return;

  /* Get task id */
  pOwner = SEM_OWNER_GET(semId);

  /* Set to next owner */
  SEM_OWNER_SET(semId, Q_FIRST(&semId->qHead));

  /* Check if it exists */
  if (SEM_OWNER_GET(semId) != NULL)
  {
    rtosPendQGet(&semId->qHead);
  }
  else
  {
    if ((semId->events.taskId != NULL) && (pOwner != NULL))
    {
      if (eventResourceSend(semId->events.taskId,
			    semId->events.registered) != OK)
        semId->events.taskId = NULL;
      else if ((semId->events.options & EVENTS_SEND_ONCE) != 0x00)
        semId->events.taskId = NULL;
    }
  }
}
