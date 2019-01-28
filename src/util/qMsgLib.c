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

/* qMsgLib.c - Used with message queues */

#include <stdlib.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/dllLib.h>
#include <util/qMsgLib.h>
#include <rtos/rtosLib.h>
#include <rtos/msgQLib.h>
#include <rtos/kernQLib.h>
#include <rtos/errnoLib.h>
#include <os/sigLib.h>

/* Imports */
IMPORT BOOL kernelState;
IMPORT BOOL kernExit(void);
IMPORT int  errno;

/* Forward declarations */
LOCAL Q_MSG_HEAD* qMsgCreate(Q_CLASS_ID pendQClass);
LOCAL STATUS qMsgInit(Q_MSG_HEAD *pQHead, Q_CLASS_ID pendQType);
LOCAL STATUS qMsgDestroy(Q_MSG_HEAD *pQHead);
STATUS qMsgTerminate (Q_MSG_HEAD *pQHead);
STATUS qMsgPut (MSG_Q_ID msgQId, Q_MSG_HEAD *pQHead,
                Q_MSG_NODE *pNode, int key);
Q_MSG_NODE* qMsgGet (MSG_Q_ID msgQId, Q_MSG_HEAD *pQHead, unsigned timeout);
LOCAL int qMsgInfo(Q_MSG_HEAD *pQHead, int nodeArray[], int max);
LOCAL Q_MSG_NODE *qMsgEach(Q_MSG_HEAD *pQHead, FUNCPTR func, ARG arg);
LOCAL void qMsgNull(void);

/* Locals */
LOCAL void qMsgPendQGet(MSG_Q_ID msgQId, Q_MSG_HEAD *pQHead);
LOCAL Q_CLASS qMsgClass =
{
  (FUNCPTR) qMsgCreate,
  (FUNCPTR) qMsgInit,
  (FUNCPTR) qMsgDestroy,
  (FUNCPTR) qMsgTerminate,
  (FUNCPTR) qMsgPut,
  (FUNCPTR) qMsgGet,
  (FUNCPTR) qMsgNull,
  (FUNCPTR) qMsgNull,
  (FUNCPTR) qMsgNull,
  (FUNCPTR) qMsgNull,
  (FUNCPTR) qMsgNull,
  (FUNCPTR) qMsgNull,
  (FUNCPTR) qMsgInfo,
  (FUNCPTR) qMsgEach,
  &qMsgClass
};

Q_CLASS_ID qMsgClassId = &qMsgClass;

/*******************************************************************************
* qMsgCreate - Create queue
*
* RETURNS: Queue head pointer
*******************************************************************************/

LOCAL Q_MSG_HEAD* qMsgCreate(Q_CLASS_ID pendQType)
{
  Q_MSG_HEAD *pQHead;

  /* Allocate memory for struct */
  pQHead = malloc( sizeof(Q_MSG_HEAD) );

  if (pQHead == NULL) {
    /* errno set by malloc() */
    return(NULL);
  }

  /* Call initializer below */
  if (qMsgInit(pQHead, pendQType) != OK) {
    /* errno set by qMsgInit() */
    return(NULL);
  }

  return(pQHead);
}

/*******************************************************************************
* qMsgInit - Initialize queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS qMsgInit(Q_MSG_HEAD *pQHead, Q_CLASS_ID pendQType)
{
  pQHead->first = NULL;
  pQHead->last = NULL;
  pQHead->count = 0;

  if (qInit(&pQHead->pendQ, pendQType, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) != OK)
  {
    /* errno set by qInit() */
    return(ERROR);
  }

  return(OK);
}

/*******************************************************************************
* qMsgDestroy - Destroy queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS qMsgDestroy(Q_MSG_HEAD *pQHead)
{
  /* End queue */
  qMsgTerminate(pQHead);

  /* Free memory */
  free(pQHead);

  return(OK);
}

/*******************************************************************************
* qMsgTerminate - End queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS qMsgTerminate(Q_MSG_HEAD *pQHead)
{
  rtosPendQTerminate(&pQHead->pendQ);

  return(OK);
}

/*******************************************************************************
* qMsgPut - Put a node on the queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS qMsgPut(MSG_Q_ID msgQId,
		     Q_MSG_HEAD *pQHead,
		     Q_MSG_NODE *pNode,
		     int key)
{
  int level;
  STATUS eventStatus, returnStatus;
  int oldErrno;

  /* Lock interrupts */
  INT_LOCK(level);

  if (key == Q_MSG_PRI_TAIL)
  {
    /* Add to tail */
    pNode->next = NULL;

    /* Insert */
    if (pQHead->first == NULL)
    {
      pQHead->last = pNode;
      pQHead->first = pNode;
    }
    else
    {
      pQHead->last->next = pNode;
      pQHead->last = pNode;
    }
  }
  else
  {
    /* Insert at head */
    if ((pNode->next = pQHead->first) == NULL)
      pQHead->last = pNode;

    pQHead->first = pNode;
  }

  /* Increase counter */
  pQHead->count++;

  if (kernelState)
  {
    INT_UNLOCK(level);
    kernQAdd2((FUNCPTR) qMsgPendQGet, msgQId, pQHead);
  }
  else
  {
    /* Check if anybody is waiting for message */
    if (Q_FIRST(&pQHead->pendQ) == NULL)
    {
      if (pQHead == &msgQId->msgQ)
      {
        if (msgQId->events.taskId != NULL)
        {
	  returnStatus = OK;
          oldErrno = errno;

          kernelState = TRUE;
          INT_UNLOCK(level);

          eventStatus = eventResourceSend(msgQId->events.taskId,
					  msgQId->events.registered);
          if (eventStatus != OK)
          {
            if ((msgQId->options & MSG_Q_EVENTSEND_ERROR_NOTIFY) != 0x00)
	    {
              errnoSet (S_eventLib_SEND_ERROR);
              returnStatus = ERROR;
	    }
            msgQId->events.taskId = NULL;
          }
          else if (!(msgQId->events.options & EVENTS_SEND_ONCE))
            msgQId->events.taskId = NULL;

          kernExit();

          errnoSet(oldErrno);
          return(returnStatus);
        }
      }

      INT_UNLOCK(level);

    } /* End if nobody is waiting */

    else
    {
      /* Unlock pedning task waiting for message */
      kernelState = TRUE;
      INT_UNLOCK(level);
      rtosPendQGet(&pQHead->pendQ);
      kernExit();
    }
  }

  return(OK);
}

/*******************************************************************************
* qMsgPendQGet - Kernel routine to unblock
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void qMsgPendQGet(MSG_Q_ID msgQId, Q_MSG_HEAD *pQHead)
{
  if (Q_FIRST(&pQHead->pendQ) != NULL)
    rtosPendQGet(&pQHead->pendQ);
  else
  {
    if (msgQId->events.taskId != NULL)
    {
      if (eventResourceSend(msgQId->events.taskId,
			    msgQId->events.registered) != OK)
      {
        msgQId->events.taskId = NULL;
        return;
      }
      else if (!(msgQId->events.options & EVENTS_SEND_ONCE))
        msgQId->events.taskId = NULL;
    }
  }
}

/*******************************************************************************
* qMsgGet - Get and remove a node from queue
*
* RETURNS: Pointer to node
*******************************************************************************/

Q_MSG_NODE* qMsgGet(MSG_Q_ID msgQId,
                    Q_MSG_HEAD *pQHead,
                    unsigned timeout)
{
  Q_MSG_NODE *pNode;
  STATUS status;
  int level;

  /* Lock interrupts */
  INT_LOCK(level);

  while ( (pNode = pQHead->first) == NULL)
  {
    if (timeout == WAIT_NONE)
    {
      errnoSet (S_msgQLib_INVALID_TIMEOUT);
      INT_UNLOCK(level);
      return(NULL);
    }

    /* Enter kernel mode */
    kernelState = TRUE;
    INT_UNLOCK(level);

    /* Put task on hold */
    rtosPendQPut(&pQHead->pendQ, timeout);

    /* Exit trough kernel */
    status = kernExit();

    /* Check kernel status */
    if (status == SIG_RESTART)
      return((Q_MSG_NODE *) NONE);

    if (status != OK)
    {
      /* timer should set errno to S_objLib_OBJ_UNAVAILABLE */
      return(NULL);
    }

    /* Verify object */
    if (OBJ_VERIFY(msgQId, msgQClassId) != OK)
    {

      /* errno set by OBJ_VERIFY() */
      return(NULL);
    }

    /* Lock interrupts */
    INT_LOCK(level);
  }

  pQHead->first = pNode->next;
  pQHead->count--;

  INT_UNLOCK(level);

  return (pNode);
}

/*******************************************************************************
* qMsgInfo - Get info about list
*
* RETURNS: Number of items
*******************************************************************************/

LOCAL int qMsgInfo(Q_MSG_HEAD *pQHead, int nodeArray[], int max)
{
  Q_MSG_NODE *pNode;
  int *pElement;
  int level;

  /* Just count requested */
  if (nodeArray == NULL)
    return (pQHead->count);

  /* Store element start */
  pElement = nodeArray;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Get first node */
  pNode = pQHead->first;

  /* While node not null and max not reached */
  while ( (pNode != NULL) && (--max >= 0) ) {

    *(pElement++) = (int) pNode;
    pNode = pNode->next;

  } /* End while node not null and max not reached */

  /* Unlock interrupts */
  INT_UNLOCK(level);

  return (pElement - nodeArray);
}

/*******************************************************************************
* qMsgEach - Run function for each node
*
* RETURNS: Pointer to node where it ended
*******************************************************************************/

LOCAL Q_MSG_NODE *qMsgEach(Q_MSG_HEAD *pQHead, FUNCPTR func, ARG arg)
{
  Q_MSG_NODE *pNode;
  int level;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Get first node */
  pNode = pQHead->first;

  /* While node not null and function returns true */
  while ( (pNode != NULL) && ((*func) (pNode, arg)) )
    pNode = pNode->next;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  return pNode;
}

/*******************************************************************************
* qMsgNull - Null method
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void qMsgNull(void)
{
  return;
}

