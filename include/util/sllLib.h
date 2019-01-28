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

#ifndef __SLL_LIB_H
#define __SLL_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define SLL_NEXT(node)  (((node))->next)

#define SLL_HEAD(list) (((list))->head)
#define SLL_TAIL(list) (((list))->tail)

/* typedefs */

typedef struct sl_node
    {
    struct sl_node *  next;
    } SL_NODE;

typedef struct sl_list
    {
    SL_NODE *  head;
    SL_NODE *  tail;
    } SL_LIST;

extern void sllInit   (SL_LIST *);
extern void sllPutAtHead (SL_LIST *, SL_NODE *);
extern void sllPutAtTail (SL_LIST *, SL_NODE *);
extern SL_NODE * sllPrevious(SL_LIST*, SL_NODE *);
extern void sllAdd    (SL_LIST *, SL_NODE *);
extern void sllRemove (SL_LIST *, SL_NODE *, SL_NODE *);
extern void sllInsert (SL_LIST *, SL_NODE *, SL_NODE *);
extern SL_NODE *  sllGet (SL_LIST *);
extern int sllCount   (SL_LIST *);
extern SL_NODE * sllEach(SL_LIST *, FUNCPTR, ARG);

#ifdef __cplusplus
}
#endif

#endif

