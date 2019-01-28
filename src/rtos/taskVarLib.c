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

/* taskVarLib.c - Task variable library */

/* Includes */

#include <stdlib.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/errnoLib.h>
#include <rtos/taskVarLib.h>

/* Defines */

/* Imports */
IMPORT TCB_ID taskIdCurrent;

/* Locals */
LOCAL BOOL taskVarLibInstalled = FALSE;

LOCAL void taskVarSwitchHook(TCB_ID oldId, TCB_ID newId);
LOCAL void taskVarDeleteHook(TCB_ID tcbId);

/* Globals */

/* Functions */

/*******************************************************************************
 * taskVarLibInit - Initialize task variable library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskVarLibInit(void)
{
  /* Check if already installed */
  if (taskVarLibInstalled)
    return OK;

  /* Install */
  if ( taskSwitchHookAdd((FUNCPTR) taskVarSwitchHook) != OK )
    return ERROR;

  /* If fail */
  if ( taskDeleteHookAdd((FUNCPTR) taskVarDeleteHook) != OK ) {

    taskSwitchHookDelete((FUNCPTR) taskVarSwitchHook);
    return ERROR;

  } /* End if fail */

  /* Mark as installed */
  taskVarLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * taskVarAdd - Add a task variable
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskVarAdd(int taskId, int *pVar)
{
  TCB_ID tcbId;
  TASK_VAR *pNewVar;
  int level;

  /* Check library */
  if (!taskVarLibInstalled)
    return ERROR;

  /* Get task tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* Allocate variable storage */
  pNewVar = (TASK_VAR *) malloc( sizeof(TASK_VAR) );
  if (pNewVar == NULL)
    return ERROR;

  /* Initialize task variable struct */
  pNewVar->addr = pVar;
  pNewVar->value = *pVar;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Add task variable to list in tcb */
  pNewVar->next = tcbId->pTaskVar;
  tcbId->pTaskVar = pNewVar;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  return OK;
}

/*******************************************************************************
 * taskVarSet - Set value of a task variable
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskVarSet(int taskId, int *pVar, int value)
{
  TASK_VAR *pTaskVar;
  TCB_ID tcbId;

  /* Get task tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* For all variables in list */
  for (pTaskVar = tcbId->pTaskVar;
       pTaskVar != NULL;
       pTaskVar = pTaskVar->next) {

    /* If match */
    if (pTaskVar->addr == pVar) {

      /* If current task owner */
      if (tcbId == taskIdCurrent)
        *pVar = value;
      else
        pTaskVar->value = value;

      return OK;

    } /* End if match */

  } /* End for all variables in list */

  /* Error if here */
  errnoSet(S_taskLib_TASK_VAR_NOT_FOUND);
  return ERROR;
}

/*******************************************************************************
 * taskVarGet - Get value of a task variable
 *
 * RETURNS: Value of task variable
 ******************************************************************************/

int taskVarGet(int taskId, int *pVar)
{
  TASK_VAR *pTaskVar;
  TCB_ID tcbId;

  /* Get task tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* For all variables in list */
  for (pTaskVar = tcbId->pTaskVar;
       pTaskVar != NULL;
       pTaskVar = pTaskVar->next) {

    /* If match */
    if (pTaskVar->addr == pVar) {

      /* If current task owner */
      if (tcbId == taskIdCurrent)
        return *pVar;

      /* Any other task owner */
      return pTaskVar->value;

    } /* End if match */

  } /* End for all variables in list */

  /* Error if here */
  errnoSet(S_taskLib_TASK_VAR_NOT_FOUND);
  return ERROR;
}

/*******************************************************************************
 * taskVarInfo - Get task variable info for a task
 *
 * RETURNS: Number of variables in tasks variable list
 ******************************************************************************/

int taskVarInfo(int taskId, TASK_VAR varList[], int max)
{
  TASK_VAR *pTaskVar;
  TCB_ID tcbId;
  int i;

  /* Get task tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* Safety first */
  taskLock();

  /* For all variables in list */
  for (pTaskVar = tcbId->pTaskVar, i = 0;
       (pTaskVar != NULL) && (i < max);
       pTaskVar = pTaskVar->next, i++) {

    /* Get value */
    if (varList != NULL)
      varList[i] = *pTaskVar;

  } /* End for all variables in list */

  taskUnlock();

  return i;
}

/*******************************************************************************
 * taskVarDelete - Remove a task variable
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskVarDelete(int taskId, int *pVar)
{
  TCB_ID tcbId;
  TASK_VAR **ppTaskVar;
  TASK_VAR *pTaskVar;
  int level;

  /* Check library */
  if (!taskVarLibInstalled)
    return ERROR;

  /* Get task tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* For all variables tasks variable list */
  for (ppTaskVar = &tcbId->pTaskVar;
       *ppTaskVar != NULL;
       ppTaskVar = &( (*ppTaskVar)->next ) ) {

    /* Get varable pointer */
    pTaskVar = *ppTaskVar;

    /* If variable matches */
    if (pTaskVar->addr == pVar) {

      /* Store variable if current task */
      if (tcbId == taskIdCurrent)
        *pVar = pTaskVar->value;

      /* Get next variable in list */
      *ppTaskVar = pTaskVar->next;

      /* Free old variable */
      free(pTaskVar);

      return OK;

    } /* End if variable matches */

  } /* End for all variables tasks variable list */

  /* Error if here */
  errnoSet(S_taskLib_TASK_VAR_NOT_FOUND);
  return ERROR;
}

/*******************************************************************************
 * taskVarSwitchHook - Called when task switch occurs
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void taskVarSwitchHook(TCB_ID oldTcbId, TCB_ID newTcbId)
{
  int i;
  TASK_VAR *pTaskVar;

  /* If old task */
  if (TASK_ID_VERIFY(oldTcbId) == OK) {

    /* For all variables in list */
    for (pTaskVar = oldTcbId->pTaskVar;
	 pTaskVar != NULL;
	 pTaskVar = pTaskVar->next) {

      /* Swap */
      i = pTaskVar->value;
      pTaskVar->value = *(pTaskVar->addr);
      *(pTaskVar->addr) = i;

    } /* End for all variables in list */

  } /* End if old task */

  /* New task */

  /* For all variables in list */
  for (pTaskVar = newTcbId->pTaskVar;
       pTaskVar != NULL;
       pTaskVar = pTaskVar->next) {

      /* Swap */
      i = pTaskVar->value;
      pTaskVar->value = *(pTaskVar->addr);
      *(pTaskVar->addr) = i;

    } /* End for all variables in list */
}

/*******************************************************************************
 * taskVarDeleteHook - Called when task delete occurs
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void taskVarDeleteHook(TCB_ID tcbId)
{
  TASK_VAR *pTaskVar, *pTaskVarNext;

  /* Initialize locals */
  pTaskVarNext = NULL;

  /* For all variables in list */
  for (pTaskVar = tcbId->pTaskVar;
       pTaskVar != NULL;
       pTaskVar = pTaskVarNext) {

    /* Get next variable */
    pTaskVarNext = pTaskVar->next;

    /* Free storage */
    free(pTaskVar);

  } /* End for all variables in list */
}

