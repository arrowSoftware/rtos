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

/* eventLib.c - Event library */

#include <rtos.h>
#include <arch/intArchLib.h>
#include <rtos/rtosLib.h>
#include <rtos/taskLib.h>
#include <util/qLib.h>
#include <util/qFifoLib.h>
#include <rtos/eventLib.h>
#include <os/sigLib.h>
#include <stdlib.h>

/* Imports */
IMPORT STATUS kernExit(void);
IMPORT TCB_ID taskIdCurrent;
IMPORT BOOL kernelState;

/* Globals */
Q_HEAD eventPendQ;

/* Locals */
LOCAL BOOL eventLibInstalled = FALSE;
LOCAL void eventPendQRemove(TCB_ID pTcb);

/*******************************************************************************
* eventLibInit - Install event library
*
* RETURNS: N/A
*******************************************************************************/

STATUS eventLibInit(void)
{
  /* Check if already installed */
  if (eventLibInstalled)
    return OK;

  qInit(&eventPendQ, qFifoClassId);

  /* Mark as installed */
  eventLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
* eventReceive - Receive events
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS eventReceive(u_int32_t events,
		    u_int8_t options,
		    unsigned timeout,
		    u_int32_t *pReceivedEvents)
{
  int level;
  STATUS status;
  u_int32_t received;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  /* Return right away if fetch option is specified */
  if (options & EVENTS_FETCH)
  {
    if (pReceivedEvents != NULL)
      *pReceivedEvents = taskIdCurrent->events.received;
    return(OK);
  }

  /* Return error if no events specified */
  if (events == 0x00)
  {
    errnoSet (S_eventLib_NO_EVENTS);
    return(ERROR);
  }

eventReceiveLoop:

  /* Cache */
  received = taskIdCurrent->events.received;

  /* ALL events received  */
  if ( (options & EVENTS_WAIT_MASK) == EVENTS_WAIT_ALL)
  {
    /* ALL events are wanted, all must already have been received */
    if ( (events & received) == events)
    {
      INT_LOCK(level);

      /* Store events to return */
      if (pReceivedEvents != NULL)
      {
        if (options & EVENTS_RETURN_ALL)
	  *pReceivedEvents = taskIdCurrent->events.received;
	else
	  *pReceivedEvents = events;
      }

      /* Clear events */
      if ( !(options & EVENTS_KEEP_UNWANTED) ||
	    (options & EVENTS_RETURN_ALL) )
        taskIdCurrent->events.received = 0x00;
      else
        taskIdCurrent->events.received &= ~events;

      INT_UNLOCK(level);
      return(OK);
    }
  }
  /* ANY events received */
  else
  {
    /* ANY events are wanted, even if only one has been received */
    if ( (events & received) != 0x00)
    {
      INT_LOCK(level);

      if (pReceivedEvents != NULL)
      {
        /* Store received events */
        if (options & EVENTS_RETURN_ALL)
          *pReceivedEvents = taskIdCurrent->events.received;
        else
          *pReceivedEvents = events & taskIdCurrent->events.received;
      }

      /* Clear events */
      if ( !(options & EVENTS_KEEP_UNWANTED) ||
            (options & EVENTS_RETURN_ALL) )
        taskIdCurrent->events.received = 0x00;
      else
        taskIdCurrent->events.received &= ~events;

      INT_UNLOCK(level);
      return(OK);
    }
  }

  /* NOT ALL events we wanted has been received */
  INT_LOCK(level);

  /* Check if events has been received while interrupts where unlocked */
  if (taskIdCurrent->events.received != received)
  {
    /* Got new events, loop */
    INT_UNLOCK(level);
    goto eventReceiveLoop;
  }

  /* Check if no wait is specified */
  if (timeout == WAIT_NONE)
  {
    if (pReceivedEvents != NULL)
    {
      /* Store received events */
      if (options & EVENTS_RETURN_ALL)
        *pReceivedEvents = taskIdCurrent->events.received;
      else
        *pReceivedEvents = events & taskIdCurrent->events.received;
    }

    /* Clear events */
    if ( !(options & EVENTS_KEEP_UNWANTED) ||
          (options & EVENTS_RETURN_ALL) )
      taskIdCurrent->events.received = 0x00;
    else
      taskIdCurrent->events.received &= ~events;

    INT_UNLOCK(level);

    errnoSet (S_objLib_UNAVAILABLE);

    /* We did not want to wait long enough */
    return(ERROR);
  }

  /* If wait is specified, then setup task wait event fields */
  taskIdCurrent->events.options = options;
  taskIdCurrent->events.wanted = events;
  taskIdCurrent->events.sysflags = EVENTS_SYSFLAGS_WAITING;

  /* Enter kernel mode */
  kernelState = TRUE;
  INT_UNLOCK(level);

  /* Put on hold */
  rtosPendQPut(&eventPendQ, timeout);

  /* Exit kernel */
  status = kernExit();

  /* Check status */
  if (status == SIG_RESTART)
  {
    INT_LOCK(level);
    taskIdCurrent->events.sysflags &= ~EVENTS_SYSFLAGS_WAITING;
    INT_UNLOCK(level);

    if (taskIdCurrent->events.sysflags & EVENTS_SYSFLAGS_DELETE_RESOURCE)
    {
      taskIdCurrent->events.sysflags &= ~EVENTS_SYSFLAGS_DELETE_RESOURCE;
      status = ERROR;
    }
    else
    {
      /* SHOULD RECALCULATE TIMEOUT HERE */
      goto eventReceiveLoop;
    }
  }
  else if (status == ERROR)
  {
    taskIdCurrent->events.sysflags &= ~EVENTS_SYSFLAGS_WAITING;
    /* SHOULD SET errno here if signal timeout*/
  }

  /* Now collect data */
  INT_LOCK(level);

  if (pReceivedEvents != NULL)
  {
    /* Store received events */
    if (options & EVENTS_RETURN_ALL)
      *pReceivedEvents = taskIdCurrent->events.received;
    else
      *pReceivedEvents = events & taskIdCurrent->events.received;
  }

  /* Clear events */
  if ( !(options & EVENTS_KEEP_UNWANTED) ||
        (options & EVENTS_RETURN_ALL) )
    taskIdCurrent->events.received = 0x00;
  else
    taskIdCurrent->events.received &= ~events;

  INT_UNLOCK(level);

  return(status);
}

/*******************************************************************************
* eventSend - Receive events
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS eventSend(int taskId, u_int32_t events)
{
  TCB_ID pTcb;
  int level;
  u_int8_t options;
  u_int32_t wanted, received;

  if ( (INT_CONTEXT()) && (taskId == (int) NULL) )
  {
    errnoSet (S_eventLib_NULL_TASKID);
    return(ERROR);
  }

  /* Get context */
  pTcb = (taskId == (int) NULL) ? taskIdCurrent : (TCB_ID) taskId;

  INT_LOCK(level);

  /* Verify task */
  if (TASK_ID_VERIFY(pTcb) != OK)
  {
    INT_UNLOCK(level);
    /* errno set by TASK_ID_VERIFY() */
    return(ERROR);
  }

  /* Set events to send */
  pTcb->events.received |= events;

  /* If task is not waiting for any events we are finished */
  if ( !(pTcb->events.sysflags & EVENTS_SYSFLAGS_WAITING) )
  {
    INT_UNLOCK(level);
    return(OK);
  }

  /* Setup data */
  received = pTcb->events.received;
  wanted = pTcb->events.wanted;
  options = pTcb->events.options;

  /* Now check the two cases */
  /* 1) all the wanted events have been received */
  /* 2) some events have been received and some are we still waiting for */
  if ( ((wanted & received) == wanted) ||
       ( ((options & EVENTS_WAIT_MASK) == EVENTS_WAIT_ANY) &&
	 ((wanted & received) != 0x00)) )
  {
    /* Events received stop waiting */
    pTcb->events.sysflags &= ~EVENTS_SYSFLAGS_WAITING;
    if (kernelState)
    {
      INT_UNLOCK(level);
      kernQAdd1((FUNCPTR) eventPendQRemove, (ARG) pTcb);
      return(OK);
    }

    /* Check if pending, then wake up */
    if ( (pTcb->status & TASK_PEND) == TASK_PEND)
    {
      kernelState = TRUE;
      INT_UNLOCK(level);

      rtosPendQRemove(pTcb);
      rtosReadyQPut(pTcb);

      kernExit();
    }
    else
    {
      INT_UNLOCK(level);
    }

    return(OK);
  }

  /* Still waiting */
  INT_UNLOCK(level);
  return(OK);
}

/*******************************************************************************
* eventPendQRemove - Remove from event queue, needed in kernel mode
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void eventPendQRemove(TCB_ID pTcb)
{
  if ( (pTcb->status & TASK_PEND) == TASK_PEND)
  {
    rtosPendQRemove(pTcb);
    rtosReadyQPut(pTcb);
  }
}

/*******************************************************************************
* eventClear - Clear all events
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS eventClear(void)
{
  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  /* Clear events */
  taskIdCurrent->events.received = 0x00;

  return(OK);
}

/*******************************************************************************
* eventResourceInit - Initialize an event for a resource
*
* RETURNS: N/A
*******************************************************************************/

void eventResourceInit(EVENTS_RESOURCE *pResource)
{
  pResource->taskId = (int) NULL;
  pResource->registered = 0x00;
  pResource->options = 0x00;
}

/*******************************************************************************
* eventResourceRegister - Register a resource for events
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS eventResourceRegister(OBJ_ID objId,
			     EVENTS_RESOURCE *pResource,
			     FUNCPTR isFreeMethod,
			     u_int32_t events,
			     u_int8_t options)
{
  int level;

  /* Don not register if any other task has done so */
  if (
      (pResource->options & EVENTS_ALLOW_OVERWRITE) != EVENTS_ALLOW_OVERWRITE
     )
  {
    if ( (pResource->taskId != (int) NULL) &&
	 (pResource->taskId != (int) taskIdCurrent) )
    {
      taskUnlock();

      errnoSet (S_eventLib_ALREADY_REGISTERED);
      return(ERROR);
    }
  }

  INT_LOCK(level);

  /* Check if events should be sent */
  if ( (options & EVENTS_SEND_IF_FREE) == EVENTS_SEND_IF_FREE )
  {
    /* Check if resource is free */
    if ( (*isFreeMethod)(objId) == TRUE)
    {
      INT_UNLOCK(level);

      /* Just once ? */
      if ( (options & EVENTS_SEND_ONCE) != EVENTS_SEND_ONCE )
      {
        pResource->registered = events;
	pResource->options = options;
	pResource->taskId = (int) taskIdCurrent;
      }
      else
      {
        pResource->taskId = (int) NULL;
      }

      taskUnlock();

      /* Send events to the task */
      if (eventSend((int) taskIdCurrent, events) != OK)
      {
        errnoSet (S_eventLib_SEND_ERROR);
	return(ERROR);
      }
      else
      {
        return(OK);
      }
    }
  }

  /* Resource not free */
  /* Store info */
  pResource->registered = events;
  pResource->options = options;
  pResource->taskId = (int) taskIdCurrent;

  INT_UNLOCK(level);
  taskUnlock();

  return(OK);
}

/*******************************************************************************
* eventResourceSend - Send events from resource
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS eventResourceSend(int taskId, u_int32_t events)
{
  TCB_ID pTcb;
  u_int8_t options;
  u_int32_t wanted, received;
  int level;

  /* Get tcb id */
  pTcb = (TCB_ID) taskId;

  INT_LOCK(level);

  /* Verify */
  if (TASK_ID_VERIFY(pTcb) != OK)
  {
    INT_UNLOCK(level);
    /* errno set by TASK_ID_VERIFY() */
    return(ERROR);
  }

  /* Set events */
  pTcb->events.received |= events;

  /* Check if task is not waiting for any events */
  if ( !(pTcb->events.sysflags & EVENTS_SYSFLAGS_WAITING) )
  {
    /* No */
    INT_UNLOCK(level);
    return(OK);
  }

  received = pTcb->events.received;
  wanted = pTcb->events.wanted;
  options = pTcb->events.options;

  /* Now check the two cases */
  /* 1) all the wanted events have been received */
  /* 2) some events have been received and some are we still waiting for */
  if ( ((wanted & received) == wanted) ||
       ( ((options & EVENTS_WAIT_MASK) == EVENTS_WAIT_ANY) &&
	 ((wanted & received) != 0x00)) )
  {
    pTcb->events.sysflags &= ~EVENTS_SYSFLAGS_WAITING;

    /* Now put it back for execution */
    INT_UNLOCK(level);
    if ((pTcb->status & TASK_PEND) == TASK_PEND)
    {
      rtosPendQRemove(pTcb);
      rtosReadyQPut(pTcb);
      return(OK);
    }
  }

  /* No news */
  INT_UNLOCK(level);
  return(OK);
}

/*******************************************************************************
* eventResourceTerminate - Terminate events for a resource
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS eventResourceTerminate(EVENTS_RESOURCE *pEventResource)
{
  int level;
  TCB_ID pTcb;

  pTcb = (TCB_ID) pEventResource->taskId;

  /* Check if NULL */
  if (pTcb == NULL)
  {
    errnoSet (S_eventLib_NULL_TASKID);
    return(ERROR);
  }

  INT_LOCK(level);

  /* Verify task */
  if (TASK_ID_VERIFY(pTcb) != OK)
  {
    INT_UNLOCK(level);
    /* errno set by TASK_ID_VERIFY() */
    return(ERROR);
  }

  /* Check if task is waiting */
  if (pTcb->events.sysflags & EVENTS_SYSFLAGS_WAITING)
  {
    pTcb->events.sysflags &= ~EVENTS_SYSFLAGS_WAITING;

    INT_UNLOCK(level);

    /* Unpend task if pending */
    if ( (pTcb->status & TASK_PEND) == TASK_PEND )
    {
      rtosPendQRemove(pTcb);
      rtosReadyQPut(pTcb);
      taskRetValueSet(pTcb, ERROR);
    }
    else
    {
      pTcb->events.sysflags |= EVENTS_SYSFLAGS_DELETE_RESOURCE;
    }
  }
  else
  {
    INT_UNLOCK(level);
  }

  return(OK);
}

