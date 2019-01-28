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

/* msgQShow.c - Message queue show facilities */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/classLib.h>
#include <util/qLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskInfo.h>
#include <rtos/eventShow.h>
#include <rtos/msgQLib.h>
#include <rtos/msgQShow.h>

/* Defines */

/* Imports */
IMPORT Q_HEAD kernTickQ;

/* Locals */
LOCAL BOOL msgQInfoEach(MSG_NODE *pNode, MSG_Q_INFO *pInfo);

/* Globals */

/* Functions */

/*******************************************************************************
 * msgQShowInit - Initialize show message queue info
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS msgQShowInit(void)
{
  return classShowConnect(msgQClassId, (FUNCPTR) msgQShow);
}

/*******************************************************************************
 * msgQInfoGet - Get information about message queue
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS msgQInfoGet(MSG_Q_ID msgQId, MSG_Q_INFO *pInfo)
{
  int level;
  Q_HEAD *pendQ;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify object class */
  if (OBJ_VERIFY(msgQId, msgQClassId) != OK) {

    INT_UNLOCK(level);
    return ERROR;

  }

  /* If no messages */
  if (!msgQId->msgQ.count)
    pendQ = &msgQId->msgQ.pendQ;
  else
    pendQ = &msgQId->freeQ.pendQ;

  /* If task id list requested */
  if (pInfo->taskIdList != NULL)
    Q_INFO(pendQ, pInfo->taskIdList, pInfo->taskIdListMax);

  /* If message list or number requested */
  if ( (pInfo->msgPtrList != NULL) || (pInfo->msgLengthList != NULL) ) {

    pInfo->numMsg = 0;
    if (pInfo->msgListMax > 0)
      Q_EACH(&msgQId->msgQ, msgQInfoEach, pInfo);

  } /* End if message list or numer requested */

  /* Setup info structure */
  pInfo->numMsg = msgQId->msgQ.count;
  pInfo->numTask = Q_INFO(pendQ, NULL, 0);
  pInfo->options = msgQId->options;
  pInfo->maxMsg = msgQId->maxMsg;
  pInfo->maxMsgLength = msgQId->maxMsgLength;
  pInfo->sendTimeouts = msgQId->sendTimeouts;
  pInfo->reciveTimeouts = msgQId->reciveTimeouts;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  return OK;
}

/*******************************************************************************
 * msgQShow - Show message queue info
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS msgQShow(MSG_Q_ID msgQId, int mode)
{
  MSG_Q_INFO info;
  TCB_ID tcbId;
  char tmpString[15];
  EVENTS_RESOURCE msgQRes;
  int level;
  STATUS status;
  int taskIdList[20], taskDList[20];
  char *msgPtrList[20];
  int msgLengthList[20];
  int i, j, len;
  char *pMsg;

  /* Clear info */
  memset(&info, 0, sizeof(info));

  /* If mode ge. 1 */
  if (mode >= 1) {

    /* Get info for each message, task pending */
    info.taskIdList = taskIdList;
    info.taskIdListMax = NELEMENTS(taskIdList);
    info.msgPtrList = msgPtrList;
    info.msgLengthList = msgLengthList;
    info.msgListMax = NELEMENTS(msgPtrList);

  } /* End if mode ge. 1 */

  /* Lock interrupts */
  INT_LOCK(level);

  /* Get message queue info structure */
  status = msgQInfoGet(msgQId, &info);
  if (status != OK) {

    INT_UNLOCK(level);
    printf("Invalid message queue id: %#x\n", (int) msgQId);
    return ERROR;

  }

  /* If show tasks pending */
  if ( (info.numTask > 0) && (mode >= 1) ) {

    /* For all in id list */
    for (i = 0;
         i < min( info.numTask, NELEMENTS(taskIdList) );
         i++) {

      /* Get tcb */
      tcbId = (TCB_ID) taskIdList[i];
      if (tcbId->status & TASK_DELAY)
        taskDList[i]  = Q_KEY(&kernTickQ, &tcbId->tickNode, 1);
      else
        taskDList[i] = 0;

    } /* End for all in id list */

  } /* End if shos tasks pending */

  /* Get events resource */
  msgQRes = msgQId->events;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  /* Get options string */
  if ( (info.options & MSG_Q_TYPE_MASK) == MSG_Q_FIFO)
    strcpy(tmpString, "MSG_Q_FIFO");
  else
    strcpy(tmpString, "MSG_Q_PRIORITY");

  /* Print summary */
  printf("\n");

  printf("Message Queue Id    : 0x%-10x\n", (int) msgQId);

  if ( (info.options & MSG_Q_TYPE_MASK) == MSG_Q_FIFO )
    printf("Task Queuing        : %-10s\n", "FIFO");
  else
    printf("Task Queuing        : %-10s\n", "PRIORITY");

  printf("Message Byte Len    : %-10d\n", info.maxMsgLength);
  printf("Messages Max        : %-10d\n", info.maxMsg);
  printf("Messages Queued     : %-10d\n", info.numMsg);

  if (info.numMsg == info.maxMsg)
    printf("Senders Blocked     : %-10d\n", info.numTask);
  else
    printf("Receivers Blocked   : %-10d\n", info.numTask);

  printf("Send timeouts       : %-10d\n", info.sendTimeouts);
  printf("Receive timeouts    : %-10d\n", info.reciveTimeouts);
  printf("Options             : 0x%x\t%s\n", info.options, tmpString);
  if ( (info.options & MSG_Q_EVENTSEND_ERROR_NOTIFY) != 0)
    printf("\t\t\t\tMSG_Q_EVENTSEND_ERROR_NOTIFY\n");

  /* Show resource events */
  eventResourceShow(&msgQRes);

  /* If detailed info requested */
  if (mode >= 1) {

    /* If tasks pending */
    if (info.numTask) {

      /* Get sender/receiver string */
      if (info.numMsg == info.maxMsg)
        strcpy(tmpString, "Senders");
      else
        strcpy(tmpString, "Receivers");

      printf("\n%s Blocked:\n", tmpString);
      printf("   NAME      TID    PRI TIMEOUT\n");
      printf("---------- -------- --- -------\n");

      /* For all tasks */
      for (i = 0; i < min( info.numTask, NELEMENTS(taskIdList) ); i++)
        printf("%-11.11s%8x %3d %7u\n",
	       taskName(taskIdList[i]),
	       taskIdList[i],
	       ( (TCB_ID) taskIdList[i])->priority,
	       taskDList[i]);

    } /* End if tasks pending */

    /* If messages pending */
    if (info.numMsg > 0) {

      printf("\nMessages queued:\n"
	     "  #   address length value\n");

      /* For all messages */
      for (i = 0; i < min( info.numMsg, NELEMENTS(msgPtrList) ); i++) {

        /* Get message and length */
        pMsg = msgPtrList[i];
        len = msgLengthList[i];

        printf("%3d %#10x %4d ", i + 1, (int) pMsg, len);

        /* For length */
        for (j = 0; j < min(len, 20); j++) {

          if ( (j % 4) == 0)
            printf(" 0x");

          printf("%02x", pMsg[j] & 0xff);

        } /* End for length */

        if (len > 20)
          printf(" ...");

        printf("\n");

      } /* End for all messages */

    } /* End if messages pending */

  } /* End if detailed info requested */

  printf("\n");

  return OK;
}

/*******************************************************************************
 * msgQInfoEach - Run for each message in message queue list
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL msgQInfoEach(MSG_NODE *pNode, MSG_Q_INFO *pInfo)
{
  /* If message list requested */
  if (pInfo->msgPtrList != NULL)
    pInfo->msgPtrList[pInfo->numMsg] = MSG_NODE_DATA(pNode);

  /* If message length requested */
  if (pInfo->msgLengthList != NULL)
    pInfo->msgLengthList[pInfo->numMsg] = pNode->msgLength;

  /* In more info to get */
  if (++pInfo->numMsg < pInfo->msgListMax);
    return TRUE;

  return FALSE;
}

