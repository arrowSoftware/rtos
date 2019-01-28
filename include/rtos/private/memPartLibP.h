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

/* memPartLibP.h - Private memory partition library header */

#ifndef _memPartLibP_h
#define _memPartLibP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtos.h>
#include <util/classLib.h>
#include <util/dllLib.h>
#include <util/private/objLibP.h>
#include <rtos/private/semLibP.h>

/* Macros */
#define NEXT_HEADER(pHeader) ((BLOCK_HEADER *)((char *)(pHeader) + (2 * (pHeader)->nWords)))
#define PREV_HEADER(pHeader) ((pHeader)->pPrevHeader)
#define HEADER_TO_BLOCK(pHeader) ((char *)((int)pHeader + sizeof(BLOCK_HEADER)))
#define BLOCK_TO_HEADER(pBlock) ((BLOCK_HEADER *)((int)pBlock - sizeof(BLOCK_HEADER)))
#define HEADER_TO_NODE(pHeader) (&((FREE_BLOCK *)pHeader)->node)
#define NODE_TO_HEADER(pNode) ((BLOCK_HEADER *) ((int)pNode - OFFSET(FREE_BLOCK,node)))

typedef struct mem_part
{
  OBJ_CORE	objCore;
  DL_LIST	freeList;
  SEMAPHORE     sem;
  unsigned	totalWords;
  unsigned	minBlockWords;
  unsigned	alignment;
  unsigned	options;
  unsigned	currBlocksAlloced;
  unsigned	currWordsAlloced;
  unsigned	cumBlocksAlloced;
  unsigned	cumWordsAlloced;
} PARTITION;

typedef struct blockHeader
{
  struct blockHeader	*pPrevHeader;
  unsigned		nWords	: 31;
  unsigned		free	: 1;
} BLOCK_HEADER;

typedef struct
{
  struct
  {
    struct blockHeader	*pPrevHeader;
    unsigned		nWords	: 31;
    unsigned		free	: 1;
  } header;

  DL_NODE		node;

} FREE_BLOCK;

extern BOOL memPartBlockValidate(PART_ID partitionId,
				 BLOCK_HEADER *pHeader,
				 BOOL isFree);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _memPartLibP_h */

