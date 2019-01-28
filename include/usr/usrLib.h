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

/* usrLib.h - Shell user functions header */

#ifndef _usrLib_h
#define _usrLib_h

#include <rtos.h>
#include <tools/moduleLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

IMPORT void show(int objId, int level);
IMPORT int taskIdFigure(int taskNameOrId);
IMPORT void help(void);
IMPORT void i(int taskNameOrId);
IMPORT void ti(int taskNameOrId);
IMPORT int  sp(FUNCPTR func,
	       ARG arg1,
	       ARG arg2,
	       ARG arg3,
	       ARG arg4,
	       ARG arg5,
	       ARG arg6,
	       ARG arg7,
	       ARG arg8,
	       ARG arg9);
IMPORT void ts(int taskNameOrId);
IMPORT void tr(int taskNameOrId);
IMPORT void td(int taskNameOrId);
IMPORT void d(void *addr, int n, int size);
IMPORT void m(void *addr, int size);
IMPORT int pc(int taskNameOrId);
IMPORT void devs(void);
IMPORT MODULE_ID ld(int symFlags, BOOL noAbort, char *name);
IMPORT void lkup(char *str);
IMPORT void checkStack(int taskNameOrId);
IMPORT void periodRun(int secs,
		      FUNCPTR func,
		      ARG arg1,
		      ARG arg2,
		      ARG arg3,
		      ARG arg4,
		      ARG arg5,
		      ARG arg6,
		      ARG arg7,
		      ARG arg8);
IMPORT int period(int secs,
		  FUNCPTR func,
		  ARG arg1,
		  ARG arg2,
		  ARG arg3,
		  ARG arg4,
		  ARG arg5,
		  ARG arg6,
		  ARG arg7,
		  ARG arg8);
IMPORT void repeatRun(int n,
		      FUNCPTR func,
		      ARG arg1,
		      ARG arg2,
		      ARG arg3,
		      ARG arg4,
		      ARG arg5,
		      ARG arg6,
		      ARG arg7,
		      ARG arg8);
IMPORT int repeat(int n,
		  FUNCPTR func,
		  ARG arg1,
		  ARG arg2,
		  ARG arg3,
		  ARG arg4,
		  ARG arg5,
		  ARG arg6,
		  ARG arg7,
		  ARG arg8);
IMPORT void version(void);
IMPORT void printLogo(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _usrLib_h */

