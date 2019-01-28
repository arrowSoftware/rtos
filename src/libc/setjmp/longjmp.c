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

/* longjmp.c - Non-local goto */

#include <rtos.h>
#include <rtos/taskLib.h>
#include <os/private/sigLibP.h>
#include <stdlib.h>
#include <setjmp.h>

#define	JMP_DATA(env)	( (int *) ( ((char *) env) + sizeof(jmp_buf) -	       \
				    2 * sizeof(int) ) )

/* Imports */
IMPORT TCB_ID taskIdCurrent;

IMPORT void _sigCtxLoad(REG_SET *env);
IMPORT void _sigCtxRetValueSet(REG_SET *pRegSet, int val);
IMPORT FUNCPTR _func_sigprocmask;

/*******************************************************************************
* _setjmpSetup - Setup jump goto
*
* RETURNS: N/A
*******************************************************************************/

void _setjmpSetup(jmp_buf env, int val)
{
  int *ptr;
  struct sigtcb *pSigTcb;

  /* Set ptr to jmp_buf extra area */
  ptr = JMP_DATA(env);

  /* Store current task id */
  ptr[0] = (int) taskIdCurrent;

  /* Store signal info if needed */
  if (val) {

    ptr[0] |= 1;

    if (taskIdCurrent->pSignalInfo == NULL) {

      ptr[1] = 0;

    }

    else {

      pSigTcb = taskIdCurrent->pSignalInfo;
      ptr[1] = pSigTcb->sigt_blocked;

    }

  }
}

/*******************************************************************************
* longjmp - Do a goto
*
* RETURNS: N/A
*******************************************************************************/

void longjmp(jmp_buf env, int val)
{
  int *ptr;

  /* Set ptr to jmp_buf extra area */
  ptr = JMP_DATA(env);

  /* Suspend task if needed */
  if ( (ptr[0] & ~1) != (int) taskIdCurrent)
    taskSuspend(0);

  /* Call sigprocmask() if needed and function is set */
  if ( (ptr[0] & 1) && (_func_sigprocmask != NULL) )
    _func_sigprocmask(SIG_SETMASK, &ptr[1], 0);

  /* Set return value */
  if (val == 0)
    _sigCtxRetValueSet((REG_SET *) env, 1);
  else
    _sigCtxRetValueSet((REG_SET *) env, val);

  /* Load task context */
  _sigCtxLoad((REG_SET *) env);
}

