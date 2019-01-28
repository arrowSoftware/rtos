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

/* semLib.c - Sempahore library */

#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/objLib.h>
#include <util/classLib.h>
#include <util/qLib.h>
#include <util/qFifoLib.h>
#include <util/qPrioLib.h>
#include <rtos/kernQLib.h>
#include <rtos/rtosLib.h>
#include <rtos/taskLib.h>
#include <rtos/memPartLib.h>
#include <rtos/semLib.h>
#include <stdlib.h>

/* Imports */
IMPORT BOOL kernInitialized;
IMPORT BOOL kernelState;
IMPORT STATUS kernExit(void);

/* Locals */
LOCAL BOOL semLibInstalled = FALSE;
LOCAL STATUS semFlushDefer(SEM_ID semId);

/* forward declarations */

LOCAL STATUS semGiveDefer(SEM_ID semId);

/* Internals */
OBJ_CLASS	semClass;

FUNCPTR semGiveTable [MAX_SEM_TYPE] =
{
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid
};

FUNCPTR semTakeTable [MAX_SEM_TYPE] =
{
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid
};

FUNCPTR semFlushTable [MAX_SEM_TYPE] =
{
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid
};

FUNCPTR semGiveDeferTable [MAX_SEM_TYPE] =
{
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid
};

FUNCPTR semFlushDeferTable [MAX_SEM_TYPE] =
{
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid,
  (FUNCPTR) semInvalid
};

/* Externals */
CLASS_ID	semClassId = &semClass;

/******************************************************************************
* semLibInit - Initialize semaphore library
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semLibInit(void)
{
  /* Instatiate class */
  if (semLibInstalled == TRUE)
    return(OK);

  if (classInit(semClassId,
		sizeof(SEMAPHORE),
		OFFSET(SEMAPHORE, objCore),
		memSysPartId,
		(FUNCPTR) NULL,
		(FUNCPTR) NULL,
		(FUNCPTR) NULL) != OK)
  {
      return(ERROR);
  }

  semLibInstalled = TRUE;

  return(OK);
}

/******************************************************************************
* semCreate - Create a semaphore
*
* RETURNS: Semaphore Id
******************************************************************************/

SEM_ID semCreate(int type, int options)
{
  SEM_ID semId;

  /* Instatiate class */
  if (semLibInstalled == FALSE)
  {
    errnoSet (S_semLib_NOT_INSTALLED);
    return(NULL);
  }

  /* Create semaphore of correct type */
  switch(type & SEM_TYPE_MASK)
  {
    case SEM_TYPE_BINARY:   semId = semBCreate(options, SEM_FULL);
			    break;
    case SEM_TYPE_MUTEX:    semId = semMCreate(options);
			    break;
    case SEM_TYPE_COUNTING: semId = semCCreate(options, 1);
			    break;
    default:		    semId = NULL;
                            
                            errnoSet (S_semLib_INVALID_OPTION);
			    break;
  }

  return(semId);
}

/******************************************************************************
* semInit - Initialize a semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semInit(SEM_ID semId, int type, int options)
{
  STATUS status;

  /* Instatiate class */
  if (semLibInstalled == FALSE)
  {
      errnoSet (S_semLib_NOT_INSTALLED);
    return(ERROR);
  }

  /* Create semaphore of correct type */
  switch(type & SEM_TYPE_MASK)
  {
    case SEM_TYPE_BINARY:   status = semBInit(semId, options, SEM_FULL);
			    break;
    case SEM_TYPE_MUTEX:    status = semMInit(semId, options);
			    break;
    case SEM_TYPE_COUNTING: status = semCInit(semId, options, 1);
			    break;
    default:		    status = ERROR;
      			    errnoSet (S_semLib_INVALID_OPTION);
			    break;
  }

  return(status);
}

/******************************************************************************
* semGive - Give up semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semGive(SEM_ID semId)
{
  /* Check kernel is running */
  if (kernInitialized == FALSE)
    return(ERROR);

  if (kernelState)
    return(semGiveDefer(semId));

  return( (*semGiveTable[semId->semType & SEM_TYPE_MASK]) (semId) );
}

/******************************************************************************
* semTake - Take hold of semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semTake(SEM_ID semId, unsigned timeout)
{
  /* Check kernel is running */
  if (kernInitialized == FALSE)
    return(ERROR);

  return( (*semTakeTable[semId->semType & SEM_TYPE_MASK]) (semId, timeout) );
}

/******************************************************************************
* semFlush - Flush all tasks depending on semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semFlush(SEM_ID semId)
{
  /* Check kernel is running */
  if (kernInitialized == FALSE)
    return(ERROR);

  if (kernelState)
    return(semFlushDefer(semId));

  return( (*semFlushTable[semId->semType & SEM_TYPE_MASK]) (semId) );
}

/******************************************************************************
* semGiveDefer - Give sempahore defered
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS semGiveDefer(SEM_ID semId)
{
  /* Verify object */
  if (OBJ_VERIFY(semId, semClassId) != OK)
    return (ERROR);

  /* A method is guaranteed to exist. */

  kernQAdd1(semGiveDeferTable[semId->semType & SEM_TYPE_MASK], semId);

  return(OK);
}

/******************************************************************************
* semFlushDefer - Flush all tasks depending on semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS semFlushDefer(SEM_ID semId)
{
  /* Verify object */
  if (OBJ_VERIFY(semId, semClassId))
    return (ERROR);

  /* A method is guaranteed to exist */

  kernQAdd1(semFlushDeferTable[semId->semType & SEM_TYPE_MASK], semId);

  return(OK);
}

/******************************************************************************
* semDelete - Delete semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semDelete(SEM_ID semId)
{
  return( semDestroy(semId, TRUE) );
}

/******************************************************************************
* semTerminate - Terminate semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semTerminate(SEM_ID semId)
{
  return( semDestroy(semId, FALSE) );
}

/******************************************************************************
* semDestroy - Destroy semaphore
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semDestroy(SEM_ID semId, BOOL deallocate)
{
  int level;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  INT_LOCK(level);

  if (OBJ_VERIFY (semId, semClassId) != OK)
  {
    INT_UNLOCK (level);
    return (ERROR);
  }

  objCoreTerminate(&semId->objCore);

  /* Delete it */
  kernelState = TRUE;
  INT_UNLOCK(level);
  rtosSemDelete(semId);
  eventResourceTerminate(&semId->events);

  taskSafe();
  kernExit();

  if (deallocate)
    objFree(semClassId, semId);

  taskUnsafe();

  return(OK);
}

/******************************************************************************
* semInvalid - Invalid semaphore function
*
* RETURNS: ERROR
******************************************************************************/

STATUS semInvalid(SEM_ID semId)
{
  return(ERROR);
}

/******************************************************************************
* semQInit - Initialize semaphore queue
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semQInit(Q_HEAD * pQHead, int options)
{
  /* Initilaize queue according to options */
  switch (options & SEM_Q_MASK)
  {
    case SEM_Q_FIFO:
      qInit(pQHead, qFifoClassId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      break;

    case SEM_Q_PRIORITY:
      qInit(pQHead, qPrioClassId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      break;

    default:
      errnoSet (S_semLib_INVALID_Q_TYPE);
      return(ERROR);
  }

  return(OK);
}

/******************************************************************************
* semQFlush - Flush semaphore queue
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS semQFlush(SEM_ID semId)
{
  int level;

  /* Check kernel is running */
  if (kernInitialized == FALSE)
    return(ERROR);

  INT_LOCK(level);

  if (OBJ_VERIFY (semId, semClassId) != OK)
  {
    INT_UNLOCK (level);
    return (ERROR);
  }

  /* Check next object */
  if (Q_FIRST(&semId->qHead) == NULL)
  {
    INT_UNLOCK(level);
  }
  else
  {
    /* Enter kernel and flush pending queue */
    kernelState = TRUE;
    INT_UNLOCK(level);
    rtosPendQFlush(&semId->qHead);
    kernExit();
  }

  return(OK);
}

/******************************************************************************
* semQFlushDefer - Flush semaphore queue in defered mode
*
* RETURNS: N/A
******************************************************************************/

void semQFlushDefer(SEM_ID semId)
{
  /* Check if flush needed */
  if (Q_FIRST(&semId->qHead) != NULL)
    rtosPendQFlush(&semId->qHead);
}

