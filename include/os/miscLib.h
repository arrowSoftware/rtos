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

/* miscLib.h - Misc os routines library header */

#ifndef _miscLib_h
#define _miscLib_h

#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/semLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

IMPORT STATUS miscLibInit();
IMPORT int splnet(void);
IMPORT int splimp(void);
IMPORT void splx(int x);
IMPORT void sleep(SEM_ID semId);
IMPORT void wakeup(SEM_ID semId);
IMPORT void panic(char *msg);
IMPORT void insque(void *node, void *prev);
IMPORT void remque(void *node);
IMPORT void* hashinit(int elements, int type, unsigned long *mask);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _miscLib_h */

