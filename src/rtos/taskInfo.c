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

/* taskInfo.c - Task data set/get methods */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/regs.h>
#include <arch/intArchLib.h>
#include <util/qLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskInfo.h>

/* Imports */
Q_NODE kernActiveQ;

/* Globals */
int defaultTaskId = 0;

/* Locals */
LOCAL BOOL taskNameNoMatch(Q_NODE *pNode, char *name);

/*******************************************************************************
 * taskIdDefault - Get/set default task
 *
 * RETURNS: Default taskId
 ******************************************************************************/

int taskIdDefault(int taskId)
{

  /* If nonzero agument, set default id */
  if (taskId != 0)
    defaultTaskId = taskId;

  return defaultTaskId;
}

/*******************************************************************************
 * taskName - Get task name
 *
 * RETURNS: Task name string
 ******************************************************************************/

char* taskName(int taskId)
{
  TCB_ID tcbId;

  /* Get task tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return (char*) NULL;

  if (tcbId->name == NULL)
    return "\0";

  return tcbId->name;
}

/*******************************************************************************
 * taskNameToId - Get task id from task name
 *
 * RETURNS: Task id
 ******************************************************************************/

int taskNameToId(char *name)
{
  int taskId;

  /* Try to match each node in the active queue against name */
  taskId = (int) Q_EACH(&kernActiveQ, taskNameNoMatch, name);

  /* If no match was found */
  if (taskId == (int) NULL)
    return ERROR;

  return ( taskId - OFFSET(TCB, activeNode) );
}

/*******************************************************************************
 * taskNameNoMatch - Check for task name mismatch
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL taskNameNoMatch(Q_NODE *pNode, char *name)
{
  TCB_ID tcbId;

  /* Get active node */
  tcbId = (TCB_ID) ( (int) pNode - OFFSET(TCB, activeNode) );

  /* Return boolean state */
  return ( (tcbId->name == NULL) || (strcmp(tcbId->name, name) != 0) );
}

/*******************************************************************************
 * taskRegsGet - Get register set from task
 *
 * RETURNS: Default taskId
 ******************************************************************************/

STATUS taskRegsGet(int taskId, REG_SET *pRegSet)
{
  TCB_ID tcbId;

  /* Get tcb */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* Copy exception regs to regs set */
  if (tcbId->pExcRegSet != NULL) {

    memcpy(&tcbId->regs, tcbId->pExcRegSet, sizeof(REG_SET));
    tcbId->pExcRegSet = NULL;
  }

  memcpy(pRegSet, &tcbId->regs, sizeof(REG_SET));

  return OK;
}

/*******************************************************************************
 * taskIdListGet - Get a list of active task ids
 *
 * RETURNS: Number of task id active
 ******************************************************************************/

int taskIdListGet(int idList[], int maxTasks)
{
  int i, count;

  /* Lock task */
  taskLock();

  /* Get number of nodes in active queue */
  count = Q_INFO(&kernActiveQ, idList, maxTasks);

  /* Unlock task */
  taskUnlock();

  /* Extract active node from TCB */
  for (i = 0; i < count; i++)
    idList[i] -= OFFSET(TCB, activeNode);

  return count;
}

/*******************************************************************************
 * taskOptionsSet - Set task options
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskOptionsSet(int tid, int mask, int options)
{
  TCB_ID tcbId;

  if (INT_RESTRICT() != OK)
    return ERROR;

  taskLock();

  tcbId = taskTcb(tid);
  if (tcbId == NULL) {

    taskUnlock();
    return ERROR;

  }

  tcbId->options = (tcbId->options & mask) | options;

  taskUnlock();

  return OK;
}

