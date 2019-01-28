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

/* taskShow.c - Show task info */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <rtos.h>
#include <arch/regs.h>
#include <arch/intArchLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskShow.h>
#include <rtos/errnoLib.h>
#include <util/qLib.h>
#include <rtos/taskInfo.h>
#include <rtos/eventShow.h>
#include <os/symLib.h>
#include <os/symbol.h>
#include <rtos/taskShow.h>

/* Defines */
#define MAX_DSP_TASKS		500		/* Max tasks to be displayed */

/* Imports */
IMPORT Q_HEAD kernTickQ;
IMPORT REG_INDEX taskRegName[];
IMPORT SYMTAB_ID sysSymTable;
IMPORT FUNCPTR _func_symFindSymbol;
IMPORT FUNCPTR _func_symNameGet;
IMPORT FUNCPTR _func_symValueGet;
IMPORT FUNCPTR _func_excInfoShow;

/* Globals */
char * taskRegsFormat ="%-6s = %8x";
char taskRegsFmt[] = "%-6s = %8x";
VOIDFUNCPTR _func_taskRegsShowFunc = NULL;

/* Locals */
LOCAL char infoHead[] = "\n\
  NAME        ENTRY       TID    PRI   STATUS      PC       SP     ERRNO  DELAY\
\n\
---------- ------------ -------- --- ---------- -------- -------- ------- -----\
\n";

LOCAL void taskSummary(TASK_DESC *pTd);

/*******************************************************************************
 * taskShowInit - Install task show routine
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskShowInit(void)
{
  return classShowConnect(taskClassId, taskShow);
}

/*******************************************************************************
 * taskInfoGet - Get info about task
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskInfoGet(int taskId, TASK_DESC *pTaskDesc)
{
  int level;
  TCB *pTcb;
  TASK_DESC *pTd;
  char *pStackHigh;

  /* Setup locals */
  pTcb = taskTcb(taskId);
  pTd = pTaskDesc;

  /* Check task */
  if (pTcb == NULL)
    return ERROR;

  /* Get information */
  pTd->td_id		=	(int) pTcb;
  pTd->td_name		=	pTcb->name;
  pTd->td_priority	=	(int) pTcb->priority;
  pTd->td_status	=	pTcb->status;
  pTd->td_options	=	pTcb->options;
  pTd->td_entry		=	pTcb->entry;
  pTd->td_sp		=	(char *) ( (int)pTcb->regs.spReg );
  pTd->td_pStackBase	=	pTcb->pStackBase;
  pTd->td_pStackLimit	=	pTcb->pStackLimit;
  pTd->td_pStackEnd	=	pTcb->pStackEnd;

  /* Calculate stack status */
#if (_STACK_DIR == _STACK_GROWS_DOWN)

  if (pTcb->options & TASK_OPTIONS_NO_STACK_FILL)
    pStackHigh = pTcb->pStackLimit;
  else
    for (pStackHigh = pTcb->pStackLimit;
	 *(unsigned char *) pStackHigh == 0xee;
	 pStackHigh++);

#else /* _STACK_GROWS_UP */

  if (pTcb->options & TASK_OPTIONS_NO_STACK_FILL)
    pStackHigh = pTcb->pStackLimit - 1;
  else
    for (pStackHigh = pTcb->pStackLimit - 1;
	 *(unsigned char *) pStackHigh == 0xee;
	 pStackHigh--);

#endif /* _STACK_DIR */

  /* Store stack status */
  pTd->td_stackSize	=	(int) (pTcb->pStackLimit - pTcb->pStackBase) *
				_STACK_DIR;
  pTd->td_stackHigh	=	(int) (pStackHigh - pTcb->pStackBase) *
				_STACK_DIR;
  pTd->td_stackMargin	=	(int) (pTcb->pStackLimit - pStackHigh) *
				_STACK_DIR;
  pTd->td_stackCurrent	=	(int) (pTd->td_sp - pTcb->pStackBase) *
				_STACK_DIR;

  /* Get error status */
  pTd->td_errorStatus	=	errnoOfTaskGet((int) pTcb);

  /* If task is delayed, get end time */
  if (pTcb->status & TASK_DELAY)
    pTd->td_delay = Q_KEY(&kernTickQ, &pTcb->tickNode, 1);
  else
    pTd->td_delay = 0;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Copy events */
  pTd->td_events = pTcb->events;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  return OK;
}

/*******************************************************************************
 * taskShow - Print information about task(s)
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskShow(int taskId, int level)
{
  TCB_ID tcbId;
  TASK_DESC td;
  int i, nTasks;
  int idList[MAX_DSP_TASKS];
  char optString[256];

  /* Get/set default task id */
  taskId = taskIdDefault(taskId);

  /* Select show level */
  switch (level) {

    /* Summarize info for one task */
    case 0:
      if (taskInfoGet(taskId, &td) != OK) {
        fprintf(stderr, "Task not found.\n");
        return ERROR;
      }

      /* Print info header */
      printf(infoHead);

      /* Print task summary */
      taskSummary(&td);

    break;

    /* Detailed info about a single task */
    case 1:

      if (taskInfoGet(taskId, &td) != OK) {
        fprintf(stderr, "Task not found.\n");
        return ERROR;
      }

      /* Get task options string */
      taskOptionsString(taskId, optString);

      /* Print info header */
      printf(infoHead);

      /* Print task summary */
      taskSummary(&td);

      /* Print stack info */
      printf("\nstack: base 0x%-6x  end 0x%-6x  size %-5d  ",
	     (int) td.td_pStackBase,
	     (int) td.td_pStackEnd,
	     td.td_stackSize);

      /* Extra stack info */
      if (td.td_options & TASK_OPTIONS_NO_STACK_FILL)
        printf("high %5s  margin %5s\n", "???", "???");
      else
        printf("high: %-5d  margin %-5d\n", td.td_stackHigh, td.td_stackMargin);

      /* Print task options */
      printf("\noptions: 0x%x\n%s\n", td.td_options, optString);

      /* Show events */
      eventTaskShow(&td.td_events);

      /* If not myself */
      if ( taskId != taskIdSelf() ) {

        /* Show task regs */
        taskRegsShow(taskId);

      } /* End if not myself */

      /* If print exception info */
      if (_func_excInfoShow != NULL) {

        tcbId = taskTcb(taskId);
        if (tcbId != NULL)
          ( *_func_excInfoShow) (&tcbId->excInfo, FALSE);

      } /* If print exception info */

    break;

    /* Summarize info for all tasks */
    case 2:

      /* Print info header */
      printf(infoHead);

      /* Get number of active tasks */
      nTasks = taskIdListGet(idList, MAX_DSP_TASKS);

      /* Sort by priority level */
      taskIdListSort(idList, nTasks);

      /* Print summary for each task */
      for (i = 0; i < nTasks; i++) {
        if (taskInfoGet(idList[i], &td) == OK)
          taskSummary(&td);
      }

    break;

  } /* End switch level */

  return OK;
}

/*******************************************************************************
 * taskStatusString - Get task status string
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskStatusString(int taskId, char *str)
{
  TCB_ID tcbId;

  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* Select status string */
  switch (tcbId->status) {

    case TASK_READY:	strcpy(str, "READY");
			break;

    case TASK_DELAY:	strcpy(str, "DELAY");
			break;

    case TASK_PEND:	strcpy(str, "PEND");
			break;

    case TASK_SUSPEND:	strcpy(str, "SUSPEND");
			break;

    case TASK_DEAD:	strcpy(str, "DEAD");
			break;

    case TASK_DELAY |
	 TASK_SUSPEND:	strcpy(str, "DELAY+S");
			break;

    case TASK_PEND |
	 TASK_DELAY:	strcpy(str, "PEND+T");
			break;

    case TASK_PEND |
	 TASK_SUSPEND:	strcpy(str, "PEND+S");

			break;
    case TASK_PEND |
	 TASK_DELAY |
	 TASK_SUSPEND:	strcpy(str, "PEND+S+T");
			break;

    default:		strcpy(str, "UNKNOWN");
			return ERROR;

  } /* End select status string */

  return OK;
}

/*******************************************************************************
 * taskOptionsString - Get task options string
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS taskOptionsString(int taskId, char *str)
{
  TCB_ID tcbId;

  /* Get task TCB */
  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  /* Null terminate string */
  str[0] = EOS;

  /* Get options string */
  if (tcbId->options & TASK_OPTIONS_SUPERVISOR_MODE)
    strcat(str, "SUPERVISOR_MODE     ");

  /* Get options string */
  if (tcbId->options & TASK_OPTIONS_UNBREAKABLE)
    strcat(str, "UNBREAKABLE         ");

  if (tcbId->options & TASK_OPTIONS_DEALLOC_STACK)
    strcat(str, "DEALLOC_STACK       ");

  if (tcbId->options & TASK_OPTIONS_NO_STACK_FILL)
    strcat(str, "NO_STACK_FILL       ");

  return OK;
}

/*******************************************************************************
 * taskSummary - Print task summary
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void taskSummary(TASK_DESC *pTd)
{
  char str[10];
  REG_SET regSet;
  SYMBOL_ID symId;
  char *name;
  void *value;

  /* Get status string */
  taskStatusString(pTd->td_id, str);

  /* Print name */
  printf("%-11.11s", pTd->td_name);

  /* If symFind function exists */
  if (_func_symFindSymbol != NULL) {

    /* If symbol found */
    if ( ( *_func_symFindSymbol) (sysSymTable, NULL, pTd->td_entry,
		N_TEXT | N_EXT, N_TEXT | N_EXT, &symId) == OK ) {

      /* Get name and value */
      ( *_func_symNameGet) (symId, &name);
      ( *_func_symValueGet) (symId, &value);

    } /* End if symbol found */

  } /* End if symFind function exists */

  /* Print entry */
  if (pTd->td_entry == (FUNCPTR) value)
    printf("%-12.12s", name);
  else
    printf("%-12x", (int) pTd->td_entry);

  /* Get register set */
  taskRegsGet(pTd->td_id, &regSet);

  /* Print summary */
  printf(" %8x %3d %-10.10s %8x %8x %7x %5u\n",
	pTd->td_id,
	pTd->td_priority,
	str,
	( (taskIdSelf() == pTd->td_id) ? (int) taskSummary : (int) regSet.pc ),
	(int) regSet.spReg,
	pTd->td_errorStatus,
	pTd->td_delay);
}

/*******************************************************************************
 * taskRegsShow - Show task register set
 *
 * RETURNS: N/A
 ******************************************************************************/

void taskRegsShow(int taskId)
{
  int i;
  int *pRegValue;
  REG_SET regSet;

  /* If external function set */
  if (_func_taskRegsShowFunc != NULL) {

    ( *_func_taskRegsShowFunc) (taskId);
    return;

  }

  /* Get register set */
  if (taskRegsGet(taskId, &regSet) != OK) {

    fprintf(stderr, "taskRegsShow: invalid task id %#x\n", taskId);
    return;

  }

  /* For all register names */
  for (i = 0; taskRegName[i].regName != NULL; i++) {

    if ( (i % 4) == 0)
      printf("\n");
    else
      printf("%3s", "");

    /* If register name */
    if (taskRegName[i].regName[0] != EOS) {

      pRegValue = (int *) ( (int) &regSet + taskRegName[i].regOffset );
      printf(taskRegsFmt, taskRegName[i].regName, *pRegValue);

    } /* End if register name */

    /* Else no register name */
    else {

      printf("%17s", "");

    } /* End else no register name */

  } /* End of all register names */

  printf("\n");

}

/*******************************************************************************
 * taskIdListSort - Sort list of task ids
 *
 * RETURNS: N/A
 ******************************************************************************/

void taskIdListSort(int idList[], int nTasks)
{
  int currPrio, prevPrio;
  int *pCurrId, *pEndId;
  int tmp;
  BOOL change;

  if (!nTasks)
    return;

  /* Setup locals */
  change = TRUE;
  pEndId = &idList[nTasks];

  /* While change */
  while (change == TRUE) {

    change = FALSE;

    /* Get start priority */
    taskPriorityGet(idList[0], &prevPrio);

    /* For all tasks */
    for (pCurrId = &idList[1];
	 pCurrId < pEndId;
	 ++pCurrId, prevPrio = currPrio) {

      /* Get current task priority */
      taskPriorityGet(*pCurrId, &currPrio);

      /* If swap */
      if (prevPrio > currPrio) {

        tmp = *pCurrId;
        *pCurrId = *(pCurrId - 1);
        *(pCurrId - 1) = tmp;
        change = TRUE;

      } /* End if swap */

    } /* End for all tasks */

  } /* End while change */
}

