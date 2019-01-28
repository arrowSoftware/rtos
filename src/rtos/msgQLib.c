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

/* msgQLib.c - Message queue library */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/qLib.h>
#include <util/qMsgLib.h>
#include <rtos/taskLib.h>
#include <rtos/memPartLib.h>
#include <rtos/eventLib.h>
#include <rtos/rtosLib.h>
#include <rtos/msgQLib.h>
#include <rtos/msgQEvLib.h>

/* Imports */
IMPORT BOOL kernelState;
IMPORT Q_CLASS_ID  qFifoClassId;
IMPORT Q_CLASS_ID  qPrioClassId;
IMPORT Q_CLASS_ID  qMsgClassId;
IMPORT STATUS kernExit(void);

IMPORT STATUS qMsgPut(MSG_Q_ID msgQId, Q_MSG_HEAD *pQHead,
                      Q_MSG_NODE *pNode, int key);
IMPORT Q_MSG_NODE* qMsgGet(MSG_Q_ID msgQId, Q_MSG_HEAD *pQHead,
                           unsigned timeout);
IMPORT STATUS qMsgTerminate (Q_MSG_HEAD *pQHead);

/* Locals */
LOCAL OBJ_CLASS msgQClass;
LOCAL BOOL msgQLibInstalled = FALSE;

/* Globals */
CLASS_ID msgQClassId = &msgQClass;

/*******************************************************************************
* msgQLibInit - Initialize message quque library
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQLibInit(void)
{
  if (msgQLibInstalled) {
    return(OK);
  }

  if (classInit(msgQClassId, sizeof(MSG_Q),
                OFFSET(MSG_Q, objCore),
		memSysPartId,
                (FUNCPTR) msgQCreate,
                (FUNCPTR) msgQInit,
                (FUNCPTR) msgQDestroy) != OK) {
    return(ERROR);
  }

  msgQLibInstalled = TRUE;

  return(OK);
}

/*******************************************************************************
* msgQCreate - Create a message queue
*
* RETURNS: MSG_Q_ID
*******************************************************************************/

MSG_Q_ID msgQCreate(int maxMsg,
		    int maxMsgLength,
		    int options)
{
  MSG_Q_ID msgQId;
  void *pPool;
  unsigned size;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(NULL);
  }

  if (msgQLibInstalled == FALSE)
  {
    errnoSet (S_msgQLib_NOT_INSTALLED);
    return(NULL);
  }

  /* Get size */
  size = (unsigned) maxMsg * MSG_NODE_SIZE(maxMsgLength);

  /* Allocate mem */
  msgQId = (MSG_Q_ID) objAllocPad(msgQClassId, size, &pPool);
  if (msgQId == NULL)
  {

    /* errno set by objAllocPad */
    return(NULL);
  }

  /* Initialize */
  if (msgQInit(msgQId, maxMsg, maxMsgLength, options, pPool) != OK)
  {

    /* errno set by msgQInit() */
    objFree(msgQClassId, msgQId);
    return(NULL);
  }

  return(msgQId);
}

/*******************************************************************************
* msgQInit - Initialize a message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQInit(MSG_Q_ID msgQId,
		int maxMsg,
		int maxMsgLength,
		int options,
		void *pMsgPool)
{
  int nodeSize;
  int i;
  Q_CLASS_ID msgQType;

  if (msgQLibInstalled == FALSE)
  {
    errnoSet (S_msgQLib_NOT_INSTALLED);
    return (ERROR);
  }

  /* Get size */
  nodeSize = MSG_NODE_SIZE(maxMsgLength);

  /* Clear */
  memset(msgQId, 0, sizeof(MSG_Q));

  /* Initialize queues */
  switch(options & MSG_Q_TYPE_MASK)
  {
    case MSG_Q_FIFO:
      msgQType = qFifoClassId;
      break;

    case MSG_Q_PRIORITY:
      msgQType = qPrioClassId;
      break;

    default:
      errnoSet (S_msgQLib_INVALID_Q_TYPE);
      return (ERROR);
  }

  if (qInit((Q_HEAD *) &msgQId->msgQ, qMsgClassId, msgQType,
	    0, 0, 0, 0, 0, 0, 0, 0, 0) != OK)
  {
    /* Let qInit() set the errno */
    return (ERROR);
  }

  if (qInit((Q_HEAD *) &msgQId->freeQ, qMsgClassId, msgQType,
	    0, 0, 0, 0, 0, 0, 0, 0, 0) != OK)
  {
    /* Let qInit() set the errno */
    return (ERROR);
  }

  /* Insert nodes on the free queue */
  for (i = 0; i < maxMsg; i++)
  {
    qMsgPut(msgQId, &msgQId->freeQ, (Q_MSG_NODE *) pMsgPool, Q_MSG_PRI_ANY);
    pMsgPool = (void *) (((char *) pMsgPool) + nodeSize);
  }

  /* Initialize structure */
  msgQId->options = options;
  msgQId->maxMsg = maxMsg;
  msgQId->maxMsgLength = maxMsgLength;

  eventResourceInit(&msgQId->events);

  objCoreInit(&msgQId->objCore, msgQClassId);

  return (OK);
}

/*******************************************************************************
* msgQTerminate - Terminate a message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQTerminate(MSG_Q_ID msgQId)
{
  return(msgQDestroy(msgQId, FALSE));
}

/*******************************************************************************
* msgQDelete - Delete a message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQDelete(MSG_Q_ID msgQId)
{
  return(msgQDestroy(msgQId, FALSE));
}

/*******************************************************************************
* msgQDestroy - Destroy a message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQDestroy(MSG_Q_ID msgQId, BOOL deallocate)
{
  Q_MSG_NODE *pNode;
  int errnoCopy;
  unsigned timeout;
  int nMsg;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  /* Lock */
  taskSafe();
  taskLock();


  if (OBJ_VERIFY(msgQId, msgQClassId) != OK)
  {
    taskUnlock();
    taskUnsafe();
    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Invalidate */
  objCoreTerminate(&msgQId->objCore);

  taskUnlock();
  timeout = WAIT_NONE;
  nMsg = 0;
  errnoCopy = errnoGet();

  /* Pick out all nodes */
  while (nMsg < msgQId->maxMsg)
  {
    while (((pNode = qMsgGet(msgQId, &msgQId->freeQ, timeout)) != NULL) &&
	   (pNode != (Q_MSG_NODE *) NONE))
      nMsg++;

    while (((pNode = qMsgGet(msgQId, &msgQId->msgQ, timeout)) != NULL) &&
	   (pNode != (Q_MSG_NODE *) NONE))
      nMsg++;

    /* Wait a little */
    timeout = 1;
  }

  errnoSet(errnoCopy);

  kernelState = TRUE;

  qMsgTerminate(&msgQId->msgQ);
  qMsgTerminate(&msgQId->freeQ);

  eventResourceTerminate(&msgQId->events);

  kernExit();

  if (deallocate)
    objFree(msgQClassId, msgQId);

  taskUnsafe();

  return(OK);
}

/*******************************************************************************
* msgQSend - Send a message on message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQSend(MSG_Q_ID msgQId,
		void *buffer,
		unsigned nBytes,
		unsigned timeout,
		unsigned priority)
{
  MSG_NODE *pMsg;

  /* Lock */
  if (!INT_CONTEXT())
  {
    taskLock();
  }
  else
  {
    if (timeout != WAIT_NONE)
    {
      errnoSet (S_msgQLib_INVALID_TIMEOUT);
      return(ERROR);
    }
  }

msgQSendLoop:

  if (OBJ_VERIFY(msgQId, msgQClassId) != OK)
  {
    if (!INT_CONTEXT())
      taskUnlock();
    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Check if size is within range */
  if (nBytes > msgQId->maxMsgLength)
  {
    if (!INT_CONTEXT())
      taskUnlock();
    errnoSet (S_msgQLib_INVALID_MSG_LENGTH);
    return(ERROR);
  }

  /* Get next node from free queue */
  pMsg = (MSG_NODE *) qMsgGet(msgQId, &msgQId->freeQ, timeout);
  if (pMsg == (MSG_NODE *) NONE)
  {
    goto msgQSendLoop;
  }

  if (pMsg == NULL)
  {
    msgQId->sendTimeouts++;
    if (!INT_CONTEXT())
      taskUnlock();
    /* timer should automatically set errno to S_objLib_UNAVAILABLE */
    return(ERROR);
  }

  /* Put node on message queue */
  pMsg->msgLength = nBytes;
  memcpy(MSG_NODE_DATA(pMsg), buffer, nBytes);
  if (qMsgPut(msgQId, &msgQId->msgQ, &pMsg->node, priority) != OK)
  {
    if (!INT_CONTEXT())
      taskUnlock();
    /* errno set by qMsgPut() */
    return(ERROR);
  }

  if (!INT_CONTEXT())
    taskUnlock();

  return(OK);
}

/*******************************************************************************
* msgQReceive - Receive a message from message queue
*
* RETURNS: number of bytes read
*******************************************************************************/

int msgQReceive(MSG_Q_ID msgQId,
		void *buffer,
		unsigned maxBytes,
		unsigned timeout)
{
  MSG_NODE *pMsg;
  int bytesRet;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  /* Lock */
  taskLock();

msgQReceiveLoop:

  if (OBJ_VERIFY(msgQId, msgQClassId) != OK)
  {
    taskUnlock();
    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Get next message from message queue */
  pMsg = (MSG_NODE *) qMsgGet(msgQId, &msgQId->msgQ, timeout);
  if (pMsg == (MSG_NODE *) NONE)
  {
    goto msgQReceiveLoop;
  }

  if (pMsg == NULL)
  {
    msgQId->sendTimeouts++;
    taskUnlock();
    /* errno set by qMsgGet() */
    return(ERROR);
  }

  /* Store data */
  bytesRet = min(pMsg->msgLength, maxBytes);
  memcpy(buffer, MSG_NODE_DATA(pMsg), bytesRet);

  /* Put node back on free queue */
  qMsgPut(msgQId, &msgQId->freeQ, &pMsg->node, 1);

  taskUnlock();

  return(bytesRet);
}

/*******************************************************************************
* msgQNumMsgs - Get number of messages on message queue
*
* RETURNS: Number of messages on queue
*******************************************************************************/

int msgQNumMsgs(MSG_Q_ID msgQId)
{
  /* Verify object class */
  if ( OBJ_VERIFY(msgQId, msgQClassId) != OK )
    return ERROR;

  return msgQId->msgQ.count;
}

