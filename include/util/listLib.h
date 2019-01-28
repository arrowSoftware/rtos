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

/* listLib.h - Linked list */

#ifndef _listLib_h
#define _listLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */
#define LIST_HEAD(pList) ( ((LIST *)(pList))->head)
#define LIST_TAIL(pList) ( ((LIST *)(pList))->tail)
#define LIST_NEXT(pNode) ( ((LIST_NODE *)(pNode))->next)
#define LIST_PREV(pNode) ( ((LIST_NODE *)(pNode))->prev)
#define LIST_EMPTY(pList) ( (((LIST *)pList)->head == NULL) )

/* A node in the linked list */
typedef struct listnode
{
  struct listnode *next;
  struct listnode *prev;
} LIST_NODE;

/* The list type itself */
typedef struct
{
  LIST_NODE *head;
  LIST_NODE *tail;
  int count;
} LIST;

extern STATUS listInit(LIST *pList);
extern void listInsert(LIST *pList,
		       LIST_NODE *pPrev,
		       LIST_NODE *pNode);
extern void listAdd(LIST *pList, LIST_NODE *pNode);
extern LIST_NODE *listGet(LIST *pList);
extern void listRemove(LIST *pList, LIST_NODE *pNode);
extern int listCount(LIST *pList);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _listLib_h */

