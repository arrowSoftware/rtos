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

/* pathLib.h - Path name functions */

#ifndef _pathLib_h
#define _pathLib_h

#include <rtos.h>
#include <sys/types.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

IMPORT STATUS pathLibInit(void);
IMPORT STATUS ioDefPathSet(char *path);
IMPORT void ioDefPathGet(char *path);
IMPORT STATUS ioDefPathCat(char *path);
IMPORT int pathCwdLen(void);
IMPORT STATUS pathPrependCwd(char *filename, char *fullPath);
IMPORT void pathSplit(char *path, char *dirname, char *filename);
IMPORT void pathCondense(char *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _pathLib_h */

