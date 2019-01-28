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

/* listLib.c - A double linked list library */

#include <rtos.h>
#include <util/listLib.h>
#include <stdlib.h>

/**************************************************************
* listInit - Initialize linked list datastruct
*
* RETURNS: OK
**************************************************************/

STATUS listInit(LIST *pList)
{
  pList->head = NULL;
  pList->tail = NULL;
  pList->count = 0;

  return(OK);
}

/**************************************************************
* listInsert - Insert element in list
*
* RETURNS: N/A
**************************************************************/

void listInsert(LIST *pList,
		LIST_NODE *pPrev,
		LIST_NODE *pNode)
{
  LIST_NODE *pNext;
  
  if (pPrev == NULL) {
    pNext = pList->head;	/* Put node at beginnig of list */
    pList->head = pNode;
  }
  else {
    pNext = pPrev->next;	/* next points to previous next */
    pPrev->next = pNode;
  }

  if (pNext == NULL)
    pList->tail = pNode;	/* Put node at end of list */
  else
    pNext->prev = pNode;	/* next previous points to node */

  /* Setup pointers */
  pNode->next = pNext;
  pNode->prev = pPrev;

  pList->count++;
}

/**************************************************************
* listAdd - Add an element to the list
*
* RETURNS: N/A
**************************************************************/

void listAdd(LIST *pList, LIST_NODE *pNode)
{
  listInsert(pList, pList->tail, pNode);
}

/**************************************************************
* listGet - Get Node from list
*
* RETURNS: Node
**************************************************************/

LIST_NODE *listGet(LIST *pList)
{
  LIST_NODE *pNode = pList->head;

  if (pNode != NULL)
  {
    pList->head = pNode->next;

    if (pNode->next == NULL)
      pList->tail = NULL;
    else
      pNode->next->prev = NULL;

    pList->count--;
  }

  return (pNode);
}

/**************************************************************
* listRemove - Remove an element from the list
*
* RETURNS: N/A
**************************************************************/

void listRemove(LIST *pList, LIST_NODE *pNode)
{
  /* Is this the first node? */
  if (pNode->prev == NULL)
    pList->head = pNode->next;
  else
    pNode->prev->next = pNode->next;

  /* Is this the last node? */
  if (pNode->next == NULL)
    pList->tail = pNode->prev;
  else
    pNode->next->prev = pNode->prev;

  pList->count--;
}

/**************************************************************
* listCount - Get number of elements in list
*
* RETURNS: Number of list elements
**************************************************************/

int listCount(LIST *pList)
{
  return(pList->count);
}

