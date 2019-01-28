/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2008 - 2010 Surplus Users Ham Society
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

/* selectLib.c - Select Library */

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <rtos/errnoLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/errnoLib.h>
#include <io/ioLib.h>
#include <io/ttyLib.h>
#include <os/excLib.h>
#include <os/selectLib.h>

/* Imports */
IMPORT TCB_ID taskIdCurrent;
IMPORT int iosMaxFd;

IMPORT FUNCPTR _func_logMsg;
IMPORT FUNCPTR _func_selWakeupListInit;
IMPORT FUNCPTR _func_selWakeupListTerminate;
IMPORT FUNCPTR _func_selWakeupAll;

IMPORT int sysClockRateGet(void);

/* Locals */
LOCAL int selMutexOptions = SEM_Q_FIFO | SEM_DELETE_SAFE;

LOCAL STATUS selTaskCreateHook(TCB_ID pTcb);
LOCAL void selTaskDeleteHook(TCB_ID tcbId);
LOCAL STATUS selIoctl(fd_set *pFdSet,
		      int width,
		      int func,
		      SEL_WAKEUP_NODE *pWakeupNode,
		      BOOL stopOnError);

/******************************************************************************
* selectLibInit - Initialize select library
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS selectLibInit(void)
{
  if (taskCreateHookAdd((FUNCPTR) selTaskCreateHook) != OK) {

    if (_func_logMsg != NULL)
      _func_logMsg("selectLibInit: couldn't install task create hook!\n",
		   (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0,(ARG) 0, (ARG) 0);
    return(ERROR);

  }

  /* Initialize select context for current task */
  if (taskIdCurrent->selectContextId == NULL) {

    if (selTaskCreateHook(taskIdCurrent) != OK) {

    if (_func_logMsg != NULL)
      _func_logMsg("selectLibInit: couldn't install create hook for the "
		    "caller! %#x\n",
		    (ARG) 0, (ARG) 0, (ARG) 0,(ARG) 0, (ARG) 0, (ARG) 0);
      return(ERROR);

    }

  }

  /* Install global functions */
  _func_selWakeupListInit = (FUNCPTR) selWakeupListInit;
  _func_selWakeupListTerminate = (FUNCPTR) selWakeupListTerminate;
  _func_selWakeupAll = (FUNCPTR) selWakeupAll;

  return(OK);
}

/******************************************************************************
* selectLibInitDelete - Initialize select library delete hook
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS selectLibInitDelete(void)
{
  /* If failed to install hook */
  if (taskDeleteHookAdd((FUNCPTR) selTaskDeleteHook) != OK) {

    /* If logging installed */
    if (_func_logMsg) {

      ( *_func_logMsg) ("selectLibInitDelete: couldn't install task delete"
			"hook\n",
			(ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);
      return ERROR;

    } /* End if logging installed */

  } /* End if failed to install hook */

  return OK;
}

/******************************************************************************
* select - Select on a set of file descriptors
*
* RETURNS: Number of file descritors, zero if timeout or ERROR
******************************************************************************/

int select(int width,
	   fd_set *pReadFds,
	   fd_set *pWriteFds,
	   fd_set *pExceptFds,
	   struct timeval *pTimeOut)
{
  SEL_CONTEXT_ID selectContextId;
  SEL_WAKEUP_NODE wNode;
  int fd, msk, bw, status;
  int nFound, qTime, clockRate;

  /* Initialize locals */
  selectContextId = taskIdCurrent->selectContextId;
  nFound = 0;
  qTime = 0;

  /* Check select context */
  if (selectContextId == NULL) {

    errnoSet(S_selectLib_NO_SELECT_CONTEXT);
    return(ERROR);

  }

  /* Check width */
  if ( (width < 0) || ( (width > iosMaxFd) && (width > FD_SETSIZE) ) ) {

    errnoSet(S_selectLib_WIDTH_OUT_OF_RANGE);
    return(ERROR);

  }

  /* Adjust width */
  if (width > iosMaxFd)
    width = iosMaxFd;

  /* Get max width in bytes */
  bw = howmany(width, NFDBITS) * sizeof(fd_mask);

  /* Copy or erase original read filedescriptors */
  if (pReadFds != NULL)
    memcpy(selectContextId->pOrigReadFds, pReadFds, bw);
  else
    memset(selectContextId->pOrigReadFds, 0, bw);

  /* Copy or erase original write filedescriptors */
  if (pWriteFds != NULL)
    memcpy(selectContextId->pOrigWriteFds, pWriteFds, bw);
  else
    memset(selectContextId->pOrigWriteFds, 0, bw);

  /* If time info argument given */
  if (pTimeOut != NULL) {

    clockRate = sysClockRateGet();

    /* Convert */
    qTime = (pTimeOut->tv_sec * clockRate) +
	((((pTimeOut->tv_usec * clockRate) / 100) / 100) / 100);

  } /* If time info agument given */

  /* Initialize select context */
  selectContextId->pReadFds = pReadFds;
  selectContextId->pWriteFds = pWriteFds;

  semTake(&selectContextId->wakeupSync, WAIT_NONE);

  wNode.taskId = taskIdSelf();

  /* Clear read file descriptor set */
  if (pReadFds != NULL)
    memset(pReadFds, 0, bw);

  /* Clear write file descriptor set */
  if (pWriteFds != NULL)
    memset(pWriteFds, 0, bw);

  /* Clear except file descriptor set */
  if (pExceptFds != NULL)
    memset(pExceptFds, 0, bw);

  /* Reset status flag */
  status = OK;

  /* If read file descriptors */
  if (pReadFds != NULL) {

    wNode.type = SELREAD;
    if (selIoctl(selectContextId->pOrigReadFds, width,
		 FIOSELECT, &wNode, TRUE) != OK)
      status = ERROR;

  } /* End if read file descriptors */

  /* If not error */
  if (status != ERROR) {

    /* If write file descriptors */
    if (pWriteFds != NULL) {

      wNode.type = SELWRITE;
      if (selIoctl(selectContextId->pOrigWriteFds, width,
		   FIOSELECT, &wNode, TRUE) != OK)
        status = ERROR;

    } /* End if write file descriptors */

  } /* End if not error */

  /* If error */
  if (status != OK) {

    /* Get errno */
    status = errnoGet();

    /* Unselect read file descriptors */
    wNode.type = SELREAD;
    selIoctl(selectContextId->pOrigReadFds, width, FIOUNSELECT,
	     &wNode, FALSE);

    /* Unselect write file descriptors */
    wNode.type = SELWRITE;
    selIoctl(selectContextId->pOrigWriteFds, width, FIOUNSELECT,
	     &wNode, FALSE);

    if (errnoGet() == S_ioLib_UNKNOWN_REQUEST)
      errnoSet(S_selectLib_NO_SELECT_SUPPORT_IN_DRIVER);

    return(ERROR);

  } /* End if error */

  /* Get requested timeout */
  if ( (pTimeOut != NULL) && (qTime == 0) )
    qTime = WAIT_NONE;
  else if (pTimeOut == NULL)
    qTime = WAIT_FOREVER;

  /* Indicate task pending on selection */
  selectContextId->width = width;
  selectContextId->pendedOnSelect = TRUE;

  semTake(&selectContextId->wakeupSync, qTime);

  /* Reset status flag */
  status = OK;

  /* If read file descriptors */
  if (pReadFds != NULL) {

    wNode.type = SELREAD;
    if (selIoctl(selectContextId->pOrigReadFds, width,
		 FIOUNSELECT, &wNode, FALSE) != OK)
      status = ERROR;

  } /* End if read file descriptors */

  /* If write file descriptors */
  if (pWriteFds != NULL) {

    wNode.type = SELWRITE;
    if (selIoctl(selectContextId->pOrigWriteFds, width,
		 FIOUNSELECT, &wNode, FALSE) != OK)
      status = ERROR;

  } /* End if write file descriptors */

  /* Mark as not pended on select */
  selectContextId->pendedOnSelect = FALSE;

  /* Return on error here */
  if (status == ERROR)
    return(ERROR);

  /* If read file descriptors */
  if (pReadFds != NULL) {

    /* For all file descriptors */
    for (fd = 0; fd < width; fd++) {

      msk = pReadFds->fds_bits[((unsigned) fd) / NFDBITS];
      if (msk == 0)
        fd += NFDBITS - 1;
      else if (msk & (1 << ( ((unsigned) fd) % NFDBITS) ) )
        nFound++;

    } /* End for all file descriptors */

  } /* End if read file descriptors */

  /* If write file descriptors */
  if (pWriteFds != NULL) {

    /* For all file descriptors */
    for (fd = 0; fd < width; fd++) {

      msk = pWriteFds->fds_bits[((unsigned) fd) / NFDBITS];
      if (msk == 0)
        fd += NFDBITS - 1;
      else if (msk & (1 << ( ((unsigned) fd) % NFDBITS) ) )
        nFound++;

    } /* End for all file descriptors */

  } /* End if read file descriptors */

  return nFound;
}

/******************************************************************************
* selWakeupListInit - Initialize wakeup list
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS selWakeupListInit(SEL_WAKEUP_LIST *pList)
{
  if (listInit(&pList->wakeupList) != OK)
    return(ERROR);

  if (semMInit(&pList->mutex, selMutexOptions) != OK)
    return(ERROR);
}

/******************************************************************************
* selWakeupListTerminate - Terminate wakeup list
*
* RETURNS: N/A
******************************************************************************/

void selWakeupListTerminate(SEL_WAKEUP_LIST *pList)
{
  semTerminate(&pList->mutex);
}

/******************************************************************************
* selWakeupListLen - Get wakeup list size
*
* RETURNS: Number of nodes
******************************************************************************/

int selWakeupListLen(SEL_WAKEUP_LIST *pList)
{
  return(listCount(&pList->wakeupList));
}

/******************************************************************************
* selNodeAdd - Add to wakeup list
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS selNodeAdd(SEL_WAKEUP_LIST *pList, SEL_WAKEUP_NODE *pNode)
{
  BOOL dontFree;
  SEL_WAKEUP_NODE *pCpNode;

  semTake(&pList->mutex, WAIT_FOREVER);

  /* Check if zero */
  if (listCount(&pList->wakeupList) == 0) {
    pCpNode = &pList->first;
    dontFree = TRUE;
  } else {
    pCpNode = (SEL_WAKEUP_NODE *) malloc( sizeof(SEL_WAKEUP_NODE) );
    dontFree = FALSE;
  }

  if (pCpNode == NULL) {
    semGive(&pList->mutex);
    return(ERROR);
  }

  /* Copy node */
  *pCpNode = *pNode;
  pCpNode->dontFree = dontFree;

  /* Add to list */
  listAdd(&pList->wakeupList, (LIST_NODE *) pCpNode);

  semGive(&pList->mutex);

  return(OK);
}

/******************************************************************************
* selNodeDelete - Delete from wakeup list
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS selNodeDelete(SEL_WAKEUP_LIST *pList, SEL_WAKEUP_NODE *pNode)
{
  SEL_WAKEUP_NODE *pI;

  semTake(&pList->mutex, WAIT_FOREVER);

  for (pI = (SEL_WAKEUP_NODE *) LIST_HEAD(&pList->wakeupList);
       pI != NULL;
       pI = (SEL_WAKEUP_NODE *) LIST_NEXT((LIST_NODE *) pI)) {

    /* Check for match */
    if ( (pI->taskId == pNode->taskId) && (pI->type == pNode->type) ) {

      /* Delete node */
      listRemove(&pList->wakeupList, (LIST_NODE *) pI);

      if (!pI->dontFree)
        free(pI);

      semGive(&pList->mutex);

      return(OK);
    }
  }

  semGive(&pList->mutex);

  return(ERROR);
}

/******************************************************************************
* selWakeup - Wakeup a task sleeping on file descriptor
*
* RETURNS: N/A
******************************************************************************/

void selWakeup(SEL_WAKEUP_NODE *pNode)
{
  TCB_ID pTcb;
  SEL_CONTEXT_ID selectContextId;

  /* Get current select context */
  pTcb = (TCB_ID) pNode->taskId;
  selectContextId = pTcb->selectContextId;

  /* Select type */
  switch(pNode->type) {

    case SELREAD : FD_SET(pNode->fd, selectContextId->pReadFds);
		   break;

    case SELWRITE: FD_SET(pNode->fd, selectContextId->pWriteFds);
		   break;
  }

  semGive(&selectContextId->wakeupSync);
}

/******************************************************************************
* selWakeupAll - Wakeup all tasks sleeping in wakeup list of specified type
*
* RETURNS: N/A
******************************************************************************/

void selWakeupAll(SEL_WAKEUP_LIST *pList, SELECT_TYPE type)
{
  SEL_WAKEUP_NODE *pNode;

  /* Check if any */
  if (listCount(&pList->wakeupList) == 0)
    return;

  /* If in interrupt */
  if (INT_CONTEXT()) {

    excJobAdd(selWakeupAll, (ARG) pList, (ARG) type,
			    (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);
    return;

  }

  semTake(&pList->mutex, WAIT_FOREVER);

  /* Wakeup all of correct type */
  for (pNode = (SEL_WAKEUP_NODE *) LIST_HEAD(&pList->wakeupList);
       pNode != NULL;
       pNode = (SEL_WAKEUP_NODE *) LIST_NEXT((LIST_NODE *) pNode)) {
    if (pNode->type == type)
      selWakeup(pNode);
  }

  semGive(&pList->mutex);
}

/******************************************************************************
* selWakeupType - Get wakeup node type
*
* RETURNS: Node type
******************************************************************************/

SELECT_TYPE selWakeupType(SEL_WAKEUP_NODE *pNode)
{
  return(pNode->type);
}

/******************************************************************************
* selTaskCreateHook - Run for every created task
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS selTaskCreateHook(TCB_ID pTcb)
{
  SEL_CONTEXT *pSelectContext;
  int fdSetBytes;

  /* Allocate memory */
  fdSetBytes = sizeof (fd_mask) * howmany(iosMaxFd, NFDBITS);
  pSelectContext = (SEL_CONTEXT *) malloc(sizeof(SEL_CONTEXT) + 2 * fdSetBytes);
  if (pSelectContext == NULL)
    return(ERROR);

  /* Initialize structure */
  pSelectContext->pOrigReadFds = (fd_set *) ((int) pSelectContext +
						sizeof(SEL_CONTEXT));
  pSelectContext->pOrigWriteFds = (fd_set *)
					((int) pSelectContext->pOrigReadFds +
					fdSetBytes);
  pSelectContext->pendedOnSelect = FALSE;

  /* Initialize sempahore */
  if (semBInit(&pSelectContext->wakeupSync, SEM_Q_PRIORITY, SEM_EMPTY) != OK) {
    free(pSelectContext);
    return(ERROR);
  }

  /* Setup pointer in tcb */
  pTcb->selectContextId = pSelectContext;

  return(OK);
}

/******************************************************************************
* selTaskDeleteHook - Run for every deleted task
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL void selTaskDeleteHook(TCB_ID tcbId)
{
  SEL_WAKEUP_NODE wNode;
  SEL_CONTEXT_ID selectContextId;

  /* Get select context from task to be deleted */
  selectContextId = tcbId->selectContextId;

  if (selectContextId == NULL)
    return;

  /* If pending on select */
  if (selectContextId->pendedOnSelect) {

    /* Store task id in wakeup node */
    wNode.taskId = (int) tcbId;

    /* Unselect all read file descriptors */
    wNode.type = SELREAD;
    selIoctl(selectContextId->pOrigReadFds, selectContextId->width,
	     FIOUNSELECT, &wNode, FALSE);

    /* Unselect all write file descriptors */
    wNode.type = SELWRITE;
    selIoctl(selectContextId->pOrigWriteFds, selectContextId->width,
	     FIOUNSELECT, &wNode, FALSE);

  } /* End if peding on select */

  /* Terminate semaphore */
  semTerminate(&selectContextId->wakeupSync);

  /* Free memory */
  free(selectContextId);
}

/******************************************************************************
* selIoctl - Perform ioctl on all file descriptors in mask
*
* RETURNS: OK or ERROR
******************************************************************************/

LOCAL STATUS selIoctl(fd_set *pFdSet,
		      int width,
		      int func,
		      SEL_WAKEUP_NODE *pWakeupNode,
		      BOOL stopOnError)
{
  int fd, msk;
  int status;

  /* Reset status */
  status = OK;

  /* For all file descritptors */
  for (fd = 0; fd < width; fd++) {

    /* Get long word from fd_set */
    msk = pFdSet->fds_bits[((unsigned) fd) / NFDBITS];

    /* If zero mask */
    if (msk == 0) {

      fd += NFDBITS - 1;

    } /* End if zero mask */

    /* Else if mask */
    else if (msk & (1 << ( ((unsigned) fd) % NFDBITS) ) ) {

      /* Store wakeup file descriptor */
      pWakeupNode->fd = fd;

      /* If ioctl performed on file descriptor */
      if ( ioctl(fd, func, (int) pWakeupNode) != OK) {

        status = ERROR;
        if (stopOnError)
          break;

      } /* End if ioctl performed on file descriptor */

    } /* End else if mask */

  } /* End for all file descriptors */

  return status;
}

