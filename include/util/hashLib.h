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

/* hashLib.h - Hash table library header */

#ifndef _hashLib_h
#define _hashLib_h

#include <util/private/hashLibP.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct {
  HASH_NODE node;			/* Hash node */
  int key;				/* Node key */
  int data;				/* Hash data */
} H_NODE_INT;

typedef struct {
  HASH_NODE node;			/* Hash node */
  char *str;				/* Node key */
  int data;				/* Hash data */
} H_NODE_STRING;

/* Imports */
IMPORT CLASS_ID hashClassId;

/* Functions */
IMPORT STATUS hashLibInit(void);
IMPORT HASH_ID hashTableCreate(int log2Size, FUNCPTR keyCmpFunc,
			       FUNCPTR keyFunc, int keyArg);
IMPORT STATUS hashTableInit(HASH_ID hashId, SL_LIST *pList, int log2Size,
			    FUNCPTR keyCmpFunc, FUNCPTR keyFunc, int keyArg);
IMPORT STATUS hashTableDestroy(HASH_ID hashId, BOOL dealloc);
IMPORT STATUS hashTableTerminate(HASH_ID hashId);
IMPORT STATUS hashTableDelete(HASH_ID hashId);
IMPORT STATUS hashTablePut(HASH_ID hashId, HASH_NODE *pHashNode);
IMPORT HASH_NODE* hashTableFind(HASH_ID hashId, HASH_NODE *pMatchNode,
				int keyCmpArg);
IMPORT HASH_NODE* hashTableEach(HASH_ID hashId, FUNCPTR func, ARG arg);
IMPORT STATUS hashTableRemove(HASH_ID hashId, HASH_NODE *pHashNode);
IMPORT BOOL hashKeyCmp(H_NODE_INT *pMatchNode, H_NODE_INT *pHashNode,
		       int keyCmpArg);
IMPORT BOOL hashKeyStringCmp(H_NODE_STRING *pMatchNode,
			     H_NODE_STRING *pHashNode,
		             int keyCmpArg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _hashLib_h */

