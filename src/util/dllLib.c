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

#include <util/dllLib.h>

/* defines */

/* typedefs */

/***************************************************************************
 *
 * dllInit - initialize a doubly linked list
 *
 * RETURNS: N/A
 */

void dllInit
    (
    DL_LIST *  pList    /* list to initialize */
    )
    {
    pList->head = NULL;
    pList->tail = NULL;
    }

/***************************************************************************
 *
 * dllAdd - add a node to the tail of the list
 *
 * This routine adds a node to the tail of the list.  It does not verify that
 * the node was not in the list in the first place.
 *
 * RETURNS: N/A
 */

void dllAdd
    (
    DL_LIST *  pList,    /* list to which to add <pNode> */
    DL_NODE *  pNode     /* node to add to tail of <pList> */
    )
    {
    DL_NODE *  pTail;    /* tail node of <pList> */

    if (pList->head == NULL)
        pList->head = pNode;

    pTail = pList->tail;
    if (pTail != NULL)
        pTail->next = pNode;

    pNode->prev = pTail;
    pNode->next = NULL;
    pList->tail = pNode;
    }

/***************************************************************************
 *
 * dllRemove - remove node from the list
 *
 * This routine removes a node from the list.  It does not verify that the
 * node was part of the list in the first place.
 *
 * RETURNS: N/A
 */

void dllRemove
    (
    DL_LIST *  pList,    /* list from which to remove <pNode> */
    DL_NODE *  pNode     /* node to remove from <pList> */
    )
    {
    DL_NODE *  pPrev;    /* item preceding <pNode> */
    DL_NODE *  pNext;    /* item following <pNode> */

    pPrev = pNode->prev;
    pNext = pNode->next;

    if (pPrev == NULL)             /* special case: <pNode> is @ head */
        pList->head = pNext;
    else                           /* general case:  update [next] ptr */
        pPrev->next = pNext;

    if (pNext == NULL)             /* special case: <pNode> is @ tail */
        pList->tail = pPrev;
    else                           /* general case: update [prev] ptr */
        pNext->prev = pPrev;
    }

/***************************************************************************
 *
 * dllInsert - insert a node after the specified node
 *
 * This routine inserts a node after the specified node.  If NULL is specified,
 * then it inserts it at the head of the list.
 *
 * RETURNS: N/A
 */

void  dllInsert
    (
    DL_LIST *  pList,   /* list to which to insert <pNode> */
    DL_NODE *  pPrev,   /* node preceding <pNode> in <pList> */
    DL_NODE *  pNode    /* node to insert after <pPrev> in <pList> */
    )
    {
    DL_NODE *  pNext;

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

    if (pNext == NULL)           /* special case: <pPrev> is @ pTail */
        {
        pNode->prev = pList->tail;
        pList->tail = pNode;
        }
    else                         /* general case: update <pNext> [prev] ptr */
        pNext->prev = pNode;

    pNode->next = pNext;         /* update <pNode> [next] and [prev] ptrs */
    pNode->prev = pPrev;
    }

/***************************************************************************
 *
 * dllGet - get (and remove) the node at the head of the list
 *
 * RETURNS: pointer to node, NULL if list is empty
 */

DL_NODE *  dllGet
    (
    DL_LIST *  pList             /* ptr to list from which to get an item */
    )
    {
    DL_NODE *  pNode;            /* ptr to retrieved item */

    pNode = pList->head;
    if (pNode == NULL)           /* special case: list is empty */
        return (NULL);

    pList->head = pNode->next;    /* general case: list is not empty */
    if (pNode->next == NULL)      /* special case: list had only one item */
        pList->tail = NULL;
    else                          /* general case: list had items */
        pNode->next->prev = NULL;

    return (pNode);
}

/***************************************************************************
 *
 * dllCount - Report number of nodes in list
 *
 * RETURNS: Number of nodes in list
 */

int  dllCount
    (
    DL_LIST *  pList             /* ptr to list from which to get an item */
    )
    {
    DL_NODE *  pNode;            /* ptr to current node in list */
    int count = 0;		 /* node counter */

    pNode = DLL_HEAD(pList);     /* get first node in list */

    while (pNode != NULL)
        {
        count++;		 /* increase node counter */
        pNode = DLL_NEXT(pNode); /* get next node in list */
        }

    return (count);
    }

