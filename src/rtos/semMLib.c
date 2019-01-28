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

/* semMLib.c - Mutex sempahore library */

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
IMPORT CLASS_ID semClassId;
IMPORT int errno;

/* Locals */
LOCAL BOOL semMLibInstalled = FALSE;
LOCAL int semMKernGiveWork;
LOCAL STATUS semMKernGive(SEM_ID semId);

/******************************************************************************
* semMLibInit - Initialize mutex semaphore library
*
* RETURNS: OK
******************************************************************************/

STATUS semMLibInit(void)
{
  /* Instatiate class */
  if (semMLibInstalled == TRUE)
    return(OK);

  /* Install call methods */
  semGiveTable[SEM_TYPE_MUTEX] = (FUNCPTR) semMGive;
  semTakeTable[SEM_TYPE_MUTEX] = (FUNCPTR) semMTake;

  /* Mark library as installed */
  semMLibInstalled = TRUE;

  return(OK);
}

/******************************************************************************
* semMCreate - Allocate and init semaphore
*
* RETURNS: SEM_ID
******************************************************************************/

SEM_ID semMCreate(int options)
{
  SEM_ID semId;

  /* Check if lib is installed */
  if (semMLibInstalled != TRUE)
  {
    errnoSet (S_semLib_NOT_INSTALLED);
    return(NULL);
  }

  /* Allocate memory */
  semId = (SEM_ID) objAlloc(semClassId);
  if (semId == NULL)   /* errno set by objAlloc() */
    return(NULL);

  /* Initialze structure */
  if (semMInit(semId, options) != OK)
  {                                 /* errno set by semMInit() */

    objFree(semClassId, semId);
    return(NULL);
  }

  return(semId);
}
  
/******************************************************************************
* semMInit - Init semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semMInit(SEM_ID semId, int options)
{
  /* Check if lib is installed */
  if (!semMLibInstalled)
  {
    errnoSet (S_semLib_NOT_INSTALLED);
    return (ERROR);
  }

  /* Initialize semaphore queue */
  if (semQInit(&semId->qHead, options) != OK)
    return (ERROR);     /* errno set by semQInit() */

  /* Initialize semaphore queue */
  if (semMCoreInit(semId, options) != OK)
    return (ERROR);     /* errno set by semMCoreInit() */

  return(OK);
}

/******************************************************************************
* semMCoreInit - Init semaphore object core
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semMCoreInit(SEM_ID semId, int options)
{
  /* Initialize variables */
  SEM_OWNER_SET(semId, NULL);
  semId->recurse = 0;
  semId->options = options;
  semId->semType = SEM_TYPE_MUTEX;

  /* Initialize events */
  eventResourceInit(&semId->events);

  /* Initialize object core */
  objCoreInit(&semId->objCore, semClassId);

  return(OK);
}

/******************************************************************************
* semMGive - Give up semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semMGive(SEM_ID semId)
{
  int level;
  int kernWork = 0x00;
  TCB_ID pOwner;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  INT_LOCK(level);

  /* Verify object */
  if (OBJ_VERIFY(semId, semClassId) != OK)
  {

    INT_UNLOCK(level);   /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  pOwner = SEM_OWNER_GET(semId);

  /* Check ownership */
  if (taskIdCurrent != pOwner)
  {
    INT_UNLOCK(level);
    errnoSet (S_semLib_NOT_OWNER);
    return(ERROR);
  }

  /* Check recurse */
  if (semId->recurse > 0)
  {
    semId->recurse--;
    INT_UNLOCK(level);
    return(OK);
  }

  /* Update to next pending task */
  SEM_OWNER_SET(semId, Q_FIRST(&semId->qHead));
  pOwner = SEM_OWNER_GET(semId);

  /* Check for more kernel work */
  if (pOwner != NULL)
    kernWork |= SEM_M_Q_GET;
  else if (semId->events.taskId != NULL)
    kernWork |= SEM_M_SEND_EVENTS;

  if ( (semId->options & SEM_DELETE_SAFE) &&
       (--taskIdCurrent->safeCount == 0) &&
       (Q_FIRST(&taskIdCurrent->safetyQ) != NULL) )
    kernWork |= SEM_M_SAFE_Q_FLUSH;

  /* Check if where is any kernel work to be done */
  if (!kernWork)
  {
    INT_UNLOCK(level);
    return(OK);
  }

  kernelState = TRUE;
  INT_UNLOCK(level);
  semMKernGiveWork = kernWork;

  return(semMKernGive(semId));
}

/******************************************************************************
* semMTake - Take hold of semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semMTake(SEM_ID semId, unsigned timeout)
{
  int level;
  TCB_ID pOwner;
  STATUS status = OK;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

semMTakeLoop:

  INT_LOCK(level);

  /* Verify object */
  if (OBJ_VERIFY(semId, semClassId) != OK)
  {

    INT_UNLOCK(level);    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  pOwner = SEM_OWNER_GET(semId);

  /* Take semaphore if free */
  if (pOwner == NULL)
  {
    SEM_OWNER_SET(semId, taskIdCurrent);

    if (semId->options & SEM_DELETE_SAFE)
      taskIdCurrent->safeCount++;

    INT_UNLOCK(level);
    return(OK);
  }

  /* Check recurse */
  if (pOwner == taskIdCurrent)
  {
    semId->recurse++;
    INT_UNLOCK(level);
    return(OK);
  }

  if (timeout == WAIT_NONE)
  {
    errnoSet (S_objLib_UNAVAILABLE);
    return (ERROR);
  }

  /* Do kernel work */
  kernelState = TRUE;
  INT_UNLOCK(level);

  rtosPendQPut (&semId->qHead, timeout);

  status = kernExit();

  if (status == SIG_RESTART)
  {
    goto semMTakeLoop;
  }

  return(status);
}

/******************************************************************************
* semMKernGive - Do kernel stuff for semaphore give
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS semMKernGive(SEM_ID semId)
{
  STATUS eventStatus;
  STATUS returnStatus = OK;
  int oldErrno = errno;
  TCB_ID pOwner;

  /* Check which kernel work should be done */

  /* Get from pending queue */
  if (semMKernGiveWork & SEM_M_Q_GET)
  {
    rtosPendQGet(&semId->qHead);

    /* Get owner */
    pOwner = SEM_OWNER_GET(semId);

    if (semId->options & SEM_DELETE_SAFE)
      pOwner->safeCount++;
  }

  /* Should safe queue be flushed */
  if (semMKernGiveWork & SEM_M_SAFE_Q_FLUSH)
    rtosPendQFlush(&taskIdCurrent->safetyQ);

  /* Should events be sent */
  if (semMKernGiveWork & SEM_M_SEND_EVENTS)
  {
    eventStatus = eventResourceSend(semId->events.taskId,
				    semId->events.registered);

    if (eventStatus != OK)
    {
      if ((semId->options & SEM_EVENTSEND_ERROR_NOTIFY) != 0x00)
      {
        errnoSet (S_eventLib_SEND_ERROR);
        returnStatus = ERROR;
      }

      semId->events.taskId = NULL;
    }
    else if ((semId->events.options & EVENTS_SEND_ONCE) != 0x00)
      semId->events.taskId = NULL;
  }

  if (returnStatus == ERROR)
  {
    kernExit();
  }
  else
  {
    errnoSet(oldErrno);
    returnStatus = kernExit();
  }

  return(returnStatus);
}

/***************************************************************************
 *
 * semMGiveForce - forcibly give a mutex (for debugging only)
 *
 * This routine forcibly releases a mutex semaphore.  It passes ownership to
 * the next in the queue (if any).  
 *
 * RETURNS: OK or ERROR
 */

STATUS semMGiveForce
    (
    SEM_ID  semId      /* mutex semaphore to forcibly give */
    )
    {
    int     level;
    int     kernWork;
    TCB_ID  pOwner;
    TCB_ID  pNewOwner;
    BOOL    taskIsValid;
    STATUS  eventStatus;
    STATUS  returnStatus;
    int     oldErrno = errno;

    if (INT_RESTRICT() != OK)
        {
        errnoSet (S_intLib_NOT_ISR_CALLABLE);
        return(ERROR);
        }

    INT_LOCK (level);    /* Lock interrupts */

    if (OBJ_VERIFY (semId, semClassId) != OK)
        {

        INT_UNLOCK (level);    /* errno set by OBJ_VERIFY() */
        return (ERROR);
        }

    semId->recurse = 0;         /* Quickly undo any recursive "takes". */

    pOwner = semId->semOwner;

    if (taskIdCurrent == pOwner)   /* If the semaphore is owned by  */
        {                          /* the current task, unlock ints */
        INT_UNLOCK (level);        /* and use the standard API to   */
        return (semMGive (semId)); /* give the semaphore.           */
        }

    /*
     * The semaphore is owned by another task.  Before using <pOwner>, verify
     * that it is still a task since it is possible that the task that took
     * this semaphore may no longer exist.
     *
     * Afterwards it will be necessary to mark <kernelState> as TRUE and unlock
     * interrupts.  Since <kernelState> exists to decrease the interrupt
     * latency period, we try to keep queue modifications (operations that
     * take an un-fixed amount of time) in this section.
     */

    taskIsValid = (TASK_ID_VERIFY (pOwner) == OK);

    kernelState = TRUE;
    INT_UNLOCK (level);

    if (taskIsValid)
        {
        if ((semId->options & SEM_DELETE_SAFE) &&
            (--pOwner->safeCount == 0) &&
            (Q_FIRST (&pOwner->safetyQ) != NULL))
            {
            rtosPendQFlush (&pOwner->safetyQ);
            }
        }

    /* Update to next pending task */
    
    pNewOwner = (TCB_ID) Q_FIRST (&semId->qHead);
    if ((semId->semOwner = pNewOwner) != NULL)
        {
        rtosPendQGet (&semId->qHead);

        if (semId->options & SEM_DELETE_SAFE)
            pNewOwner->safeCount++;
        }
    else if (semId->events.taskId != NULL)
        {
        eventStatus = eventResourceSend(semId->events.taskId,
				    semId->events.registered);

        if (eventStatus != OK)
            {
            if ((semId->options & SEM_EVENTSEND_ERROR_NOTIFY) != 0x00)
                {
                errnoSet (S_eventLib_SEND_ERROR);
                returnStatus = ERROR;
                }

            semId->events.taskId = NULL;
            }
        else if ((semId->events.options & EVENTS_SEND_ONCE) != 0x00)
            semId->events.taskId = NULL;
        }   

    if (returnStatus == ERROR)
        {
        kernExit ();
        }
    else
        {
        errnoSet (oldErrno);
        returnStatus = kernExit ();
        }

    return (returnStatus);
    }
