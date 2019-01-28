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

/* taskLib.h - Task scheduling library */

#ifndef _taskLib_h
#define _taskLib_h

#include <tools/moduleNumber.h>

#define S_taskLib_NOT_INSTALLED        		(M_taskLib | 0x0001)
#define S_taskLib_NULL_TASK_ID         		(M_taskLib | 0x0002)
#define S_taskLib_INVALID_TIMEOUT      		(M_taskLib | 0x0003)
#define S_taskLib_STACK_OVERFLOW       		(M_taskLib | 0x0004)
#define S_taskLib_TASK_HOOK_TABLE_FULL		(M_taskLib | 0x0005)
#define S_taskLib_TASK_HOOK_NOT_FOUND		(M_taskLib | 0x0006)
#define S_taskLib_TASK_SWAP_HOOK_REFERENCED	(M_taskLib | 0x0007)
#define S_taskLib_TASK_SWAP_HOOK_SET		(M_taskLib | 0x0008)
#define S_taskLib_TASK_SWAP_HOOK_CLEAR		(M_taskLib | 0x0009)
#define S_taskLib_TASK_VAR_NOT_FOUND		(M_taskLib | 0x000a)

#define TASK_READY      		0x00
#define TASK_SUSPEND    		0x01
#define TASK_PEND       		0x02
#define TASK_DELAY      		0x04
#define TASK_DEAD       		0x08

#define NUM_TASK_PRIORITIES		256
#define MAX_TASK_DELAY			0xffffffff
#define DEFAULT_STACK_SIZE		0x1000

#define MAX_TASK_ARGS			10

#define TASK_UNDELAYED			2
#define TASK_DELETED			3

#define TASK_OPTIONS_SUPERVISOR_MODE	0x0001
#define TASK_OPTIONS_UNBREAKABLE	0x0002
#define TASK_OPTIONS_DEALLOC_STACK	0x0004
#define TASK_OPTIONS_NO_STACK_FILL	0x0100
#define TASK_OPTIONS_PRIVATE_ENV	0x0200

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/types.h>
#include <arch/regs.h>
#include <arch/excArchLib.h>
#include <util/classLib.h>
#include <util/objLib.h>
#include <rtos/eventLib.h>
#include <rtos/semLib.h>
#include <rtos/taskVarLib.h>
#include <rtos/errnoLib.h>
#include <util/qLib.h>
#include <os/selectLib.h>
#include <signal.h>

/* Task control block */
typedef struct taskTCB {

  /* Queues for sheduler */
  Q_HEAD qNode;				/* 0x00 */
  Q_HEAD tickNode;			/* 0x10 */
  Q_HEAD activeNode;			/* 0x20 */

  /* Stack */
  char *pStackBase;			/* 0x30 */
  char *pStackLimit;			/* 0x34 */
  char *pStackEnd;			/* 0x38 */

  /* Entry point */
  FUNCPTR entry;			/* 0x3c */

  /* Errno */
  int errno;				/* 0x40 */

  /* Status */
  unsigned id;				/* 0x44 */
  unsigned status;			/* 0x48 */
  unsigned lockCount;			/* 0x4c */

  /* Kernel hooks */
  u_int16_t swapInMask;			/* 0x50 */
  u_int16_t swapOutMask;		/* 0x52 */

  /* Registers */
  REG_SET regs;				/* 0x54 */

  char *name;
  unsigned priority;
  int options;

  /* Timing */
  unsigned timeSlice;

  /* Used with semaphores */
  Q_HEAD *pPendQ;

  /* 
   * These next three fields are used to handle the case when the task
   * has timed out, or has been deleted, BEFORE it has obtained the
   * object on which it is pending.  Some objects require special code
   * for this (inversion safe mutex semaphores, read-write semaphores, ...).
   */

  void    (*objUnpendHandler) (void *, int);
  void *  pObj;     /* ptr to object (sem/msgQ/...) on which task pends */
  int     objInfo;  /* info to handler about what to do */

  /* Safety */
  unsigned safeCount;
  Q_HEAD safetyQ;

  /* Events */
  EVENTS events;

  /* Error codes */
  int errorStatus;
  int exitCode;

  /* Task extensions */
  struct taskVar *pTaskVar;

  /* Posix related */
  struct sigtcb *pSignalInfo;
  SEL_CONTEXT_ID selectContextId;

  /* Exceptions */
  REG_SET *pExcRegSet;
  EXC_INFO excInfo;

  /* I/O related */
  struct __sFILE *taskStdFp[3];
  int taskStd[3];

  /* Environment */
  char **ppEnviron;
  int envTableSize;
  int nEnvVarEntries;

  /* Class */
  OBJ_CORE objCore;

} __attribute__((packed)) TCB;


/* Macros */

/*******************************************************************************
* TASK_ID_VERIFY - Verify that this is a task object
*
* RETURNS: OK or ERROR
*******************************************************************************/

#define TASK_ID_VERIFY(tcbId)						       \
  ( (TCB_ID) (tcbId) == NULL )						       \
    ? (errno = S_taskLib_NULL_TASK_ID, ERROR)				       \
    : ( OBJ_VERIFY ((tcbId), taskClassId) )

typedef TCB *TCB_ID;
IMPORT CLASS_ID taskClassId;

extern STATUS taskLibInit(void);
extern int taskSpawn(const char *name,
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
		     ARG arg9);
extern int taskCreat(const char *name,
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
		     ARG arg9);
extern STATUS taskInit(TCB_ID tcbId,
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
		       ARG arg9);
extern STATUS taskDelete(int taskId);
extern STATUS taskDeleteForce(int taskId);
extern STATUS taskTerminate(int taskId);
extern STATUS taskDestroy(int taskId,
                   	  BOOL freeStack,
                   	  unsigned timeout,
  	             	  BOOL forceDestroy);
extern STATUS taskActivate(int taskId);
extern STATUS taskSuspend(int taskId);
extern STATUS taskResume(int taskId);
extern STATUS taskDelay(unsigned timeout);
extern STATUS taskUndelay(int taskId);
extern STATUS taskPrioritySet(int taskId, unsigned priority);
extern STATUS taskPriorityGet(int taskId, unsigned *priority);
extern STATUS taskRestart(int taskId);
extern void taskExit(int code);
extern STATUS taskLock(void);
extern STATUS taskUnlock(void);
extern STATUS taskSafe(void);
extern STATUS taskUnsafe(void);
extern int taskIdSelf(void);
extern TCB_ID taskTcb(int taskId);
extern void *taskStackAllot(int taskId, unsigned size);
extern STATUS taskIdVerify(int taskId);
extern int taskIdle(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _taskLib_h */

