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

/* pipeDrv.c - Pipe device driver */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <rtos/errnoLib.h>
#include <rtos/taskLib.h>
#include <rtos/semLib.h>
#include <rtos/msgQLib.h>
#include <rtos/msgQShow.h>
#include <util/listLib.h>
#include <os/pipeDrv.h>

/* Defines */

/* Imports */
IMPORT FUNCPTR _func_selWakeupListInit;
IMPORT FUNCPTR _func_selWakeupAll;

/* Locals */
LOCAL int pipeDrvNum = ERROR;

LOCAL int pipeCreate(PIPE_DEV *pPipeDev, const char *name, mode_t mode,
		     const char *symlink);
LOCAL int pipeDelete(PIPE_DEV *pPipeDev, const char *name, mode_t mode);
LOCAL int pipeOpen(PIPE_DEV *pPipeDev, const char *name, int flags,
		   mode_t mode);
LOCAL int pipeClose(PIPE_DEV *pPipeDev);
LOCAL int pipeRead(PIPE_DEV *pPipeDev, char *buf, unsigned int maxBytes);
LOCAL int pipeWrite(PIPE_DEV *pPipeDev, char *buf, int maxBytes);
LOCAL STATUS pipeIoctl(PIPE_DEV *pPipeDev, int func, ARG arg);

/* Globals */
int pipeDrvMsgQOptions = MSG_Q_FIFO;

/* Functions */

/*******************************************************************************
 * pipeDrvInit- Install pipe device
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS pipeDrvInit(void)
{
  if (pipeDrvNum == ERROR)
    pipeDrvNum = iosDrvInstall((FUNCPTR) pipeCreate,
			       (FUNCPTR) pipeDelete,
			       (FUNCPTR) pipeOpen,
			       (FUNCPTR) pipeClose,
			       (FUNCPTR) pipeRead,
			       (FUNCPTR) pipeWrite,
			       (FUNCPTR) pipeIoctl);
  if (pipeDrvNum == ERROR)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * pipeDevCreate - Create pipe device
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS pipeDevCreate(char *name, int maxMsg, int maxBytes)
{
  PIPE_DEV *pPipeDev;

  /* Not callable from interrupts */
  if ( INT_RESTRICT() != OK )
    return ERROR;

  /* If invalid driver number */
  if (pipeDrvNum == ERROR)
    return ERROR;

  /* Allocate memory for pipe */
  pPipeDev = (PIPE_DEV *) malloc(sizeof(PIPE_DEV) +
				 maxMsg * MSG_NODE_SIZE(maxBytes));
  if (pPipeDev == NULL)
    return ERROR;

  /* Initailize data structure */
  pPipeDev->numOpens = 0;

  /* Initalize message queue */
  if ( msgQInit(&pPipeDev->msgQ, maxMsg, maxBytes, pipeDrvMsgQOptions,
		(void *) ( ((char *) pPipeDev) + sizeof(PIPE_DEV) ) ) != OK) {

    free(pPipeDev);
    return ERROR;

  }

  /* Initalize select wakup list */
  if (_func_selWakeupListInit != NULL)
    ( *_func_selWakeupListInit) (&pPipeDev->selWakeupList);

  /* Install driver */
  if ( iosDevAdd(&pPipeDev->devHeader, name, pipeDrvNum) != OK) {

    msgQTerminate(&pPipeDev->msgQ);
    free(pPipeDev);
    return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * pipeDevDelete - Delete pipe device
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS pipeDevDelete(char *name, BOOL force)
{
  PIPE_DEV *pPipeDev;
  char *pTail;
  SEL_WAKEUP_NODE *wNode;

  /* Not callable from interrupts */
  if ( INT_RESTRICT() != OK )
    return ERROR;

  /* If invalid driver number */
  if (pipeDrvNum == ERROR)
    return ERROR;

  /* Find device */
  pPipeDev = (PIPE_DEV *) iosDevFind(name, &pTail);
  if (pPipeDev == NULL)
    return ERROR;

  /* If not forced delete */
  if (!force) {

    if (pPipeDev->numOpens != 0) {

      errnoSet(EMFILE);
      return ERROR;

    }

    if ( selWakeupListLen(&pPipeDev->selWakeupList) != 0 ) {

      errnoSet(EBUSY);
      return ERROR;

    }

  } /* End if not forced delete */

  /* Delete device */
  iosDevDelete(&pPipeDev->devHeader);

  /* If forced and a threads selects on this pipe */
  if ( force && (selWakeupListLen(&pPipeDev->selWakeupList) != 0) ) {

    wNode = (SEL_WAKEUP_NODE *) LIST_HEAD(&pPipeDev->selWakeupList);
    do {

      selNodeDelete(&pPipeDev->selWakeupList, wNode);

    } while ( (wNode = (SEL_WAKEUP_NODE *) LIST_NEXT(wNode)) != NULL);

    selWakeupListTerminate(&pPipeDev->selWakeupList);

  } /* End if forced and a thread selects on this pipe */

  /* Terminate message queue */
  msgQTerminate(&pPipeDev->msgQ);

  /* Free struct */
  free(pPipeDev);

  return OK;
}

/*******************************************************************************
 * pipeCreate - Create not supported
 *
 * RETURNS: ERROR
 ******************************************************************************/

LOCAL int pipeCreate(PIPE_DEV *pPipeDev, const char *name, mode_t mode,
		     const char *symlink)
{
  return ERROR;
}

/*******************************************************************************
 * pipeDelete - Delete not supported
 *
 * RETURNS: ERROR
 ******************************************************************************/

LOCAL int pipeDelete(PIPE_DEV *pPipeDev, const char *name, mode_t mode)
{
  return ERROR;
}

/*******************************************************************************
 * pipeOpen - Open a pipe
 *
 * RETURNS: Pointer to pipe device or ERROR
 ******************************************************************************/

LOCAL int pipeOpen(PIPE_DEV *pPipeDev, const char *name, int flags, mode_t mode)
{
  /* Increase num opens */
  ++pPipeDev->numOpens;

  return (int) pPipeDev;
}

/*******************************************************************************
 * pipeClose - Close a pipe
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL int pipeClose(PIPE_DEV *pPipeDev)
{
  /* Check pipe device */
  if (pPipeDev == NULL)
    return ERROR;

  /* Descrease num opens */
  if (pPipeDev->numOpens > 0)
    --pPipeDev->numOpens;

  return OK;
}

/*******************************************************************************
 * pipeRead - Read from a pipe
 *
 * RETURNS: Number of bytes read or ERROR
 ******************************************************************************/

LOCAL int pipeRead(PIPE_DEV *pPipeDev, char *buf, unsigned int maxBytes)
{
  int nBytes;

  /* Block for messages to arrive on pipe */
  nBytes = msgQReceive(&pPipeDev->msgQ, buf, maxBytes, WAIT_FOREVER);
  if (nBytes == ERROR)
    return ERROR;

  /* Wakeup select */
  if (_func_selWakeupAll != NULL)
    ( *_func_selWakeupAll) (&pPipeDev->selWakeupList, SELWRITE);

  return nBytes;
}

/*******************************************************************************
 * pipeWrite - Write to pipe
 *
 * RETURNS: Number of bytes written or ERROR
 ******************************************************************************/

LOCAL int pipeWrite(PIPE_DEV *pPipeDev, char *buf, int maxBytes)
{
  int timeout;

  /* Lock task if from interrupt */
  if ( INT_CONTEXT() )
    taskLock();

  /* Get timeout */
  if ( INT_CONTEXT() )
    timeout = WAIT_NONE;
  else
    timeout = WAIT_FOREVER;

  /* Send message on message queue */
  if ( msgQSend(&pPipeDev->msgQ, buf, (unsigned) maxBytes,
		timeout, MSG_PRI_NORMAL) != OK) {

    if ( !INT_CONTEXT() )
      taskUnlock();

    return ERROR;
  }

  /* If not called from interrput */
  if ( !INT_CONTEXT() ) {

    semTake(&pPipeDev->selWakeupList.mutex, WAIT_FOREVER);
    taskUnlock();

  }

  /* Wakeup select */
  if (_func_selWakeupAll != NULL)
    ( *_func_selWakeupAll) (&pPipeDev->selWakeupList, SELREAD);

  if ( !INT_CONTEXT() )
    semGive(&pPipeDev->selWakeupList.mutex);

  return maxBytes;
}

/*******************************************************************************
 * pipeIoctl - Perform ioctl on pipe
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS pipeIoctl(PIPE_DEV *pPipeDev, int func, ARG arg)
{
  MSG_Q_INFO msgQInfo;
  SEL_WAKEUP_NODE *wNode;

  /* Select function */
  switch (func) {

    /* Number of messages on pipe */
    case FIONMSGS:
      //*(int *) arg = msgQNumMsgs(&pPipeDev->msgQ);
    break;

    /* Get number of bytes in first message on pipe */
    case FIONREAD:
      memset(&msgQInfo, 0, sizeof(msgQInfo));
      msgQInfo.msgListMax = 1;
      msgQInfo.msgLengthList = (int *) arg;
      *(int *) arg = 0;
      msgQInfoGet(&pPipeDev->msgQ, &msgQInfo);
    break;

    /* Flush all messages */
    case FIOFLUSH:
      taskLock();
      while ( msgQReceive(&pPipeDev->msgQ, NULL, 0, WAIT_NONE) != ERROR);
      taskUnlock();
    break;

    /* Perform select on pipe */
    case FIOSELECT:
      wNode = (SEL_WAKEUP_NODE *) arg;
      selNodeAdd(&pPipeDev->selWakeupList, wNode);
      switch(wNode->type) {
        case SELREAD:
          if (msgQNumMsgs(&pPipeDev->msgQ) > 0)
            selWakeup(wNode);
        break;

        case SELWRITE:
          if (pPipeDev->msgQ.maxMsg > msgQNumMsgs(&pPipeDev->msgQ))
            selWakeup(wNode);
        break;
      }
    break;

    /* Unselect pipe */
    case FIOUNSELECT:
      wNode = (SEL_WAKEUP_NODE *) arg;
      selNodeDelete(&pPipeDev->selWakeupList, wNode);
    break;

    default:
      errnoSet(S_ioLib_UNKNOWN_REQUEST);
      return ERROR;
    break;

  } /* End select function */

  return OK;
}

