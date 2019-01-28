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

/* memPartLib.h - Memory library header */

#ifndef _memPartLib_h
#define _memPartLib_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mem_part *PART_ID;

#include <tools/moduleNumber.h>

#include <rtos.h>
#include <util/dllLib.h>
#include <rtos/private/memPartLibP.h>

#define S_memPartLib_NOT_INSTALLED       (M_memPartLib | 0x0001)
#define S_memPartLib_INVALID_NBYTES      (M_memPartLib | 0x0002)
#define S_memPartLib_BLOCK_ERROR	 (M_memPartLib | 0x0004)
#define S_memPartLib_NOT_IMPLEMENTED     (M_memPartLib | 0xffff)

extern STATUS memPartLibInit(char *pPool, unsigned poolSize);

extern PART_ID memPartCreate(char *pPool, unsigned poolSize);
extern void memPartInit(PART_ID partitionId, char *pPool, unsigned poolSize);
extern STATUS memPartDestroy(PART_ID partitionId);

extern STATUS memPartAddToPool(PART_ID partitionId, char *pPool,
			       unsigned poolSize);

extern void *memPartAlignedAlloc(PART_ID partitionId, unsigned nBytes,
				 unsigned alignment);
extern void *memPartAlloc(PART_ID partitionId, unsigned nBytes);

extern void *memPartRealloc(PART_ID partitionId, void *ptr,
			    unsigned nBytes);

extern STATUS memPartFree(PART_ID partitionId, void *ptr);

extern PART_ID memSysPartId;
extern CLASS_ID memPartClassId;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _memPartLib_h */

