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

/* wdLib.h - Watchdog library header */

#ifndef _wdLib_h
#define _wdLib_h

#include <rtos.h>
#include <rtos/private/wdLibP.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef WDOG *WDOG_ID;

IMPORT STATUS wdLibInit(void);
IMPORT WDOG_ID wdCreate(void);
IMPORT STATUS wdInit(WDOG_ID wdId);
IMPORT STATUS wdDestroy(WDOG_ID wdId, BOOL dealloc);
IMPORT STATUS wdDelete(WDOG_ID wdId);
IMPORT STATUS wdTerminate(WDOG_ID wdId);
IMPORT STATUS wdStart(WDOG_ID wdId, int delay, FUNCPTR func, ARG arg);
IMPORT STATUS wdCancel(WDOG_ID wdId);

IMPORT CLASS_ID wdClassId;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _wdLib_h */

