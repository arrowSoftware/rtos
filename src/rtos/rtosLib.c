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

/* rtosLib.c - Task scheduling library */

#include <stdlib.h>
#include <sys/types.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/qLib.h>
#include <util/qPrioLib.h>
#include <util/qFifoLib.h>
#include <rtos/kernLib.h>
#include <rtos/kernQLib.h>
#include <rtos/taskLib.h>
#include <rtos/wdLib.h>
#include <rtos/rtosLib.h>

/* Imports */
IMPORT void taskRetValueSet(TCB_ID pTcb, int val);
IMPORT BOOL kernelState;
IMPORT BOOL kernRoundRobin;
IMPORT unsigned kernRoundRobinTimeSlice;
IMPORT TCB_ID taskIdCurrent;		/* Running task */
IMPORT Q_HEAD kernActiveQ;		/* Active tasks: TCB.activeNode */
IMPORT Q_HEAD kernTickQ;		/* Delayed tasks: TCB.tickNode */
IMPORT Q_HEAD kernReadyQ;		/* Ready tasks: TCB.qNode */
IMPORT volatile unsigned kernTicks;
IMPORT volatile unsigned kernAbsTicks;
IMPORT u_int32_t intCnt;
IMPORT TCB_ID taskIdCurrent;
IMPORT int taskSwapReference[];

/******************************************************************************
* rtosSpawn - Add a new task, initiallly in suspended state
*
* RETURNS: N/A
******************************************************************************/

void rtosSpawn(TCB_ID pTcb)
{
    /* Activate task */
    Q_PUT(&kernActiveQ, &pTcb->activeNode, FIFO_KEY_TAIL);
}

/******************************************************************************
* rtosDelete - Remove a task
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS rtosDelete(TCB_ID pTcb)
{
  u_int16_t mask;
  int i;
  BOOL status = OK;

  /* If task is ready, remove it from ready queue */
  if (pTcb->status == TASK_READY)
  {
    Q_REMOVE(&kernReadyQ, pTcb);
  }
  else
  {

    /* If task peding flag is set, remove it from pending queue */
    if (pTcb->status & TASK_PEND)
      status = rtosPendQRemove(pTcb);

    /* If task delay flag is set, remove it from tick queue */
    if (pTcb->status & TASK_DELAY)
      Q_REMOVE(&kernTickQ, &pTcb->tickNode);

  }

  /* Disconnect to all swap hooks */
  for (i = 0, mask = pTcb->swapInMask; mask != 0; i++, mask = mask << 1)
    if (mask & 0x8000)
      taskSwapReference[i]--;

  for (i = 0, mask = pTcb->swapOutMask; mask != 0; i++, mask = mask << 1)
    if (mask & 0x8000)
      taskSwapReference[i]--;

  /* Deactivate task */
  Q_REMOVE(&kernActiveQ, &pTcb->activeNode);
  pTcb->status = TASK_DEAD;

  return(status);
}

/******************************************************************************
* rtosSuspend - Suspend a task
*
* RETURNS: N/A
******************************************************************************/

void rtosSuspend(TCB_ID pTcb)
{
  /* If task is ready, remove it from ready queue */
  if (pTcb->status == TASK_READY)
    Q_REMOVE(&kernReadyQ, pTcb);

  /* Set suspended flag */
  pTcb->status |= TASK_SUSPEND;
}

/******************************************************************************
* rtosResume - Resume a suspended task
*
* RETURNS: N/A
******************************************************************************/

void rtosResume(TCB_ID pTcb)
{
  /* If task is suspended, put it on ready queue */
  if (pTcb->status == TASK_SUSPEND)
    Q_PUT(&kernReadyQ, pTcb, pTcb->priority);

  /* Reset suspended flag */
  pTcb->status &= ~TASK_SUSPEND;
}

/******************************************************************************
* rtosTickAnnounce - Make kernel time pass
*
* RETURNS: N/A
******************************************************************************/

void rtosTickAnnounce(void)
{
  Q_NODE *pNode;
  TCB_ID pTcb;
  WDOG_ID wdId;
  FUNCPTR wdFunc;
  ARG wdArg;
  STATUS status;

  /* Increase time counters */
  kernTicks++;
  kernAbsTicks++;

  /* Advance time queue */
  Q_ADVANCE(&kernTickQ);

  while ((pNode = (Q_NODE *) Q_EXPIRED(&kernTickQ)) != NULL)
  {
    pTcb = (TCB_ID) ((int)pNode - OFFSET(TCB, tickNode));

    /* If this is a task */
    if (pTcb->objCore.pObjClass == taskClassId)
    {
      pTcb->status &= ~TASK_DELAY;		/* Clear delay flag */

      if (pTcb->status == TASK_READY)		/* Timeout */
      {
        taskRetValueSet(pTcb, OK);
      }
      else if (pTcb->status & TASK_PEND)
      {
        /* Remove from pend queue */
        status = rtosPendQRemove (pTcb);   /* Remove item from pend Q. */
        if (status == ERROR)              /* If returned ERROR (unlikely) */
            pTcb->errorStatus = ERROR;    /* set task's [errorStatus]. */

        taskRetValueSet (pTcb, ERROR);    /* Item has timed-out.  Set errno. */
        pTcb->errno = S_objLib_TIMEOUT;

        /*
         * If the task removed from the pend queue has a custom
         * 'unpend' handler, call it.
         */

        if (pTcb->objUnpendHandler != NULL) {
            pTcb->objUnpendHandler (pTcb, rtos_TIMEOUT);
        }
      }

      /* If task is ready now, add it to ready queue */
      if (pTcb->status == TASK_READY)
        Q_PUT(&kernReadyQ, pTcb, pTcb->priority);
    }

    /* Else this is a watchdog timer */
    else {

      /* Convert to watchdog id */
      wdId = (WDOG_ID) ((int) pNode - OFFSET(WDOG, tickNode) );

      /* Process */
      wdId->status = WDOG_OUT_OF_Q;
      wdFunc = wdId->wdFunc;
      wdArg = wdId->wdArg;

      /* Call watchdog function */
      intCnt++;
      if ( wdId->dfrStartCount == 0)
        ( *wdFunc) (wdArg);
      intCnt--;

      /* Do kernel work */
      kernQDoWork();

    }

  }

  /* Perform periodic task switching in round robin mode */
  if ((kernRoundRobin) &&
      (taskIdCurrent->lockCount == 0) &&
      (taskIdCurrent->status == TASK_READY) &&
      (++taskIdCurrent->timeSlice >= kernRoundRobinTimeSlice))
  {
    taskIdCurrent->timeSlice = 0;
    Q_REMOVE(&kernReadyQ, taskIdCurrent);
    Q_PUT(&kernReadyQ, taskIdCurrent, taskIdCurrent->priority);
  }
}

/******************************************************************************
* rtosDelay - Put a task to sleep
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS rtosDelay(unsigned timeout)
{
  /* Remove from running tasks queue */
  Q_REMOVE(&kernReadyQ, taskIdCurrent);

  /* Will kernel timer overflow ? */
  if ((unsigned) (kernTicks + timeout) < kernTicks)
  {
    Q_OFFSET(&kernTickQ, ~kernTicks + 1);
    kernTicks = 0;
  }

  /* Put task on delay queue */
  Q_PUT(&kernTickQ, &taskIdCurrent->tickNode, timeout + kernTicks);

  /* Update status to delayed */
  taskIdCurrent->status |= TASK_DELAY;

  return(OK);
}

/******************************************************************************
* rtosUndelay - Wake up a sleeping task
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS rtosUndelay(TCB_ID pTcb)
{
  /* If not in delayed mode, just return */
  if ((pTcb->status & TASK_DELAY) == 0)
    return(OK);

  /* Remove delay flag */
  pTcb->status &= ~TASK_DELAY;

  /* Remove from tick queue */
  Q_REMOVE(&kernTickQ, &pTcb->priority);
  pTcb->errorStatus = TASK_UNDELAYED;

  /* Put on ready queue */
  if (pTcb->status == TASK_READY)
    Q_PUT(&kernReadyQ, pTcb, pTcb->priority);

  return(OK);
}

/******************************************************************************
* rtosPrioritySet - Set task priority
*
* RETURNS: N/A
******************************************************************************/

void rtosPrioritySet(TCB_ID pTcb, unsigned priority)
{
  /* Check mutex and current priority */
  /* REMOVED CODE */

  /* Update priotity */
  pTcb->priority = priority;

  /* Update correct in queue */
  if (pTcb->status == TASK_READY)
    Q_MOVE(&kernReadyQ, pTcb, priority);
  else if (pTcb->status & TASK_PEND)
    Q_MOVE(pTcb->pPendQ, pTcb, priority);
}

/******************************************************************************
* rtosSemDelete - Delete a semaphore
*
* RETURNS: N/A
******************************************************************************/

void rtosSemDelete(SEM_ID semId)
{
  TCB_ID pOwner;

  /* End pending queue */
  rtosPendQTerminate(&semId->qHead);

  /* Get owner */
  pOwner = SEM_OWNER_GET(semId);

  /* Special for mutex */
  if (semId->semType == SEM_TYPE_MUTEX)
  {
    /* Has it been taken ? */
    if (pOwner != NULL)
    {
      /* Valid task pending ? */
      if (OBJ_VERIFY(pOwner, taskClassId) == OK)
      {
        /* Is it inversion safe */
	/* REMOVED CODE */

        /* Is it delete safe */
        if ( (semId->options & SEM_DELETE_SAFE) &&
	     (--pOwner->safeCount == 0) &&
	     (Q_FIRST(&pOwner->safetyQ) != NULL) )
        {
          rtosPendQFlush(&pOwner->safetyQ);
        }
      }
    }
  }
}

/******************************************************************************
* rtosPendQGet - Take next task on pend queue
*
* RETURNS: N/A
******************************************************************************/

void rtosPendQGet(Q_HEAD *pQHead)
{
  /* Get task on queue */
  TCB_ID pTcb = (TCB_ID) Q_GET(pQHead);

  pTcb->status &= ~TASK_PEND;	/* Clear pending flag */
  taskRetValueSet(pTcb, OK);	/* Set return value */

  if (pTcb->status & TASK_DELAY)
  {
    pTcb->status &= ~TASK_DELAY;		/* Clear delay flag */
    Q_REMOVE(&kernTickQ, &pTcb->tickNode);	/* Remove from queue */
  }

  /* Check if task is ready */
  if (pTcb->status == TASK_READY)
  {
    Q_PUT(&kernReadyQ, pTcb, pTcb->priority);
  }
}

/******************************************************************************
* rtosReadyQPut - Put task on ready queue
*
* RETURNS: N/A
******************************************************************************/

void rtosReadyQPut(TCB_ID pTcb)
{
  pTcb->status &= ~TASK_PEND;	/* Clear pending flag */
  taskRetValueSet(pTcb, OK);	/* Set return value */

  if (pTcb->status & TASK_DELAY)
  {
    pTcb->status &= ~TASK_DELAY;		/* Clear delay flag */
    Q_REMOVE(&kernTickQ, &pTcb->tickNode);	/* Remove from queue */
  }

  /* Check if task is ready */
  if (pTcb->status == TASK_READY)
    Q_PUT(&kernReadyQ, pTcb, pTcb->priority);
}

/******************************************************************************
* rtosReadyQRemove - Remove current pending task from ready queue
*
* RETURNS: N/A
******************************************************************************/

void rtosReadyQRemove(Q_HEAD *pQHead, unsigned timeout)
{
  /* Remove from ready queue */
  Q_REMOVE(&kernReadyQ, taskIdCurrent);

  /* Update status */
  taskIdCurrent->status |= TASK_PEND;

  /* Set task pending queue */
  taskIdCurrent->pPendQ = pQHead;

  if (timeout != WAIT_FOREVER)
  {
    /* Check timer overflow */
    if ((unsigned)(kernTicks + timeout) < kernTicks)
    {
      Q_OFFSET(&kernTickQ, ~kernTicks + 1);
      kernTicks = 0;
    }

    /* Put on tick queue */
    Q_PUT(&kernTickQ, &taskIdCurrent->tickNode, timeout + kernTicks);
    taskIdCurrent->status |= TASK_DELAY;
  }
}

/******************************************************************************
* rtosPendQFlush - Flush all tasks on pending queue to make them ready
*
* RETURNS: N/A
******************************************************************************/

void rtosPendQFlush(Q_HEAD *pQHead)
{
  TCB_ID pTcb;

  /* Get all tasks */
  while ((pTcb = (TCB_ID) Q_GET(pQHead)) != NULL)
  {
    pTcb->status &= ~TASK_PEND;	/* Clear pending flag */
    taskRetValueSet(pTcb, OK);	/* Set return value */

    if (pTcb->status & TASK_DELAY)
    {
      pTcb->status &= ~TASK_DELAY;		/* Clear delay flag */
      Q_REMOVE(&kernTickQ, &pTcb->tickNode);	/* Remove from queue */
    }

    /* Check if task is ready */
    if (pTcb->status == TASK_READY)
      Q_PUT(&kernReadyQ, pTcb, pTcb->priority);
  }
}

/******************************************************************************
 *
 * rtosPendQWithHandlerPut - put a task on pending queue and set unpend handler
 *
 * This routine adds a task wtih a custom 'unpend' handler to the specified
 * pending queue.  In the event that the task times-out, is deleted, or for
 * some other reason does not obtain the resource for which it is pending, it
 * will call the 'unpend' handler.  It is assumed that the parameter <timeout>
 * will never be WAIT_NONE.
 *
 * RETURNS: N/A
 */

void rtosPendQWithHandlerPut (
    Q_HEAD *  pQHead,                  /* ptr to Q on which to pend */
    unsigned  timeout,                 /* timeout with which to pend */
    void      (*handler)(void *, int), /* custom 'unpend' handler */
    void *    pObj,                    /* ptr to object upon which to pend */
    int       info                     /* custom info to pass to handler */
    ) {
    Q_REMOVE (&kernReadyQ, taskIdCurrent);   /* Remove from ready queue */

    taskIdCurrent->status |= TASK_PEND;      /* Update status */

    taskIdCurrent->pPendQ = pQHead;          /* Set task pending queue */

    taskIdCurrent->objUnpendHandler = handler;
    taskIdCurrent->pObj = pObj;
    taskIdCurrent->objInfo = info;

    /* Put on task pending queue */
    Q_PUT (pQHead, taskIdCurrent, taskIdCurrent->priority);

    if (timeout != WAIT_FOREVER)
    {
        /* Check timer overflow */
        if ((unsigned) (kernTicks + timeout) < kernTicks)
        {
            Q_OFFSET (&kernTickQ, ~kernTicks + 1);
            kernTicks = 0;
        }

        /* Put on tick queue */
        Q_PUT (&kernTickQ, &taskIdCurrent->tickNode, timeout + kernTicks);
        taskIdCurrent->status |= TASK_DELAY;
    }
}

/******************************************************************************
* rtosPendQPut - Put a task on pending queue
*
* RETURNS: OK, or ERROR
******************************************************************************/

STATUS rtosPendQPut(Q_HEAD *pQHead, unsigned timeout)
{
  if (timeout == WAIT_NONE)
  {
    return(ERROR);
  }

  rtosPendQWithHandlerPut (pQHead, timeout, NULL, NULL, 0);

  return(OK);
}

/******************************************************************************
* rtosPendQRemove - Remove a task from pending queue
*
* RETURNS: OK, or ERROR
******************************************************************************/

STATUS rtosPendQRemove(TCB_ID pTcb)
{
  STATUS status;

  status = (STATUS) Q_REMOVE(pTcb->pPendQ, pTcb);

  pTcb->status &= ~TASK_PEND;	/* Clear pending flag */
  /* REMOVED CODE */

  if (pTcb->status & TASK_DELAY)
  {
    pTcb->status &= ~TASK_DELAY;		/* Clear delay flag */
    Q_REMOVE(&kernTickQ, &pTcb->tickNode);	/* Remove from queue */
  }

  return(status);
}

/******************************************************************************
* rtosPendQTerminate - Flush all tasks on pending queue and make the ready
*
* RETURNS: N/A
******************************************************************************/

void rtosPendQTerminate(Q_HEAD *pQHead)
{
  TCB_ID pTcb;

  /* Get all tasks */
  while ((pTcb = (TCB_ID) Q_GET(pQHead)) != NULL)
  {
    pTcb->status &= ~TASK_PEND;	/* Clear pending flag */
    /* REMOVED CODE */
    pTcb->errorStatus = TASK_DELETED;

    /* Return error */
    taskRetValueSet(pTcb, ERROR);

    if (pTcb->status & TASK_DELAY)
    {
      pTcb->status &= ~TASK_DELAY;		/* Clear delay flag */
      Q_REMOVE(&kernTickQ, &pTcb->tickNode);	/* Remove from queue */
    }

    /* Check if task is ready */
    if (pTcb->status == TASK_READY)
      Q_PUT(&kernReadyQ, pTcb, pTcb->priority);
  }
}

/******************************************************************************
* rtosWdStart - Start watchdog timer
*
* RETURNS: OK, or ERROR
******************************************************************************/

STATUS rtosWdStart(WDOG_ID wdId, unsigned timeout)
{
  int level;
  int dfrStartCount;

  /* Get defer start count */
  INT_LOCK(level);
  dfrStartCount = --wdId->dfrStartCount;
  INT_UNLOCK(level);

  if (dfrStartCount == 0) {

    /* If overflow */
    if ( (unsigned) (kernTicks + timeout) < kernTicks ) {

      Q_OFFSET(&kernTickQ, ~kernTicks + 1);
      kernTicks = 0;

    }

    if (wdId->status == WDOG_IN_Q)
      Q_MOVE(&kernTickQ, &wdId->tickNode, timeout + kernTicks);
    else
      Q_PUT(&kernTickQ, &wdId->tickNode, timeout + kernTicks);

    /* Mark as in queue */
    wdId->status = WDOG_IN_Q;

  }

  return OK;
}

/******************************************************************************
* rtosWdCancel - Cancel watchdog timer
*
* RETURNS: N/A
******************************************************************************/

void rtosWdCancel(WDOG_ID wdId)
{

  /* If in queue */
  if (wdId->status == WDOG_IN_Q) {

    Q_REMOVE(&kernTickQ, &wdId->tickNode);
    wdId->status = WDOG_OUT_OF_Q;

  }

}

