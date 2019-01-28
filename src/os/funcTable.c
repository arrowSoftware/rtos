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

/* funcTable.h - Global function table */

#include <stdlib.h>
#include <rtos.h>
#include <os/vmLib.h>
#include <os/cacheLib.h>

/* excArchLib.c */
FUNCPTR		_func_excBaseHook		=	NULL;
FUNCPTR		_func_excIntHook		=	NULL;
FUNCPTR		_func_excPanicHook		=	NULL;

/* excShowLib.c */
FUNCPTR		_func_excInfoShow		=	NULL;

/* sigLib.c */
FUNCPTR		_func_sigprocmask		=	NULL;
FUNCPTR		_func_sigTimeoutRecalc		=	NULL;
FUNCPTR		_func_sigExcKill		=	NULL;

/* selectLib.c */
FUNCPTR		_func_selWakeupListInit		=	NULL;
FUNCPTR		_func_selWakeupListTerminate	=	NULL;
FUNCPTR		_func_selWakeupAll		=	NULL;

/* excLib.c */
FUNCPTR		_func_excJobAdd			=	NULL;

/* logLib.c */
FUNCPTR		_func_logMsg			=	NULL;

/* symLib.c */
FUNCPTR		_func_symFindSymbol		=	NULL;
FUNCPTR		_func_symNameGet		=	NULL;
FUNCPTR		_func_symValueGet		=	NULL;
FUNCPTR		_func_symTypeGet		=	NULL;

/* vmLib.c */
VM_LIB_INFO vmLibInfo = {
  FALSE,					/* vmLibInstalled */
  FALSE,					/* protextTextSegs */
  NULL,						/* vmStateSetFunc */
  NULL,						/* vmStateGetFunc */
  NULL,						/* vmEnableFunc */
  NULL,						/* vmPageSizeGetFunc */
  NULL,						/* vmTranslateFunc */
  NULL						/* vmTextProtectFunc */
};

/* cacheLib.c */
CACHE_LIB cacheLib = {
  NULL,						/* cacheEnable */
  NULL,						/* cacheDisable */
  NULL,						/* cacheLock */
  NULL,						/* cacheUnlock */
  NULL,						/* cacheFlush */
  NULL,						/* cacheInvalidate */
  NULL,						/* cacheClear */
  NULL,						/* cacheTextUpdate */
  NULL,						/* cachePipeFlush */
  NULL,						/* dmaMalloc */
  NULL,						/* dmaFree */
  NULL,						/* dmaVirtToPhys */
  NULL						/* dmaPhysToVirt */
};

CACHE_FUNCS cacheNullFunc = {
  NULL,						/* cacheFlush */
  NULL,						/* cacheInvalidate */
  NULL,						/* cacheVirtToPhys */
  NULL						/* cachePhysToVirt */
};

CACHE_FUNCS cacheDmaFunc = {
  NULL,						/* cacheFlush */
  NULL,						/* cacheInvalidate */
  NULL,						/* cacheVirtToPhys */
  NULL						/* cachePhysToVirt */
};

CACHE_FUNCS cacheUsrFunc = {
  NULL,						/* cacheFlush */
  NULL,						/* cacheInvalidate */
  NULL,						/* cacheVirtToPhys */
  NULL						/* cachePhysToVirt */
};

