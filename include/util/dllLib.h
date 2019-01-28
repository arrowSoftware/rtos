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

#ifndef __DLL_LIB_H
#define __DLL_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define DLL_NEXT(node)  (((node))->next)
#define DLL_PREV(node)  (((node))->prev)

#define DLL_HEAD(list) (((list))->head)
#define DLL_TAIL(list) (((list))->tail)

#define DLL_EMPTY(list)  (((list)->head == NULL))

/* typedefs */

typedef struct dl_node
    {
    struct dl_node *  prev;
    struct dl_node *  next;
    } DL_NODE;

typedef struct dl_list
    {
    DL_NODE *  head;
    DL_NODE *  tail;
    } DL_LIST;

extern void dllInit   (DL_LIST *);
extern void dllAdd    (DL_LIST *, DL_NODE *);
extern void dllRemove (DL_LIST *, DL_NODE *);
extern void dllInsert (DL_LIST *, DL_NODE *, DL_NODE *);
extern DL_NODE *  dllGet (DL_LIST *);
extern int dllCount   (DL_LIST *);

#ifdef __cplusplus
}
#endif

#endif

