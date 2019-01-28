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

/* symLibP.h - Symbol table library private header */

#ifndef _symLibP_h
#define _symLibP_h

#include <util/classLib.h>
#include <util/hashLib.h>
#include <rtos/semLib.h>
#include <rtos/memPartLib.h>
#include <os/private/symbolP.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct symtab {
  OBJ_CORE objCore;			/* Object class */
  HASH_ID nameHashId;			/* Hash table for symbol names */
  SEMAPHORE symMutex;			/* Symbol syncronization */
  PART_ID partId;			/* Symbol memory partition id */
  BOOL sameNameOk;			/* Allow duplicate symbol names */
  int numSymbols;			/* Number of symbols in table */
} SYMTAB;

typedef struct {
  FUNCPTR func;
  ARG arg;
} SYM_FUNC_DESC;

typedef SYMTAB *SYMTAB_ID;		/* Symbol table id */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _symLibP_h */

