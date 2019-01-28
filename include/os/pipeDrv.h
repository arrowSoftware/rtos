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

/* pipeDrv.h - Pipe device header */

#ifndef _pipeDrv_h
#define _pipeDrv_h

#include <rtos.h>
#include <rtos/msgQLib.h>
#include <io/iosLib.h>
#include <os/selectLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct {
  DEV_HEADER devHeader;
  MSG_Q msgQ;
  SEL_WAKEUP_LIST selWakeupList;
  unsigned int numOpens;
} PIPE_DEV;

IMPORT STATUS pipeDevCreate(char *name, int nMsg, int nBytes);
IMPORT STATUS pipeDevDelete(char *name, BOOL force);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _pipeDrv_h */

