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

/* qPrioLib.h - Priority queue */

#ifndef _qPrioLib_h
#define _qPrioLib_h

#include <rtos.h>
#include <util/qLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <util/dllLib.h>
#include <util/qLib.h>

/* typedefs */

typedef union {           /* Explicitly show that Q_PRIO_NODE */
    Q_NODE  qNode;        /* overlays Q_NODE. */
    struct {
        DL_NODE  node;
        unsigned   key;
    } qPrio;
} Q_PRIO_NODE;

typedef union {           /* Explicitly show that Q_PRIO_HEAD */
    Q_HEAD  qHead;        /* overlays Q_HEAD. */
    struct {
        DL_LIST  head;
    } qPrio;
} Q_PRIO_HEAD;

extern Q_CLASS_ID qPrioClassId;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _qPrioLib_h */

