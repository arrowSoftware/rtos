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

/* taskHookLib.c - Hooks called when kernel switch tasks */

#include <stdlib.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>

/* GLOBALS */
FUNCPTR taskCreateHooks[MAX_TASK_CREATE_HOOKS + 1];
FUNCPTR taskSwitchHooks[MAX_TASK_SWITCH_HOOKS + 1];
FUNCPTR taskDeleteHooks[MAX_TASK_DELETE_HOOKS + 1];
FUNCPTR taskSwapHooks[MAX_TASK_SWAP_HOOKS + 1];
int taskSwapReference[MAX_TASK_SWAP_HOOKS + 1];

/* LOCALS */
LOCAL BOOL taskHookLibInstalled = FALSE;
LOCAL STATUS taskHookAdd(FUNCPTR hook, FUNCPTR table[], int max);
LOCAL STATUS taskHookDelete(FUNCPTR hook, FUNCPTR table[], int max);
LOCAL STATUS taskSwapMaskSet(int taskId,
			     int index,
			     BOOL swapIn,
			     BOOL swapOut);
LOCAL STATUS taskSwapMaskClear(int taskId,
			       int index,
			       BOOL swapIn,
			       BOOL swapOut);

/*******************************************************************************
* taskHookLibInit - Initialize task switch hook library
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskHookLibInit(void)
{
  int i;

  /* Null all entries */
  for (i = 0; i < MAX_TASK_CREATE_HOOKS; i++)
    taskCreateHooks[i] = NULL;

  for (i = 0; i < MAX_TASK_SWITCH_HOOKS; i++)
    taskSwitchHooks[i] = NULL;

  for (i = 0; i < MAX_TASK_DELETE_HOOKS; i++)
    taskDeleteHooks[i] = NULL;

  for (i = 0; i < MAX_TASK_SWAP_HOOKS; i++)
  {
    taskSwapHooks[i] = NULL;
    taskSwapReference[i] = 0;
  }

  /* Mark as installed */
  taskHookLibInstalled = TRUE;

  return(OK);
}

/*******************************************************************************
* taskCreateHookAdd - Add a task create hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskCreateHookAdd(FUNCPTR hook)
{
  /* Call general function */
  return (taskHookAdd(hook, taskCreateHooks, MAX_TASK_CREATE_HOOKS));
}

/*******************************************************************************
* taskCreateHookDelete - Delete a task create hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskCreateHookDelete(FUNCPTR hook)
{
  /* Call general function */
  return (taskHookDelete(hook, taskCreateHooks, MAX_TASK_CREATE_HOOKS));
}

/*******************************************************************************
* taskSwitchHookAdd - Add a task switch hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSwitchHookAdd(FUNCPTR hook)
{
  /* Call general function */
  return (taskHookAdd(hook, taskSwitchHooks, MAX_TASK_SWITCH_HOOKS));
}

/*******************************************************************************
* taskSwitchHookDelete - Delete a task switch hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSwitchHookDelete(FUNCPTR hook)
{
  /* Call general function */
  return (taskHookDelete(hook, taskSwitchHooks, MAX_TASK_SWITCH_HOOKS));
}

/*******************************************************************************
* taskDeleteHookAdd - Add a task delete hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskDeleteHookAdd(FUNCPTR hook)
{
  /* Call general function */
  return (taskHookAdd(hook, taskDeleteHooks, MAX_TASK_DELETE_HOOKS));
}

/*******************************************************************************
* taskDeleteHookDelete - Delete a task switch delete hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskDeleteHookDelete(FUNCPTR hook)
{
  /* Call general function */
  return (taskHookDelete(hook, taskDeleteHooks, MAX_TASK_DELETE_HOOKS));
}

/*******************************************************************************
* taskHookAdd - Add a task hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS taskHookAdd(FUNCPTR hook, FUNCPTR table[], int max)
{
  int i;

  /* Lock task */
  taskLock();

  /* Find next free slot */
  for (i = 0; i < max; i++)
  {
    if (table[i] == NULL)
    {

      /* Insert info fist free spot */
      table[i] = hook;

      /* Exit */
      taskUnlock();
      return(OK);
    }
  }

  /* No slots left */
  taskUnlock();

  errnoSet(S_taskLib_TASK_HOOK_TABLE_FULL);
  return(ERROR);
}

/*******************************************************************************
* taskHookDelete - Delete a task hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS taskHookDelete(FUNCPTR hook, FUNCPTR table[], int max)
{
  int i;

  /* Lock task */
  taskLock();

  /* Find entry */
  for (i = 0; i < max; i++)
  {
    if (table[i] == hook)
    {

      /* Move up all other hooks */
      do {
	table[i] = table[i + 1];
      } while (table[i] != NULL);

      /* Exit */
      taskUnlock();
      return(OK);
    }
  }

  /* Slot not found */
  taskUnlock();

  errnoSet(S_taskLib_TASK_HOOK_NOT_FOUND);
  return(ERROR);
}

/*******************************************************************************
* taskSwapHookAdd - Add a task swap hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSwapHookAdd(FUNCPTR hook)
{
  int i;

  /* Lock task */
  taskLock();

  /* Find next free slot */
  for (i = 0; i < MAX_TASK_SWAP_HOOKS; i++)
  {
    if (taskSwapHooks[i] == NULL)
    {

      /* Insert info fist free spot */
      taskSwapHooks[i] = hook;
      taskSwapReference[i] = 0;

        /* Exit */
        taskUnlock();
	return(OK);
      }
  }
  /* Slot not found */
  taskUnlock();

  errnoSet(S_taskLib_TASK_HOOK_TABLE_FULL);
  return(ERROR);
}

/*******************************************************************************
* taskSwapHookAttach - Attach a task to swap hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSwapHookAttach(FUNCPTR hook,
			  int taskId,
			  BOOL swapIn,
			  BOOL swapOut)
{
  int i;

  /* Lock task */
  taskLock();

  /* Find hook */
  for (i = 0; i < MAX_TASK_SWAP_HOOKS; i++)
  {

    if (taskSwapHooks[i] == hook)
    {

      if ( (taskSwapMaskSet(taskId, i, swapIn, swapOut) != OK) )
      {
	/* Return error */
	taskUnlock();
	return(ERROR);
      }
      else
      {

        /* Set swap in/out */
        taskSwapReference[i] += (swapIn) ? 1 : 0;
        taskSwapReference[i] += (swapOut) ? 1 : 0;

	taskUnlock();
	return(OK);
      }

    }

  }

  /* Not found */
  taskUnlock();

  errnoSet(S_taskLib_TASK_HOOK_NOT_FOUND);
  return(ERROR);
}

/*******************************************************************************
* taskSwapHookDetach - Detach a task to swap hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSwapHookDetach(FUNCPTR hook,
			  int taskId,
			  BOOL swapIn,
			  BOOL swapOut)
{
  int i;

  /* Lock task */
  taskLock();

  /* Find hook */
  for (i = 0; i < MAX_TASK_SWAP_HOOKS; i++)
  {

    if (taskSwapHooks[i] == hook)
    {

      if ( (taskSwapMaskClear(taskId, i, swapIn, swapOut) != OK) )
      {
	/* Return error */
	taskUnlock();
	return(ERROR);
      }
      else
      {

	/* Reset in/out */
        taskSwapReference[i] -= (swapIn) ? 1 : 0;
        taskSwapReference[i] -= (swapOut) ? 1 : 0;

	/* Return error */
	taskUnlock();
	return(OK);
      }

    }

  }

  /* Not found */
  taskUnlock();

  errnoSet(S_taskLib_TASK_HOOK_NOT_FOUND);
  return(ERROR);
}

/*******************************************************************************
* taskSwapHookDelete - Delete a task swap hook
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS taskSwapHookDelete(FUNCPTR hook)
{
  int i;

  /* Lock task */
  taskLock();

  /* Find entry */
  for (i = 0; i < MAX_TASK_SWAP_HOOKS; i++)
  {
    if (taskSwapHooks[i] == hook)
    {

      /* Check if task are still onnected to this hook */
      if (taskSwapReference[i] != 0)
      {
	/* Unable to remove */
	errnoSet(S_taskLib_TASK_SWAP_HOOK_REFERENCED);
	taskUnlock();
	return(ERROR);
      }
      else
      {

        /* Remove entry */
        taskSwapHooks[i] = NULL;
	taskUnlock();
	return(OK);
      }
    }
  }

  /* Slot not found */
  taskUnlock();

  errnoSet(S_taskLib_TASK_HOOK_NOT_FOUND);
  return(ERROR);
}

/*******************************************************************************
* taskSwapMaskSet - Set swap mask for a task
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS taskSwapMaskSet(int taskId,
			     int index,
			     BOOL swapIn,
			     BOOL swapOut)
{
  TCB_ID pTcb;
  u_int16_t indexBit;

  /* Get TCB */
  pTcb = taskTcb(taskId);

  /* Check if valid task */
  if (pTcb == NULL)
    return(NULL);

  /* Calculate index bit */
  indexBit = (1 << (15 - index));

  /* Check if valid  */
  if ( ((swapIn) && (pTcb->swapInMask & indexBit)) ||
       ((swapOut) && (pTcb->swapOutMask & indexBit)) )
  {
    errnoSet(S_taskLib_TASK_SWAP_HOOK_SET);
    return(ERROR);
  }

  /* Set bits */
  if (swapIn) pTcb->swapInMask |= indexBit;
  if (swapOut) pTcb->swapOutMask |= indexBit;

  return(OK);
}

/*******************************************************************************
* taskSwapMaskClear - Clear swap mask for a task
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS taskSwapMaskClear(int taskId,
			       int index,
			       BOOL swapIn,
			       BOOL swapOut)
{
  TCB_ID pTcb;
  u_int16_t indexBit;

  /* Get TCB */
  pTcb = taskTcb(taskId);

  /* Check if valid task */
  if (pTcb == NULL)
    return(NULL);

  /* Calculate index bit */
  indexBit = (1 << (15 - index));

  /* Check if valid  */
  if ( ((swapIn) && (pTcb->swapInMask & indexBit)) ||
       ((swapOut) && (pTcb->swapOutMask & indexBit)) )
  {
    errnoSet(S_taskLib_TASK_SWAP_HOOK_CLEAR);
    return(ERROR);
  }

  /* Set bits */
  if (swapIn) pTcb->swapInMask &= ~indexBit;
  if (swapOut) pTcb->swapOutMask &= ~indexBit;

  return(OK);
}

