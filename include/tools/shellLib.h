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

/* shellLib.h - Shell library header */

#ifndef _shellLib_h
#define _shellLib_h

#include <tools/private/shellLibP.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Functions */
IMPORT STATUS shellLibInit(int stackSize, ARG arg);
IMPORT STATUS shellSpawn(int stackSize, ARG arg);
IMPORT void shellLoginInstall(FUNCPTR func, int value);
IMPORT STATUS shellLogin(int fd);
IMPORT void shellLogoutInstall(FUNCPTR func, int value);
IMPORT STATUS shellLogout(void);
IMPORT BOOL shellLock(BOOL lock);
IMPORT STATUS shell(BOOL interactive);
IMPORT STATUS execute(char *line);

/* Functions */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _shellLib_h */

