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

/* kernLib.c - Kernel */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/qLib.h>
#include <util/qPrioLib.h>
#include <util/qPriBmpLib.h>
#include <util/qFifoLib.h>
#include <rtos/taskLib.h>
#include <rtos/kernLib.h>
#include <rtos/kernQLib.h>
#include <rtos/rtosLib.h>
#include <rtos/memPartLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/kernTickLib.h>
#include <arch/taskArchLib.h>

/* Macros */
#define TCB_SIZE		((unsigned)STACK_ROUND_UP(sizeof(TCB)))
#define MEM_BLOCK_HEADER_SIZE	((unsigned)STACK_ROUND_UP(sizeof(BLOCK_HEADER)))
#define MEM_FREE_BLOCK_SIZE	((unsigned)STACK_ROUND_UP(sizeof(FREE_BLOCK)))
#define MEM_BASE_BLOCK_SIZE	(MEM_BLOCK_HEADER_SIZE+MEM_FREE_BLOCK_SIZE)
#define MEM_END_BLOCK_SIZE	(MEM_BLOCK_HEADER_SIZE)
#define MEM_TOTAL_BLOCK_SIZE	((2*MEM_BLOCK_HEADER_SIZE)+MEM_FREE_BLOCK_SIZE)


/* Globals */
char kernVersion[] = "rtos pre-alpha";
BOOL kernInitialized = FALSE;
BOOL kernelState = FALSE;
BOOL kernelIsIdle = FALSE;
BOOL kernRoundRobin = FALSE;
unsigned kernRoundRobinTimeSlice = 0;
TCB_ID taskIdCurrent = NULL;
Q_HEAD kernActiveQ = {NULL, 0, 0 ,NULL};
Q_HEAD kernTickQ;
Q_HEAD kernReadyQ;

char* pKernExcStkBase;
char* pKernExcStkEnd;
unsigned kernExcStkCnt = 0;

char *pRootMemStart;
unsigned rootMemNBytes;
int rootTaskId;

/* Imports */
IMPORT void kernTaskLoadContext(void);

/*******************************************************************************
 * kernelInit - Initialize kernel
 * 
 * RETURNS: N/A
 ******************************************************************************/

void kernelInit(FUNCPTR rootFunc,
		unsigned rootMemSize,
		char *pMemPoolStart,
		char *pMemPoolEnd,
		unsigned intStackSize,
		int lockOutLevel)
{
  union {
    char align[8];
    TCB initTcb;
  } tcbAligned;

  TCB_ID tcbId;
  unsigned rootStkSize, memPoolSize;
  char *pRootStkBase;
  int level;

  /* Align input parameters */
  pMemPoolStart = (char *) STACK_ROUND_UP(pMemPoolStart);
  pMemPoolEnd = (char *) STACK_ROUND_UP(pMemPoolEnd);
  intStackSize = STACK_ROUND_UP(intStackSize);

  /* Setup global root task memory byte count */
  rootMemNBytes = STACK_ROUND_UP(rootMemSize);

  /* Setup interrupt lock level */
  intLockLevelSet(lockOutLevel);

  /* Initialize kernel state variables */
  kernelState = FALSE;
  kernRoundRobin = FALSE;
  kernRoundRobinTimeSlice = 0;

#if (_STACK_DIR == _STACK_GROWS_DOWN)

  /* Setup interrupt stack at bottom of memory pool */
  pKernExcStkBase = pMemPoolStart + intStackSize;
  pKernExcStkEnd = pMemPoolStart;

  /* Fill stack with 0xee */
  memset(pKernExcStkEnd, 0xee, (int) intStackSize);

  /* Memory will start after interrupt stack */
  pMemPoolStart = pKernExcStkBase;

#else /* _STACK_DIR == _STACK_GROWS_UP */

  /* Setup interrupt stack at bottom of memory pool */
  pKernExcStkBase = pMemPoolStart;
  pKernExcStkEnd = pMemPoolStart + intStackSize;

  /* Fill stack with 0xee */
  memset(pKernExcStkBase, 0xee, (int) intStackSize);

  /* Memory will start after interrupt stack */
  pMemPoolStart = pKernExcStkEnd;

#endif /* _STACK_DIR */

  /*
   * The root tasks stack and TCB will be located at the end of the memory pool.
   * Where the TCB will be just above the stack.
   * One memory block must be left alone at the end and at the beginning
   * of the memory pool.
   */
  rootStkSize = rootMemNBytes - TCB_SIZE - MEM_TOTAL_BLOCK_SIZE;
  pRootMemStart = pMemPoolEnd - rootMemNBytes;

#if (_STACK_DIR == _STACK_GROWS_DOWN)

  pRootStkBase = pRootMemStart + rootStkSize + MEM_BASE_BLOCK_SIZE;
  tcbId = (TCB_ID) pRootStkBase;

#else /* _STACK_DIR == _STACK_GROWS_UP */

  tcbId = (TCB_ID) (pRootMemStart + MEM_BASE_BLOCK_SIZE);
  pRootStkBase = pRootMemStart + TCB_SIZE + MEM_BASE_BLOCK_SIZE;

#endif /* _STACK_DIR */

  /*
   * Initialize the root task with taskIdCurrent as NULL.
   * Since the kernel ready queue will be empty no task
   * switch will occur.
   */

  taskIdCurrent = NULL;
  memset(&tcbAligned, 0, sizeof(TCB));

  /* New memory pool will lose the memory which is reserved for the root task */
  memPoolSize = (unsigned) ( (int) pRootMemStart - (int) pMemPoolStart );

  /* Initialize the root task */
  taskInit(tcbId,						/* TCB */
	   "tRootTask",						/* Name */
	   0,							/* Priority */
	   /* TASK_OPTIONS_DEALLOC_STACK | */
	   TASK_OPTIONS_UNBREAKABLE,				/* Options */
	   pRootStkBase,				 	/* Stk base */
	   (unsigned) rootStkSize,				/* Stk size */
	   (FUNCPTR) rootFunc,					/* Func */
	   (ARG) pMemPoolStart,					/* arg0 */
	   (ARG) memPoolSize,					/* arg1 */
	   (ARG) 0,
	   (ARG) 0,
	   (ARG) 0,
	   (ARG) 0,
	   (ARG) 0,
	   (ARG) 0,
	   (ARG) 0,
	   (ARG) 0);

  /* Store root tasks id */
  rootTaskId = (int) tcbId;

  /* Temporary storage for taskIdCurrent, later it points to next ready task */
  taskIdCurrent = &tcbAligned.initTcb;

  /* Mark kernel as initialized */
  kernInitialized = TRUE;

  /* Start the root task */
  taskActivate (rootTaskId);
}

/*******************************************************************************
 * kernelVersion - Return a string with kernel version
 * 
 * RETURNS: N/A
 ******************************************************************************/

char* kernelVersion(void)
{
  return kernVersion;
}

/******************************************************************************
* kernelTimeSlice - Enable/Disable round robin task scheduling
*
* RETURNS: N/A
******************************************************************************/

STATUS kernelTimeSlice(unsigned ticks)
{
  /* 0 turns round robin off */
  if (ticks == 0)
  {
    kernRoundRobin = FALSE;
  }
  else
  {
    kernRoundRobinTimeSlice = ticks;
    kernRoundRobin = TRUE;
  }

  return(OK);
}

