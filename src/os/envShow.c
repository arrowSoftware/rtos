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

/* envShow.c - Environment variables show */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <os/envLib.h>
#include <os/envShow.h>

/* Defines */

/* Imports */
IMPORT char **ppGlobalEnviron;
IMPORT int globalEnvTableSize;
IMPORT int globalEnvNEntries;

/* Locals */

/* Globals */

/* Functions */

/*******************************************************************************
 * envShowInit - Initialize environment show library
 *
 * RETURNS: N/A
 ******************************************************************************/

void envShowInit(void)
{
}

/*******************************************************************************
 * envShow - Show environment
 *
 * RETURNS: N/A
 ******************************************************************************/

void envShow(int taskId)
{
  TCB_ID tcbId;
  char **ppEnv;
  int i, nEnvEntries;

  /* Initialize locals */
  tcbId = taskTcb(taskId);

  /* If global environment */
  if (tcbId->ppEnviron == NULL) {

    printf("(global environment)\n");
    ppEnv = ppGlobalEnviron;
    nEnvEntries = globalEnvNEntries;

  }

  /* Else private environment */
  else {

    printf("(private environment)\n");
    ppEnv = tcbId->ppEnviron;
    nEnvEntries = tcbId->nEnvVarEntries;

  }

  /* For all environment variables */
  for (i = 0; i < nEnvEntries; i++, ppEnv++)
    printf("%d: %s\n", i, *ppEnv);
}

