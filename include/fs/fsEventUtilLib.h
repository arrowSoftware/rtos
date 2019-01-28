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

/* fsEventUtilLib.h - file system event utility library */

#ifndef __FS_EVENT_UTIL_LIB_H
#define __FS_EVENT_UTIL_LIB_H

#include <rtos.h>
#include <tools/moduleNumber.h>
#include <rtos/semLib.h>

#define S_fsEventUtil_INVALID_PARAMETER (M_fsEventUtil | 0x0001)

typedef struct {
    SEM_ID  semId;
    char *  path;
} FS_PATH_WAIT_STRUCT;

extern STATUS fsEventUtilInit (void);
extern STATUS fsPathAddedEventSetup (FS_PATH_WAIT_STRUCT *  pWaitData,
                                     char *                 path);
extern void fsPathAddedEventRaise (char *  path);
extern STATUS  fsWaitForPath (FS_PATH_WAIT_STRUCT *  pWaitData); 
#endif
