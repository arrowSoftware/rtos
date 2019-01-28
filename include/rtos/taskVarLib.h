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

/* taskVarLib.h - Task variables library header */

#ifndef _taskVarLib_h
#define _taskVarLib_h

#include <rtos.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct taskVar {

  struct taskVar *next;
  int *addr;
  int value;

} TASK_VAR;

/* Functions */
IMPORT STATUS taskVarLibInit(void);
IMPORT STATUS taskVarAdd(int taskId, int *pVar);
IMPORT STATUS taskVarSet(int taskId, int *pVar, int value);
IMPORT int taskVarGet(int taskId, int *pVar);
IMPORT int taskVarInfo(int taskId, TASK_VAR varList[], int max);
IMPORT STATUS taskVarDelete(int taskId, int *pVar);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _taskVarLib_h */

