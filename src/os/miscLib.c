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

/* miscLib.c - Misc os routines library */

/* Includes */
#include <stdlib.h>
#include <errno.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/listLib.h>
#include <rtos/taskLib.h>
#include <rtos/semLib.h>
#include <os/miscLib.h>
#include <net/netLib.h>
#include <os/logLib.h>

/* Defines */

/* Imports */
IMPORT TCB_ID taskIdCurrent;

/* Locals */
LOCAL SEM_ID splSemId;

/* Globals */
int splTaskId;
int splMutexOptions = SEM_Q_PRIORITY | SEM_DELETE_SAFE;
BOOL panicSuspend = TRUE;
FUNCPTR panicHookFunc = NULL;

/* Functions */

/*******************************************************************************
 * miscLibInit - Inititalize misc library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS miscLibInit(void)
{
#ifndef NO_SPLSEM

  splSemId = semMCreate(splMutexOptions);
  if (splSemId == NULL)
    return ERROR;

#endif /* NO_SPLSEM */

  return OK;
}

/*******************************************************************************
 * splnet - Get network processor level
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

int splnet(void)
{
#ifndef NO_SPLSEM

  if ( splTaskId == ((int) taskIdCurrent) )
    return TRUE;

  semTake(splSemId, WAIT_FOREVER);

  splTaskId = (int) taskIdCurrent;

  return FALSE;

#else /* SPLSEM */

  return TRUE;

#endif /* NO_SPLSEM */
}

/*******************************************************************************
 * splimp - Set imp processor level
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

int splimp(void)
{
#ifndef NO_SPLSEM

  if ( splTaskId == ((int) taskIdCurrent) )
    return TRUE;

  semTake(splSemId, WAIT_FOREVER);

  splTaskId = (int) taskIdCurrent;

  return FALSE;

#else /* SPLSEM */

  return TRUE;

#endif /* NO_SPLSEM */
}

/*******************************************************************************
 * splx - Set processor level
 *
 * RETURNS: N/A
 ******************************************************************************/

void splx(int x)
{
#ifndef NO_SPLSEM

  if (!x) {

    splTaskId = 0;
    semGive(splSemId);

  }

#endif /* NO_SPLSEM */
}

/*******************************************************************************
 * sleep - Got to sleep
 *
 * RETURNS: N/A
 ******************************************************************************/

void sleep(SEM_ID semId)
{
#ifdef DEBUG_SPLSEM
  printf("=========TAKE SEM========\n");
#endif

#ifndef NO_SPLSEM

  BOOL gotSplSem;

  /* Check if current task is spl task */
  if (splTaskId == (int) taskIdCurrent)
    gotSplSem = TRUE;
  else
    gotSplSem = FALSE;

  /* If had spl semaphore */
  if (gotSplSem) {

    splTaskId = 0;
    semGive(splSemId);

  } /* End if had spl semaphore */

  semTake(semId, WAIT_FOREVER);

  /* If had spl semaphore */
  if (gotSplSem) {

    semTake(splSemId, WAIT_FOREVER);
    splTaskId = (int) taskIdCurrent;

  } /* End if had spl semaphore */

#else /* SPLSEM */

  taskDelay(60);

#endif /* NO_SPLSEM */
}

/*******************************************************************************
 * wakeup - Wakeup
 *
 * RETURNS: N/A
 ******************************************************************************/

void wakeup(SEM_ID semId)
{
#ifdef DEBUG_SPLSEM
  printf("=========GIVE SEM========\n");
#endif

#ifndef NO_SPLSEM

  semGive(semId);

#endif /* NO_SPLSEM */
}

/*******************************************************************************
 * panic - Suspend task
 *
 * RETURNS: N/A
 ******************************************************************************/

void panic(char *msg)
{

  /* If panic hook set */
  if (panicHookFunc != NULL) {

    ( *panicHookFunc) (msg);
    return;

  } /* End if panic hook set */

  logMsg("panic: %s\n", (ARG) msg, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);

  if ( (!INT_CONTEXT()) && (panicSuspend) )
    taskSuspend(0);
}

/*******************************************************************************
 * insque - Insert into queue
 *
 * RETURNS: N/A
 ******************************************************************************/

void insque(void *node, void *prev)
{
  LIST_NODE *pNode, *pPrev, *pNext;

  /* Initialize locals */
  pNode = (LIST_NODE *) node;
  pPrev = (LIST_NODE *) prev;
  pNext = pPrev->next;

  /* Insert node */
  pPrev->next = pNode;
  pNext->prev = pNode;
  pNode->next = pNext;
  pNode->prev = pPrev;
}

/*******************************************************************************
 * remque - Remove from queue
 *
 * RETURNS: N/A
 ******************************************************************************/

void remque(void *node)
{
  LIST_NODE *pNode;

  /* Initialize locals */
  pNode = (LIST_NODE *) node;

  /* Remove node */
  pNode->prev->next = pNode->next;
  pNode->next->prev = pNode->prev;
}

/*******************************************************************************
 * hashinit - Initialize hash table
 *
 * RETURNS: Pointer to table
 ******************************************************************************/

void* hashinit(int elements, int type, unsigned long *hashmask)
{
  LIST *hashtable;
  long hashsize;
  int i;

  /* If le. zero elements */
  if (elements <= 0)
    panic("hashinit: bad elements");

  /* For all elements log 2 */
  for (hashsize = 1; hashsize <= elements; hashsize <<= 1)
    continue;

  /* Shift size size */
  hashsize >>= 1;

  /* Allocate hash table */
  hashtable = malloc((size_t) hashsize * sizeof(LIST));
  if (hashtable == NULL)
    return NULL;

  /* For all lists */
  for (i = 0; i < hashsize; i++)
    listInit(&hashtable[i]);

  /* Set mask parameter */
  *hashmask = hashsize - 1;

  return hashtable;
}

