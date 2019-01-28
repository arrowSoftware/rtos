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

/* memShow.c - Show memory partition info */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <rtos.h>
#include <util/classLib.h>
#include <rtos/taskLib.h>
#include <rtos/semLib.h>
#include <rtos/memPartLib.h>
#include <rtos/private/memPartLibP.h>
#include <rtos/memLib.h>
#include <rtos/memShow.h>

/* Locals */
LOCAL size_t memPartAvailable(PART_ID partitionId,
			      size_t *largestBlock,
			      BOOL printEach);

/*******************************************************************************
 * memShowInit - Initialize memory show routines
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS memShowInit(void)
{
  return classShowConnect(memPartClassId, (FUNCPTR) memPartShow);
}

/*******************************************************************************
 * memPartShow - Show memory partition statistics
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS memPartShow(PART_ID partitionId, int type)
{
  unsigned numBlocks, totalBytes, largestWords;

  /* Setup locals */
  totalBytes = 0;
  largestWords = 0;

  /* Verify object */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK)
    return ERROR;

  /* Print free list header if type is one */
  if (type == 1) {

    printf("\nFREE LIST:\n");
    printf("   num    addr       size\n");
    printf("  ---- ---------- ----------\n");

  }

  semTake(&partitionId->sem, WAIT_FOREVER);

  /* Get total bytes and print all blocks if requested */
  totalBytes = memPartAvailable(partitionId, &largestWords, type);
  if (totalBytes == ERROR) {
    semGive(&partitionId->sem);
    return ERROR;
  }

  /* Convert to word size */
  largestWords /= 2;

  /* If type is one add space */
  if (type == 1)
    printf("\n\n");

  /* Get number if block in the free list */
  numBlocks = dllCount(&partitionId->freeList);

  /* If type is one add heading info */
  if (type == 1)
    printf("SUMMARY:\n");

  /* Print free block statistics */
  printf(" status    bytes     blocks   avg block  max block\n");
  printf(" ------ ---------- --------- ---------- ----------\n");
  printf("current\n");

  if (numBlocks)
    printf("   free %10u %9u %10u %10u\n",
	   totalBytes,
	   numBlocks,
	   totalBytes / numBlocks,
	   2 * largestWords);
  else
    printf("   no free blocks\n");

  /* Print total blocks statistics */
  if (partitionId->currBlocksAlloced)
    printf("  alloc %10u %9u %10u          -\n",
	   2 * partitionId->currWordsAlloced,
	   partitionId->currBlocksAlloced,
	   2 * partitionId->currWordsAlloced / partitionId->currBlocksAlloced);
  else
    printf("   no allocated blocks\n");

  printf("cumulative\n");

  /* Print cumulative blocks statistics */
  if (partitionId->cumBlocksAlloced)
    printf("  alloc %10u %9u %10u          -\n",
	   2 * partitionId->cumWordsAlloced,
	   partitionId->cumBlocksAlloced,
	   2 * partitionId->cumWordsAlloced / partitionId->cumBlocksAlloced);
  else
    printf("   no allocated blocks\n");

  semGive(&partitionId->sem);

  return OK;
}

/*******************************************************************************
 * memPartAvailable - Get available memory in partition
 *
 * RETURNS: Number of bytes lefet in memory
 ******************************************************************************/

LOCAL size_t memPartAvailable(PART_ID partitionId,
			      size_t *largestBlock,
			      BOOL printEach)
{
  size_t totalBytes, largestWords;
  int i;
  BLOCK_HEADER *pHeader;
  DL_NODE *pNode;

  /* Setup locals */
  i = 0;
  totalBytes = 0;
  largestWords = 0;

  /* Verify object */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK)
    return ERROR;

  /* For all nodes in the free list */
  for ( pNode = DLL_HEAD(&partitionId->freeList);
        pNode != NULL;
        pNode = DLL_NEXT(pNode) ) {

    /* Get block header from node */
    pHeader = NODE_TO_HEADER(pNode);

    /* If block is invalid */
    if ( !memPartBlockValidate(partitionId, pHeader, pHeader->free) ) {

      printf("  invalid block at %#x deleted\n", (unsigned) pHeader);

      /* Remove invalid block */
      dllRemove(&partitionId->freeList, HEADER_TO_NODE(pHeader));

      return ERROR;

    } /* End if block is invalid */

    /* Else if block is valid */
    else {

      /* Add block size to total bytes count */
      totalBytes += 2 * pHeader->nWords;

      /* Check if this is the largest block */
      if (pHeader->nWords > largestWords)
        largestWords = pHeader->nWords;

      /* Print block info */
      if (printEach)
        printf("  %4d 0x%08x %10u\n",
	       i++,
	       (unsigned) pHeader,
	       (unsigned) 2 * pHeader->nWords);

    } /* End else if block is valid */

  } /* End for all nodes in the free list */

  /* Store largest block if requested */
  if (largestBlock != NULL)
    *largestBlock = largestWords * 2;

  return totalBytes;
}

