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

/* symLib.h - Symbol table library header */

#ifndef _symLib_h
#define _symLib_h

#include <tools/moduleNumber.h>
#define S_symLib_SYMBOL_NOT_FOUND		(M_symLib | 0x0001)

#include <os/private/symLibP.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Imports */
IMPORT CLASS_ID symTableClassId;
IMPORT unsigned short symGroupDefault;

/* Functions */
IMPORT STATUS symLibInit(void);
IMPORT SYMTAB_ID symTableCreate(int log2Size, BOOL sameNameOk, PART_ID partId);
IMPORT STATUS symTableInit(SYMTAB_ID symTalbeId, BOOL sameNameOk,
			   PART_ID partId, HASH_ID nameHashId);
IMPORT STATUS symTableDestroy(SYMTAB_ID symTableId, BOOL dealloc);
IMPORT STATUS symTableDelete(SYMTAB_ID symTableId);
IMPORT STATUS symTableTerminate(SYMTAB_ID symTableId);

IMPORT SYMBOL_ID symCreate(SYMTAB_ID symTableId, char *name, void *value,
			   SYM_TYPE type, unsigned short group);
IMPORT STATUS symInit(SYMBOL_ID symId, char *name, void *value,
		      SYM_TYPE type, unsigned short group);
IMPORT STATUS symDestroy(SYMTAB_ID symTableId, SYMBOL_ID symId);

IMPORT STATUS symAdd(SYMTAB_ID symTableId, char *name, void *value,
	      	     SYM_TYPE type, unsigned short group);
IMPORT STATUS symTableAdd(SYMTAB_ID symTableId, SYMBOL_ID symId);

IMPORT STATUS symFindByName(SYMTAB_ID symTableId, char *name,
			    void **pValue, SYM_TYPE *pType);
IMPORT STATUS symFindByNameAndType(SYMTAB_ID symTableId, char *name,
				   void **pValue, SYM_TYPE *pType,
				   SYM_TYPE sType, SYM_TYPE mask);
IMPORT STATUS symFindSymbol(SYMTAB_ID symTableId, char *name, void *value,
			    SYM_TYPE type, SYM_TYPE mask, SYMBOL_ID *pSymId);

IMPORT STATUS symNameGet(SYMBOL_ID symId, char **pName);
IMPORT STATUS symValueGet(SYMBOL_ID symId, void **pValue);
IMPORT STATUS symTypeGet(SYMBOL_ID symId, SYM_TYPE *pType);

IMPORT SYMBOL* symEach(SYMTAB_ID symTableId, FUNCPTR func, ARG arg);

IMPORT STATUS symRemove(SYMTAB_ID symTableId, char *name, SYM_TYPE type);
IMPORT STATUS symTableRemove(SYMTAB_ID symTableId, SYMBOL_ID symId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _symLib_h */

