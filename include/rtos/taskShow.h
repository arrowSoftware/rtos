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

/* taskShow.h - Show task information header */

#ifndef _taskShow_h
#define _taskShow_h

#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/eventLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Typedefs */
typedef struct {
  int			td_id;			/* Task id */
  char*			td_name;		/* Task name */
  int			td_priority;		/* Task priority */
  int			td_status;		/* Task status */
  int			td_options;		/* Task options */
  FUNCPTR		td_entry;		/* Task entry point */
  char*			td_sp;			/* Task saved stack pointer */
  char*			td_pStackBase;		/* Task bottom stack pointer */
  char*			td_pStackLimit;		/* Task end of stack */
  char*			td_pStackEnd;		/* Stack end */
  int			td_stackSize;		/* Task stack size */
  int			td_stackHigh;		/* Maximum stack usage */
  int			td_stackMargin;		/* Current stack margin */
  int			td_stackCurrent;	/* Current stack usage */
  int			td_errorStatus;		/* Task recent error status */
  int			td_delay;		/* Task delay timeout ticks */
  EVENTS		td_events;		/* Task event info */
} TASK_DESC;

/* Functions */
IMPORT STATUS taskInfoGet(int taskId, TASK_DESC *pTaskDesc);
IMPORT STATUS taskShow(int taskId, int level);
IMPORT STATUS taskStatusString(int taskId, char *str);
IMPORT STATUS taskOptionsString(int taskId, char *str);
IMPORT void taskRegsShow(int taskId);
IMPORT void taskIdListSort(int idList[], int nTasks);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _taskShow_h */

