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

/* includes */

#include <rtos.h>
#include <stdlib.h>

#include <util/sllLib.h>

/* defines */

/* typedefs */

/***************************************************************************
 *
 * sllInit - initialize a singly linked list
 *
 * RETURNS: N/A
 */

void sllInit
    (
    SL_LIST *  pList    /* list to initialize */
    )
    {
    pList->head = NULL;
    pList->tail = NULL;
    }

/*******************************************************************************
 * sllPutAtHead - Put node at list head
 *
 * RETURNS: N/A
 ******************************************************************************/

void sllPutAtHead(SL_LIST *pList, SL_NODE *pNode)
{
  /* Next element will be head node */
  pNode->next = pList->head;

  /* If this is the first node */
  if (pList->head == NULL) {

    pList->head = pNode;
    pList->tail = pNode;

  } /* End if this is the first node */

  /* Else this is not the first node */
  else {

    pList->head = pNode;

  } /* End else this is not the first node */

}

/*******************************************************************************
 * sllPutAtTail - Put node at tail of list
 *
 * RETURNS: N/A
 ******************************************************************************/

void sllPutAtTail(SL_LIST *pList, SL_NODE *pNode)
{

  /* Set next element as zero */
  pNode->next = NULL;

  /* If this is the first element in the list */
  if (pList->head == NULL) {

    pList->tail = pNode;
    pList->head = pNode;

  } /* End if this is the first element in the list */

  /* Else this is not the first element in the list */
  else {

    pList->tail->next = pNode;
    pList->tail = pNode;

  } /* End else this is not the fist element in the list */

}

/*******************************************************************************
 * sllPrevious - Find previous node in list
 *
 * RETURNS: Pointer to node
 ******************************************************************************/

SL_NODE* sllPrevious(SL_LIST *pList, SL_NODE *pNode)
{
  SL_NODE *pLocalNode;

  /* Get head node in list */
  pLocalNode = pList->head;

  /* If list empty or it head is the sent node */
  if ( (pLocalNode == NULL) || (pLocalNode == pNode) )
    return NULL;

  /* While more nodes in list */
  while (pLocalNode->next != NULL) {

    /* Found node */
    if (pLocalNode->next == pNode)
      return pLocalNode;

    /* Advance */
    pLocalNode = pLocalNode->next;

  } /* End while more elements in list */

  return NULL;
}

/***************************************************************************
 *
 * sllAdd - add a node to the tail of the list
 *
 * This routine adds a node to the tail of the list.  It does not verify that
 * the node was not in the list in the first place.
 *
 * RETURNS: N/A
 */

void sllAdd
    (
    SL_LIST *  pList,    /* list to which to add <pNode> */
    SL_NODE *  pNode     /* node to add to tail of <pList> */
    )
    {
    SL_NODE *  pTail;    /* tail node of <pList> */

    if (pList->head == NULL)
        pList->head = pNode;

    pTail = pList->tail;
    if (pTail != NULL)
        pTail->next = pNode;

    pNode->next = NULL;
    pList->tail = pNode;
    }

/***************************************************************************
 *
 * sllRemove - remove node from the list
 *
 * This routine removes a node from the list.  It does not verify that the
 * node was part of the list in the first place.
 *
 * RETURNS: N/A
 */

void sllRemove
    (
    SL_LIST *  pList,    /* list from which to remove <pNode> */
    SL_NODE *  pPrev,    /* node preceding <pNode> */
    SL_NODE *  pNode     /* node to remove from <pList> */
    )
    {
    SL_NODE *  pNext;    /* item following <pNode> */

    pNext = pNode->next;

    if (pPrev == NULL)             /* special case: <pNode> is @ head */
        pList->head = pNext;
    else                           /* general case:  update [next] ptr */
        pPrev->next = pNext;

    if (pNext == NULL)             /* special case: <pNode> is @ tail */
        pList->tail = pPrev;
    }

/***************************************************************************
 *
 * sllInsert - insert a node after the specified node
 *
 * This routine inserts a node after the specified node.  If NULL is specified,
 * then it inserts it at the head of the list.
 *
 * RETURNS: N/A
 */

void  sllInsert
    (
    SL_LIST *  pList,   /* list to which to insert <pNode> */
    SL_NODE *  pPrev,   /* node preceding <pNode> in <pList> */
    SL_NODE *  pNode    /* node to insert after <pPrev> in <pList> */
    )
    {
    SL_NODE *  pNext;

    if (pPrev == NULL)            /* special case: <pNode> is @ head */
        {
        pNext = pList->head;
        pList->head = pNode;
        }
    else
        {                        /* general case: update <pPrev> [next] ptr */
        pNext = pPrev->next;
        pPrev->next = pNode;
        }

    if (pNext == NULL)           /* special case: <pPrev> is @ tail */
        pList->tail = pNode;

    pNode->next = pNext;         /* update <pNode> [next] and [prev] ptrs */
    }

/***************************************************************************
 *
 * sllGet - get (and remove) the node at the head of the list
 *
 * RETURNS: pointer to node, NULL if list is empty
 */

SL_NODE *  sllGet
    (
    SL_LIST *  pList             /* ptr to list from which to get an item */
    )
    {
    SL_NODE *  pNode;            /* ptr to retrieved item */

    pNode = pList->head;
    if (pNode == NULL)           /* special case: list is empty */
        return (NULL);

    pList->head = pNode->next;    /* general case: list is not empty */
    if (pNode->next == NULL)      /* special case: list had only one item */
        pList->tail = NULL;

    return (pNode);
}

/***************************************************************************
 *
 * sllCount - Report number of nodes in list
 *
 * RETURNS: Number of nodes in list
 */

int sllCount
    (
    SL_LIST * pList
    )
    {
    SL_NODE *  pNode;            /* ptr to list */
    int count = 0;		 /* counter for nodes */

    while (pNode != NULL)
        {
        count++;		 /* increase counter */
        pNode = SLL_NEXT(pNode); /* get next node in list */
        }

    return (count);
    }

/*******************************************************************************
 * sllEach - Called for each node in list
 *
 * RETURNS: Pointer to node where it stopped
 ******************************************************************************/

SL_NODE* sllEach(SL_LIST *pList, FUNCPTR func, ARG arg)
{
  SL_NODE *pCurrNode, *pNextNode;

  /* Get first node in list */
  pCurrNode = SLL_HEAD(pList);

  /* While current node non-null */
  while (pCurrNode != NULL) {

    /* Get next node in list */
    pNextNode = SLL_NEXT(pCurrNode);

    /* Call function on current node */
    if ( ( *func) (pCurrNode, arg) == FALSE )
      break;

    /* Advance */
    pCurrNode = pNextNode;

  } /* End while current node non-null */

  return pCurrNode;

}
