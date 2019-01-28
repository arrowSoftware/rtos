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

/* sigCtxArchLib.c - Signal processing library */

#include <string.h>
#include <rtos.h>
#include <os/private/sigLibP.h>
#include <arch/regs.h>

/* Imports */
IMPORT void kernTaskEntry(void);

/* Exception signals */
struct sigfaulttable _sigfaulttable[] = {
  {  0, SIGFPE },
  {  1, SIGEMT },
  {  2, SIGILL },
  {  3, SIGEMT },
  {  4, SIGILL },
  {  5, SIGILL },
  {  6, SIGILL },
  {  7, SIGFPE },
  {  8, SIGILL },
  {  9, SIGFPE },
  { 10, SIGILL },
  { 11, SIGBUS },
  { 12, SIGBUS },
  { 13, SIGILL },
  { 14, SIGBUS },
  { 15, SIGILL },
  { 16, SIGFPE },
  { 17, SIGBUS },
  {  0, 0 }
};

/*****************************************************************************
* _sigCtxRetValueSet - Set return value of context
*
* RETURNS: N/A
*****************************************************************************/

void _sigCtxRetValueSet(REG_SET *pRegSet, int val)
{
  pRegSet->eax = val;
}

/*****************************************************************************
* _sigCtxStackEnd - Get end of stack for context
*
* RETURNS: Pointer to stacke end
*****************************************************************************/

void* _sigCtxStackEnd(const REG_SET *pRegSet)
{
  return (void *) (pRegSet->esp - 12);
}

/*****************************************************************************
* _sigCtxStackSetup - Setup context
*
* RETURNS: Pointer to stacke end
*****************************************************************************/

void _sigCtxSetup(REG_SET *pRegSet,
		  void *pStackBase, void (*taskEntry)(), int *pArgs)
{
  int i;
  union {
    void *pv;
    int *pi;
    void (**ppfv)();
    int i;
  } pu;

  /* Zero regset */
  memset(pRegSet, 0, sizeof(*pRegSet));

  /* Setup stack */
  pu.pv = (void *) ((int) pStackBase & ~3);
  for (i = pArgs[0]; i > 0; --i)
    *--pu.pi = pArgs[i];

  /* Setup entry */
  *--pu.ppfv = kernTaskEntry;
  pRegSet->esp = pu.i;
  pRegSet->pc = (INSTR *) taskEntry;
  pRegSet->eflags = EFLAGS_BRANDNEW;
}

