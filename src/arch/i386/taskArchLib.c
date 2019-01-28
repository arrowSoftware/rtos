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

/* taskArchLib.c - Task switching functions */

#include <stdlib.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <arch/regs.h>
#include <arch/taskArchLib.h>

/* Imports */
IMPORT void kernTaskEntry(void);

/* Globals */
REG_INDEX taskRegName[] = {
  {"edi", G_REG_OFFSET(0)},
  {"esi", G_REG_OFFSET(1)},
  {"ebp", G_REG_OFFSET(2)},
  {"esp", G_REG_OFFSET(3)},
  {"ebx", G_REG_OFFSET(4)},
  {"edx", G_REG_OFFSET(5)},
  {"ecx", G_REG_OFFSET(6)},
  {"eax", G_REG_OFFSET(7)},
  {"eflags", SR_OFFSET},
  {"pc", PC_OFFSET},
  {NULL, 0}
};

/*******************************************************************************
* taskRegsInit - Initialize architecture depedant tcb data
*
* RETURNS: N/A
*******************************************************************************/

void taskRegsInit(TCB *pTcb, char *pStackBase)
{
ARG args[MAX_TASK_ARGS];
int i;
for (i = 0; i < MAX_TASK_ARGS; i++)
  args[i] = (ARG) i;

  pTcb->regs.eflags = TASK_INITIAL_FLAGS;
  pTcb->regs.pc = (INSTR *) kernTaskEntry;

  pTcb->regs.edi = 0;
  pTcb->regs.esi = 0;
  pTcb->regs.ebp = 0;
  pTcb->regs.ebx = 0;
  pTcb->regs.edx = 0;
  pTcb->regs.ecx = 0;
  pTcb->regs.eax = 0;

  /* Setup initial stack */
  pTcb->regs.esp = (u_int32_t) (pStackBase - (MAX_TASK_ARGS * sizeof(u_int32_t)) );
}

/*******************************************************************************
* taskRetValueSet - Set task return value
*
* RETURNS: N/A
*******************************************************************************/

void taskRetValueSet(TCB_ID pTcb, int val)
{
  pTcb->regs.eax = val;
}

/*******************************************************************************
* taskArgSet - Setup task arguments
*
* RETURNS: N/A
*******************************************************************************/

void taskArgSet(TCB_ID pTcb, char *pStackBase, ARG args[])
{
  int i;
  ARG *sp;

  sp = (ARG *) pStackBase;

  /* Push args on stack */
  for (i = MAX_TASK_ARGS - 1; i >= 0; --i)
    *--sp = (ARG) args[i];
}

/*******************************************************************************
* taskArgGet - Read task arguments
*
* RETURNS: N/A
*******************************************************************************/

void taskArgGet(TCB_ID pTcb, char *pStackBase, ARG args[])
{
  int i;
  ARG *sp;

  sp = (ARG *) pStackBase;

  /* Push args on stack */
  for (i = MAX_TASK_ARGS - 1; i >= 0; --i)
    args[i] = *--sp;
}

