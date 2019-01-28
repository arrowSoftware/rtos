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

/* taskLib.c - Task handeling library */

#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <util/classLib.h>
#include <util/objLib.h>
#include <util/qLib.h>
#include <util/qPrioLib.h>
#include <rtos/memPartLib.h>
#include <rtos/rtosLib.h>
#include <arch/intArchLib.h>
#include <os/sigLib.h>
#include <rtos/kernQLib.h>
#include <rtos/taskHookLib.h>
#include <arch/taskArchLib.h>
#include <rtos/taskLib.h>

/* Defines */
#define TASK_EXTRA_BYTES		16

/* Imports */
IMPORT BOOL kernelState;
IMPORT STATUS kernExit(void);
IMPORT FUNCPTR taskCreateHooks[];
IMPORT FUNCPTR taskDeleteHooks[];
IMPORT TCB_ID taskIdCurrent;
IMPORT Q_HEAD kernReadyQ;
IMPORT FUNCPTR _func_excJobAdd;
IMPORT FUNCPTR _func_sigTimeoutRecalc;

/* Locals */
LOCAL OBJ_CLASS taskClass;
LOCAL BOOL taskLibInstalled = FALSE;
LOCAL int namelessCount = 0;

/* Globals */
CLASS_ID taskClassId = &taskClass;
char namelessPrefix[] = "t";
char restartTaskName[] = "tRestart";
int restartTaskPriority = 0;
int restartTaskStackSize = 6000;
int restartTaskOptions = TASK_OPTIONS_UNBREAKABLE |
			 TASK_OPTIONS_NO_STACK_FILL;

/*******************************************************************************
* taskLibInit - Initialize task library
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskLibInit(void)
{
  if (taskLibInstalled) {
    return OK;
  }

  if (classInit(taskClassId,
		STACK_ROUND_UP(sizeof(TCB) + TASK_EXTRA_BYTES),
		OFFSET(TCB, objCore),
		memSysPartId,
		(FUNCPTR) taskCreat,
		(FUNCPTR) taskInit,
		(FUNCPTR) taskDestroy) != OK) {
    /* errno set by classInit() */
    return ERROR;
  }

  taskLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
* taskSpawn - Create a new task and start it
*
* RETURNS: Integer taskId or NULL
*******************************************************************************/

int taskSpawn(const char *name,
	      unsigned priority,
	      int options,
	      unsigned stackSize,
	      FUNCPTR func,
	      ARG arg0,
	      ARG arg1,
	      ARG arg2,
	      ARG arg3,
	      ARG arg4,
	      ARG arg5,
	      ARG arg6,
	      ARG arg7,
	      ARG arg8,
	      ARG arg9)
{
  int taskId;

  /* Create task */
  taskId = taskCreat(name, priority, options, stackSize, func,
		     arg0,
		     arg1,
		     arg2,
		     arg3,
		     arg4,
		     arg5,
		     arg6,
		     arg7,
		     arg8,
		     arg9);
  if (taskId == (int) NULL)
  {
    /* errno set by taskCreat() */
    return ((int) NULL);
  }

  /* Then start it */
  taskActivate(taskId);

  return taskId;
}

/*******************************************************************************
* taskCreat - Create a new task without starting it
*
* RETURNS: Integer taskId or NULL
*******************************************************************************/

int taskCreat(const char *name,
	      unsigned priority,
	      int options,
	      unsigned stackSize,
	      FUNCPTR func,
	      ARG arg0,
	      ARG arg1,
	      ARG arg2,
	      ARG arg3,
	      ARG arg4,
	      ARG arg5,
	      ARG arg6,
	      ARG arg7,
	      ARG arg8,
	      ARG arg9)
{
  static char digits[] = "0123456789";

  TCB_ID tcbId;
  char *pTaskMem;
  char *pStackBase;
  char newName[20];
  int value, nBytes, nPreBytes;
  char *pBufStart, *pBufEnd;
  char ch;

  /* Check if task lib is installed */
  if (taskLibInstalled == FALSE)
  {
    errnoSet (S_taskLib_NOT_INSTALLED);
    return ((int) NULL);
  }

  /* If NULL name create default name for task */
  if (name == NULL) {

    strcpy(newName, namelessPrefix);
    nBytes = strlen(newName);
    nPreBytes = nBytes;
    value = ++namelessCount;

    /* Do while value is non-zero */
    do {

      newName[nBytes++] = digits[value % 10];
      value /= 10;

    } while(value != 0);

    /* Calculate start/end positions in name */
    pBufStart = newName + nPreBytes;
    pBufEnd = newName + nBytes - 1;

    /* While startbuffer lt. end buffer */
    while (pBufStart < pBufEnd) {

      ch = *pBufStart;
      *pBufStart = *pBufEnd;
      *pBufEnd = ch;
      pBufStart++;
      pBufEnd--;

    } /* End while startbuffer lt. end buffer */

    /* Terminate string */
    newName[nBytes] = EOS;

    /* Use this a the name for the task */
    name = newName;

  } /* End if null name create default name for task */

  /* Round up stack size */
  stackSize = STACK_ROUND_UP(stackSize);

  /* Allocate new TCB plus stack */
  pTaskMem = objAllocPad(taskClassId,
		  	 (unsigned) stackSize,
			 (void **) NULL);
  if (pTaskMem == NULL) {

    /* errno set by objAllocPad() */
    return ((int) NULL);
  }

  /* Setup stack vars */

#if (_STACK_DIR == _STACK_GROWS_DOWN)

  pStackBase = pTaskMem + stackSize;
  tcbId = (TCB_ID) pStackBase;

#else /* _STACK_GROWS_UP */

  tcbId = (TCB_ID) (pTaskMem + TASK_EXTRA_BYTES);
  pStackBase = STACK_ROUND_UP(pTaskMem + TASK_EXTRA_BYTES + sizeof(TCB));

#endif /* _STACK_DIR */

  /* Initialize task */
  if (taskInit(tcbId, name, priority, options, pStackBase, stackSize, func,
	       arg0, arg1, arg2, arg3, arg4, arg5,
	       arg6, arg7, arg8, arg9) != OK)
  {

    /* errno set by taskInit() */
    objFree(taskClassId, tcbId);
    return ((int) NULL);
  }

  return ((int) tcbId);
}

/*******************************************************************************
* taskInit - Initialize task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskInit(TCB_ID tcbId,
                const char *name,
                unsigned priority,
		int options,
  		char *pStackBase,
                unsigned stackSize,
                FUNCPTR func,
		ARG arg0,
		ARG arg1,
		ARG arg2,
		ARG arg3,
		ARG arg4,
		ARG arg5,
		ARG arg6,
		ARG arg7,
		ARG arg8,
		ARG arg9)
{
  static unsigned new_id;
  int i, len;
  char *taskName;
  ARG args[MAX_TASK_ARGS];

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return ERROR;
  }

  /* Check if task lib is installed */
  if (!taskLibInstalled)
  {
    errnoSet (S_taskLib_NOT_INSTALLED);
    return ERROR;
  }

  /* Copy args to array */
  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;
  args[5] = arg5;
  args[6] = arg6;
  args[7] = arg7;
  args[8] = arg8;
  args[9] = arg9;

  /* Set unique id */
  tcbId->id = new_id++;

  /* Set initial status as suspended */
  tcbId->status = TASK_SUSPEND;
  tcbId->lockCount = 0;
  tcbId->priority = priority;
  tcbId->options = options;

  /* Time slice counter */
  tcbId->timeSlice = 0;

  /* Pending queue, used by semaphores */
  tcbId->pPendQ = NULL;

  tcbId->objUnpendHandler = NULL;
  tcbId->pObj             = NULL;
  tcbId->objInfo          = 0;

  /* Initialize safety */
  tcbId->safeCount = 0;
  qInit(&tcbId->safetyQ, qPrioClassId,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0,
	(ARG)0);

  /* Task entry point */
  tcbId->entry = func;

  /* Setup error status */
  tcbId->errorStatus = OK;
  tcbId->exitCode = 0;

  /* Zero task extensions */
  tcbId->pTaskVar = NULL;

  /* Zero signals and exception info */
  tcbId->pSignalInfo = NULL;

  /* Exception info */
  tcbId->pExcRegSet = NULL;
  tcbId->excInfo.valid = 0;

  /* Setup stack */
  tcbId->pStackBase = pStackBase;
  tcbId->pStackLimit = tcbId->pStackBase + stackSize * _STACK_DIR;
  tcbId->pStackEnd = tcbId->pStackLimit;

  if ( !(options & TASK_OPTIONS_NO_STACK_FILL) )

#if (_STACK_DIR == _STACK_GROWS_DOWN)

    memset(tcbId->pStackLimit, 0xee, stackSize);

#else /* _STACK_GROWS_UP */

    memset(tcbId->stackBase, 0xee, stackSize);

#endif /* _STACK_DIR */

  /* Clear events structure */
  memset((char *) &(tcbId->events), 0, sizeof(EVENTS));

  /* Initialize standard file desriptors */
  for (i = 0; i < 3; i++) {

    tcbId->taskStd[i] = i;
    tcbId->taskStdFp[i] = NULL;

  }

  /* Initialize architecutre depedent stuff */
  taskRegsInit(tcbId, pStackBase);

  /* Push args on task stack */
  taskArgSet(tcbId, pStackBase, args);

  /* Object core */
  objCoreInit(&tcbId->objCore, taskClassId);

  /* Copy name if not unnamed */
  if (name != NULL) {
    len = strlen(name) + 1;
    taskName = (char *) taskStackAllot((int) tcbId, len);
    if (taskName != NULL)
      strcpy(taskName, name);
    tcbId->name = taskName;
  }
  else
    tcbId->name = NULL;

  /* Run create hooks */
  for (i = 0; i < MAX_TASK_CREATE_HOOKS; i++)
    if (taskCreateHooks[i] != NULL)
      (*taskCreateHooks[i])(tcbId);

  /* Start task */
  rtosSpawn(tcbId);

  return OK;
}

/*******************************************************************************
* taskDelete - Delete task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskDelete(int taskId)
{
  return (taskDestroy(taskId, TRUE, WAIT_FOREVER, FALSE));
}

/*******************************************************************************
* taskDeleteForce - Remove task forcevly
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskDeleteForce(int taskId)
{
  return (taskDestroy(taskId, TRUE, WAIT_FOREVER, TRUE));
}

/*******************************************************************************
* taskTerminate - Terminate task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskTerminate(int taskId)
{
  return (taskDestroy(taskId, FALSE, WAIT_FOREVER, FALSE));
}

/*******************************************************************************
* taskDestroy - Kill task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskDestroy(int taskId,
		   BOOL freeStack,
		   unsigned timeout,
		   BOOL forceDestroy)
{
  STATUS status;
  int i, level;
  TCB_ID tcbId;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return ERROR;
  }

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* If task self destruct and excption lib installed */
  if ( (tcbId == taskIdCurrent) && (_func_excJobAdd != NULL) ) {

    /* Wait for safe to destroy */
    while (tcbId->safeCount > 0)
      taskUnsafe();

    /* Kill it */
    ( *_func_excJobAdd) (taskDestroy, tcbId, freeStack, WAIT_NONE, FALSE);

    /* Block here and suspend */
    while(1)
      taskSuspend(0);

  } /* End if task self destruct and exception lib installed */

taskDestroyLoop:

  /* Lock interrupts */
  INT_LOCK(level);

  /* Check id */
  if (TASK_ID_VERIFY(tcbId) != OK)
  {
    /* errno set by taskIdVerify() */

    /* Unlock interrupts */
    INT_UNLOCK(level);

    return ERROR;
  }

  /* Mask all signals */
  if (tcbId->pSignalInfo != NULL)
    tcbId->pSignalInfo->sigt_blocked = 0xffffffff;

  /* Block here for safe and running locked tasks */
  while ( (tcbId->safeCount > 0) ||
	  ( (tcbId->status == TASK_READY) && (tcbId->lockCount > 0) )
        )
  {
    /* Enter kernel mode */
    kernelState = TRUE;

    /* Unlock interrupts */
    INT_UNLOCK(level);

    /* Check if force deletion, or suicide */
    if (forceDestroy || (tcbId == taskIdCurrent))
    {

      /* Remove protections */
      tcbId->safeCount = 0;
      tcbId->lockCount = 0;

      /* Check if flush of safety queue is needed */
      if (Q_FIRST(&tcbId->safetyQ) != NULL)
        rtosPendQFlush(&tcbId->safetyQ);

      /* Exit trough kernel */
      kernExit();
    }
    else
    {
      /* Not forced deletion or suicide */

      /* Put task on safe queue */
      if (rtosPendQPut(&tcbId->safetyQ, timeout) != OK)
      {
        /* Exit trough kernel */
        kernExit();

        errnoSet (S_taskLib_INVALID_TIMEOUT);

        return ERROR;
      }

      /* Exit trough kernel */
      status = kernExit();

      /* Check for restart */
      if (status == SIG_RESTART)
      {
        /* If singnal recalc function exists */
        if (_func_sigTimeoutRecalc != NULL) {

          timeout = ( *_func_sigTimeoutRecalc) (timeout);
          goto taskDestroyLoop;

        } /* End if signal recalc function exists */
      }

      /* Check if unsuccessful */
      if (status == ERROR)
      {
          /* timer should have set errno to S_objLib_TIMEOUT */
          return ERROR;
      }

    } /* End else forced or suicide */

    /* Lock interrupts */
    INT_LOCK(level);

    /* Now verify class id again */
    if (TASK_ID_VERIFY(tcbId) != OK)
    {
      /* errno set by taskIdVerify() */

      /* Unlock interrupts */
      INT_UNLOCK(level);

      return ERROR;
    }

  } /* End while blocked by safety */

  /* Now only one cadidate is selected for deletion */

  /* Make myself safe */
  taskSafe();

  /* Protet deletion cadidate */
  tcbId->safeCount++;

   /* Check if not suicide */
   if (tcbId != taskIdCurrent)
   {
     /* Enter kernel mode */
     kernelState = TRUE;

     /* Unlock interrupts */
     INT_UNLOCK(level);

     /* Suspend victim */
     rtosSuspend(tcbId);

     /* Exit trough kernel */
     kernExit();
  }
  else
  {
     /* Unlock interrupts */
     INT_UNLOCK(level);
  }

  /* Run deletion hooks */
  for (i = 0; i < MAX_TASK_DELETE_HOOKS; i++)
    if (taskDeleteHooks[i] != NULL)
      (*taskDeleteHooks[i])(tcbId);

  /* Lock task */
  taskLock();

  /* If dealloc and options dealloc stack */
  if ( freeStack && (tcbId->options & TASK_OPTIONS_DEALLOC_STACK) ) {

#if (_STACK_DIR == _STACK_GROWS_DOWN)

    objFree(taskClassId, tcbId->pStackEnd);

#else /* _STACK_GROWS_UP */

    objFree(taskClassId, tbcId - TASK_EXTRA_BYTES);

#endif /* _STACK_DIR */

  }

  /* Lock interrupts */
  INT_LOCK(level);

  /* Invalidate id */
  objCoreTerminate(&tcbId->objCore);

  /* Enter kernel mode */
  kernelState = TRUE;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  /* Delete task */
  status = rtosDelete(tcbId);

  /* Check if safe quque needs to be flushed */
  if (Q_FIRST(&tcbId->safetyQ) != NULL)
    rtosPendQFlush(&tcbId->safetyQ);

  /* Exit trough kernel */
  kernExit();

  /* Unprotect */
  taskUnlock();
  taskUnsafe();

  return OK;
}

/*******************************************************************************
* taskActivate - Activate task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskActivate(int taskId)
{
  return(taskResume(taskId));
}

/*******************************************************************************
* taskSuspend - Suspend a task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSuspend(int taskId)
{
  TCB_ID tcbId;

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* TASK_ID_VERIFY() already done by taskTcb() */

  /* Check if in kernel mode */
  if (kernelState == TRUE)
  {
    /* Add to kernel queue */
    kernQAdd1 ((FUNCPTR) rtosSuspend, (ARG) tcbId);

    return OK;
  }

  /* Enter kernel mode */
  kernelState = TRUE;

  /* Suspend task */
  rtosSuspend(tcbId);

  /* Exit trough kernel */
  kernExit();

  return OK;
}

/*******************************************************************************
* taskResume - Resume task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskResume(int taskId)
{
  TCB_ID tcbId;

  /* If try to resume current task */
  if (taskId == 0)
    return OK;

  tcbId = (TCB_ID) taskId;

  /* Verify that it is actually a task */
  if (TASK_ID_VERIFY(tcbId) != OK)
  {
    /* errno set by TASK_ID_VERIFY() */
    return ERROR;
  }

  /* Put on queue if in kernel mode */
  if (kernelState == TRUE)
  {
    /* Put on kernel queue */
    kernQAdd1((FUNCPTR) rtosResume, (ARG) tcbId);

    return OK;
  }

  /* Enter kernel mode */
  kernelState = TRUE;

  /* Resume task */
  rtosResume(tcbId);

  /* Exit kernel mode */
  kernExit();

  return OK;
}

/*******************************************************************************
* taskDelay - Put a task to sleep
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskDelay(unsigned timeout)
{
  STATUS status;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return ERROR;
  }

  /* Enter kernel mode */
  kernelState = TRUE;

  /* If no wait, then just reinsert it */
  if (timeout == WAIT_NONE)
  {
    Q_REMOVE(&kernReadyQ, taskIdCurrent);
    Q_PUT(&kernReadyQ, taskIdCurrent, taskIdCurrent->priority);
  }
  /* Put to sleep */
  else
  {
    rtosDelay(timeout);
  }

  /* Exit trough kernel, and check for error */
  if ( (status = kernExit()) == SIG_RESTART)
  {
    status = ERROR;
  }

  return status;
}

/*******************************************************************************
* taskUndelay - Wake up a sleeping task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskUndelay(int taskId)
{
  TCB_ID tcbId;

  tcbId = (TCB_ID) taskId;

  /* Verify that it is actually a task */
  if (TASK_ID_VERIFY(tcbId) != OK)
  {
    /* errno set by TASK_ID_VERIFY() */
    return ERROR;
  }

  /* Put on queue if in kernel mode */
  if (kernelState == TRUE)
  {
    /* Put on kernel queue */
    kernQAdd1((FUNCPTR) rtosUndelay, (ARG) tcbId);

    return OK;
  }

  /* Enter kernel mode */
  kernelState = TRUE;

  /* Resume task */
  rtosUndelay(tcbId);

  /* Exit kernel mode */
  kernExit();

  return OK;
}

/*******************************************************************************
* taskPrioritySet - Change task priority
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskPrioritySet(int taskId, unsigned priority)
{
  TCB_ID tcbId;

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* TASK_ID_VERIFY() already done by taskTcb() */

  /* Check if in kernel mode */
  if (kernelState == TRUE)
  {
    /* Add work to kernel */
    kernQAdd2 ((FUNCPTR) rtosPrioritySet, (ARG) tcbId, (ARG) priority);

    return OK;
  }

  /* Enter kernel mode */
  kernelState = TRUE;

  /* Set priority */
  rtosPrioritySet(tcbId, priority);

  /* Exit trough kernel */
  kernExit();

  return OK;
}

/*******************************************************************************
* taskPriorityGet - Get task priority
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskPriorityGet(int taskId, unsigned *priority)
{
  TCB_ID tcbId;

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* TASK_ID_VERIFY() already done by taskTcb() */

  /* Store priority */
  *priority = tcbId->priority;

  return OK;
}

/*******************************************************************************
* taskRestart - Restart a task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskRestart(int taskId)
{
  TCB_ID tcbId;
  char *name, *rename;
  int len;
  unsigned priority;
  int options;
  char *pStackBase;
  unsigned stackSize;
  FUNCPTR entry;
  ARG args[MAX_TASK_ARGS];
  STATUS status;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return ERROR;
  }

  /* If self restart */
  if ( (taskId == 0) || (taskId == (int) taskIdCurrent) )
  {

    /* Task must be unsafe */
    while (taskIdCurrent->safeCount > 0)
      taskSafe();

    /* Spawn a task that will restart this task */
    taskSpawn(restartTaskName, restartTaskPriority, restartTaskOptions,
	      restartTaskStackSize, taskRestart, (ARG) taskIdCurrent,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0,
	      (ARG) 0);

    /* Wait for restart */
    while (1)
      taskSuspend(0);

  } /* End if self restart */

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* TASK_ID_VERIFY() already done by taskTcb() */

  /* Copy task data */
  priority = tcbId->priority;
  options = tcbId->options;
  entry = tcbId->entry;
  pStackBase = tcbId->pStackBase;
  stackSize = (tcbId->pStackEnd - tcbId->pStackBase) * _STACK_DIR;
  taskArgGet(tcbId, pStackBase, args);

  /* Copy name if needed */
  name = tcbId->name;
  rename = NULL;
  if (name != NULL) {
    len = strlen(name) + 1;
    rename = malloc(len);
    if (rename != NULL)
      strcpy(rename, name);
    name = rename;
  }

  /* Prevent deletion */
  taskSafe();

  if (taskTerminate((int) tcbId) != OK)
  {
    taskUnsafe();
    /* errno set by taskTerminate() */
    return ERROR;
  }

  /* Initialize task with same data */
  status = taskInit(tcbId, name, priority, options, pStackBase,
		    stackSize, entry, args[0], args[1], args[2],
		    args[3], args[4], args[5], args[6], args[7],
		    args[8], args[9]);
  if (status != OK)
  {
    /* errno set by taskInit() */
    return ERROR;
  }

  /* And start it */
  status = taskActivate((int) tcbId);
  if (status != OK)
  {
    /* errno set by taskActivate() */
    return ERROR;
  }

  /* Make me mortal */
  taskUnsafe();

  /* Free rename buffer if needed */
  if (rename != NULL)
    free(rename);

  return OK;
}

/*******************************************************************************
* taskExit - Exit from task
*
* RETURNS: N/A
*******************************************************************************/

void taskExit(int code)
{
  /* Store return code */
  taskIdCurrent->exitCode = code;

  /* Lock task */
  taskLock();

  /* Destroy task */
  taskDestroy(NULL, TRUE, WAIT_FOREVER, FALSE);

  /* Unlock task */
  taskUnlock();
}

/*******************************************************************************
* exit - Exit from task
*
* RETURNS: N/A
*******************************************************************************/

void exit(int code)
{
  taskExit(code);
}

/*******************************************************************************
* taskLock - Prevent task from beeing switched out
*
* RETURNS: N/A
*******************************************************************************/

STATUS taskLock(void)
{
  taskIdCurrent->lockCount++;

  return OK;
}

/*******************************************************************************
* taskUnlock - Allow task to be switched out
*
* RETURNS: N/A
*******************************************************************************/

STATUS taskUnlock(void)
{
  /* Check if state is chaged */
  if ( (taskIdCurrent->lockCount > 0) && (--taskIdCurrent->lockCount == 0) )
  {
    /* Enter kernel mode */
    kernelState = TRUE;

    if (Q_FIRST(&taskIdCurrent->safetyQ) != NULL)
      rtosPendQFlush(&taskIdCurrent->safetyQ);

    /* Exit trough kernel */
    kernExit();
  }
  return OK;
}

/*******************************************************************************
* taskSafe - Make safe from deletion
*
* RETURNS: Ok or ERROR
*******************************************************************************/

STATUS taskSafe(void)
{
  taskIdCurrent->safeCount++;

  return OK;
}

/*******************************************************************************
* taskUnsafe - Make unsafe from deletion
*
* RETURNS: Ok or ERROR
*******************************************************************************/

STATUS taskUnsafe(void)
{
  /* Check if state is chaged */
  if ( (taskIdCurrent->safeCount > 0) && (--taskIdCurrent->safeCount == 0) )
  {
    /* Enter kernel mode */
    kernelState = TRUE;

    if (Q_FIRST(&taskIdCurrent->safetyQ) != NULL)
      rtosPendQFlush(&taskIdCurrent->safetyQ);

    /* Exit trough kernel */
    kernExit();
  }

  return OK;
}

/*******************************************************************************
* taskIdSelf - Get my own TCB
*
* RETURNS: Integer taskId
*******************************************************************************/

int taskIdSelf(void)
{
  return((int) taskIdCurrent);
}

/*******************************************************************************
* taskTcb - Get TCB
*
* RETURNS: TCB_ID to task, or to runnig task if 0 as arg
*******************************************************************************/

TCB_ID taskTcb(int taskId)
{
  TCB_ID tcbId;

  /* If 0, return current task id */
  if (taskId == 0) return(taskIdCurrent);

  /* Convert to pointer to TCB struct */
  tcbId = (TCB_ID) taskId;

  /* Verify that it is actually a task */
  if (TASK_ID_VERIFY(tcbId) != OK)
  {
    /* errno set by TASK_ID_VERIFY() */
    return NULL;
  }

  /* Return pointer to TCB struct */
  return tcbId;
}

/*******************************************************************************
* taskStackAllot - Allot memory from callers stack
*
* RETURNS: Pointer to memory, or NULL
*******************************************************************************/

void *taskStackAllot(int taskId, unsigned size)
{
  TCB_ID tcbId;
  char *pStackPrev;

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return NULL;

  /* TASK_ID_VERIFY() already done by taskTcb() */

  /* Round up */
  size = STACK_ROUND_UP(size);

  /* Check if size is ok */
  if (size > (tcbId->pStackLimit - tcbId->pStackBase) * _STACK_DIR)
  {
    errnoSet (S_taskLib_STACK_OVERFLOW);
    return NULL;
  }

  /* Store old stack end */
  pStackPrev = tcbId->pStackLimit;

#if (_STACK_DIR == _STACK_GROWS_DOWN)

  tcbId->pStackLimit += size;
  return( (void *) pStackPrev);

#else /* _STACK_GROWS_UP */

  tcbId->pStackLimit -= size;
  return( (void *) tcbId->pStackLimit);

#endif /* _STACK_DIR */
}

/*******************************************************************************
* taskIdVerify - Verify that this is actually a task
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskIdVerify(int taskId)
{
  TCB_ID tcbId;

  /* Get task context */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* TASK_ID_VERIFY() already done by taskTcb() */

  return OK;
}

/*******************************************************************************
* taskIdle - An idle task
*
* RETURNS: return value
*******************************************************************************/

int taskIdle(void)
{
  for (;;);

  return 0;
}

