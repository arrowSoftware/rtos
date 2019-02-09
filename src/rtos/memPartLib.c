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

/* memPartLib.c - Memory partitions */

#include <stdio.h>
#include <string.h>
#include <rtos.h>
#include <util/dllLib.h>
#include <rtos/errnoLib.h>
#include <util/classLib.h>
#include <rtos/memPartLib.h>
#include <rtos/private/memPartLibP.h>
#include <rtos/memLib.h>

#ifndef  NO_TASKLIB

#include <rtos/semLib.h>
#include <rtos/rtosLib.h>
#include <rtos/taskLib.h>

#endif   /* NO_TASKLIB */

#include <stdlib.h>

/* Imports */
IMPORT TCB_ID taskIdCurrent;

/* Locals */
LOCAL OBJ_CLASS		memPartClass;
LOCAL BOOL		memPartLibInstalled = FALSE;

LOCAL BLOCK_HEADER *memAlignedBlockSplit(PART_ID partitionId,
					 BLOCK_HEADER *pHeader,
					 unsigned nWords,
					 unsigned minWords,
					 unsigned alignment);
LOCAL void memPartSemInit(PART_ID partitionId);

/* Globals */
PARTITION		memSysPartition;
CLASS_ID		memPartClassId = &memPartClass;
PART_ID			memSysPartId   = &memSysPartition;
unsigned		memDefaultAlignment = _ALLOC_ALIGN_SIZE;

FUNCPTR			memPartBlockErrorFunc = NULL;
FUNCPTR			memPartAllocErrorFunc = NULL;
FUNCPTR			memPartSemInitFunc = (FUNCPTR) memPartSemInit;

unsigned		memPartOptionsDefault = MEM_BLOCK_ERROR_SUSPEND_FLAG |
						MEM_BLOCK_CHECK;

/*******************************************************************************
* memPartLibInit - Initialize memory partition library
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS memPartLibInit(char *pPool, unsigned poolSize)
{

  /* Check if library is already installed */
  if (memPartLibInstalled) {
    return(OK);
  }

  /* Initialize class */
  if (classInit(memPartClassId, sizeof(PARTITION),
		OFFSET(PARTITION, objCore),
		memSysPartId,
		(FUNCPTR) memPartCreate,
		(FUNCPTR) memPartInit,
		(FUNCPTR) memPartDestroy) != OK) {

    /* errno set by classInit() */
    return(ERROR);
  }

  /* Initialize system partition */
  memPartInit(&memSysPartition, pPool, poolSize);

  /* Mark library installed */
  memPartLibInstalled = TRUE;

  return (OK);
}

/*******************************************************************************
* memPartCreate - Create memory partition
*
* RETURNS: Created memory partition
*******************************************************************************/

PART_ID memPartCreate(char *pPool, unsigned poolSize)
{
  PART_ID pPart;

  /* Allocate object */
  pPart = (PART_ID) objAlloc(memPartClassId);

  /* Initialize partition */
  if (pPart == NULL) {

    /* errno set by objAlloc() */
    return(NULL);
  }

  /* Initialize partition */
  memPartInit(pPart, pPool, poolSize);

  return(pPart);
}

/*******************************************************************************
* memPartInit - Initialize memory partition
*
* RETURNS: N/A
*******************************************************************************/

void memPartInit(PART_ID partitionId, char *pPool, unsigned poolSize)
{
  /* Clear descriptor */
  memset((char *)partitionId, 0, sizeof(*partitionId));

  /* Setup options */
  partitionId->options = memPartOptionsDefault;
  partitionId->alignment = memDefaultAlignment;
  partitionId->minBlockWords = sizeof(FREE_BLOCK) >> 1;

#ifndef  NO_TASKLIB

  (* memPartSemInitFunc) (partitionId);

#endif /* NO_TASKLIB */

  /* Initialize free blocks list */
  dllInit(&partitionId->freeList);

  /* Initialize object class core */
  objCoreInit(&partitionId->objCore, memPartClassId);

  /* Add memory pool to partition  */
  memPartAddToPool(partitionId, pPool, poolSize);
}

/*******************************************************************************
* memPartDestroy - Free memory partition
*
* RETURNS: ERROR
*******************************************************************************/

STATUS memPartDestroy(PART_ID partitionId)
{
  errnoSet (S_memPartLib_NOT_IMPLEMENTED);
  return(ERROR);
}

/*******************************************************************************
* memPartAddToPool - Add memory to partition pool
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS memPartAddToPool(PART_ID partitionId, char *pPool, unsigned poolSize)
{
  BLOCK_HEADER *pHeaderStart;
  BLOCK_HEADER *pHeaderMiddle;
  BLOCK_HEADER *pHeaderEnd;
  char *tmp;
  int reducePool;

  /* Verify object class */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Round address to start of memory allocation block */
  tmp = (char *) ROUND_UP(pPool, partitionId->alignment);
  reducePool = tmp - pPool;
  if (poolSize >= reducePool)
    poolSize -= reducePool;
  else
    poolSize = 0;
  pPool = tmp;

  /* Round down size to memory allocation block */
  poolSize = ROUND_DOWN(poolSize, partitionId->alignment);

  /* Check size */
  if ( poolSize <
       ( (sizeof(BLOCK_HEADER) * 3) + (partitionId->minBlockWords * 2) ) ) {

    errnoSet(S_memPartLib_INVALID_NBYTES);
    return (ERROR);
  }

  /* Setup header at start of pool */
  pHeaderStart = (BLOCK_HEADER *) pPool;
  pHeaderStart->pPrevHeader = NULL;
  pHeaderStart->free = FALSE;
  pHeaderStart->nWords = sizeof(BLOCK_HEADER) >> 1;

  /* Setup header for block contaning allocateble memory */
  pHeaderMiddle = NEXT_HEADER(pHeaderStart);
  pHeaderMiddle->pPrevHeader = pHeaderStart;
  pHeaderMiddle->free = TRUE;
  pHeaderMiddle->nWords = ( poolSize - 2 * sizeof(BLOCK_HEADER) ) >> 1;

  /* Setup header at end of pool */
  pHeaderEnd = NEXT_HEADER(pHeaderMiddle);
  pHeaderEnd->pPrevHeader = pHeaderMiddle;
  pHeaderEnd->free = FALSE;
  pHeaderEnd->nWords = sizeof(BLOCK_HEADER) >> 1;

#ifndef  NO_TASKLIB

  semTake(&partitionId->sem, WAIT_FOREVER);

#endif /* NO_TASKLIB */

  /* Insert nodes into free blocks list */
  dllInsert(&partitionId->freeList,
	    (DL_NODE *) NULL,
	    HEADER_TO_NODE(pHeaderMiddle));

  /* Add size to pool */
  partitionId->totalWords += (poolSize >> 1);

#ifndef  NO_TASKLIB

  semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

  return(OK);
}

/*******************************************************************************
* memAlignedBlockSplit - Split a block in the free list
*
* RETURNS: BLOCK_HEADER *pBlock
*******************************************************************************/

LOCAL BLOCK_HEADER *memAlignedBlockSplit(PART_ID partitionId,
					 BLOCK_HEADER *pHeader,
					 unsigned nWords,
					 unsigned minWords,
					 unsigned alignment)
{
  BLOCK_HEADER *pNewHeader, *pNextHeader;
  char *pEndOfBlock, *pNewBlock;
  int blockSize;

  /* Calculate address at end of this block */
  pEndOfBlock = (char *) pHeader + (pHeader->nWords * 2);

  /* Calculate unaligned address at beginning of new block */
  pNewBlock = (char *) ( (unsigned) pEndOfBlock -
		         (nWords - sizeof(BLOCK_HEADER) / 2) * 2 );

  /* Align start address of new block */
  pNewBlock = (char *) ( (unsigned) pNewBlock & ~(alignment - 1) );

  /* Get header to new block */
  pNewHeader = BLOCK_TO_HEADER(pNewBlock);

  /* Recalculate wordsize for new block */
  blockSize = ( (char *) pNewHeader - (char *) pHeader) / 2;

  /* If block size less than minWords */
  if (blockSize < minWords) {

    /* Remove block if new block is actually the same block as the original */
    if (pNewHeader == pHeader)
      dllRemove(&partitionId->freeList, HEADER_TO_NODE(pHeader));
    else
      return (NULL);

  } /* End if block size less than minWords */

  /* Else block size is greater than minWords */
  else {

    /* Setup new blocks previous header pointer and size */
    pNewHeader->pPrevHeader = pHeader;
    pHeader->nWords = blockSize;

  } /* End else block size is greater than minWords */

  /* If left-over space is too small for a block of its own */
  if (  ( (unsigned) pEndOfBlock - (unsigned) pNewHeader - (nWords * 2) ) <
        ( (minWords * 2) )  ) {

    /* Give all memory to the new block */
    pNewHeader->nWords = (pEndOfBlock - pNewBlock + sizeof(BLOCK_HEADER)) / 2;
    pNewHeader->free = TRUE;

    /* Setup next block previous header pointer */
    NEXT_HEADER(pNewHeader)->pPrevHeader = pNewHeader;

  } /* End if left-over space is too small for a block of its own */

  /* Else left-over space is large enough to form a block of its own */
  else {

    /* First set up the newly created block of the requested size */
    pNewHeader->nWords = nWords;
    pNewHeader->free = TRUE;

    /* Add left-over space to an extra block in the free list */
    pNextHeader = NEXT_HEADER(pNewHeader);
    pNextHeader->nWords = ((unsigned) pEndOfBlock - (unsigned) pNextHeader) / 2;
    pNextHeader->pPrevHeader = pNewHeader;
    pNextHeader->free = TRUE;

    /* Add it to the free list */
    dllAdd(&partitionId->freeList, HEADER_TO_NODE(pNextHeader));

    /* Set header to block after this to have its previous pointer point here */
    NEXT_HEADER(pNextHeader)->pPrevHeader = pNextHeader;

  } /* End else left-over space is large enough to for a block of its own */

  return (pNewHeader);
}

/*******************************************************************************
* memPartBlockValidate - Check validity of a block
*
* RETURNS: OK or ERROR
*******************************************************************************/

BOOL memPartBlockValidate(PART_ID partitionId, BLOCK_HEADER *pHeader,
			  BOOL isFree)
{
  BOOL valid;

  /* Start assuming the block is valid */
  valid = TRUE;

#ifndef  NO_TASKLIB

  taskLock();
  semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

  /* Check if block header is aligned */
  if ( !ALIGNED(pHeader, partitionId->alignment) )
    valid = FALSE;

  /* Check if block size is aligned */
  if ( !ALIGNED(2 * pHeader->nWords, partitionId->alignment) )
    valid = FALSE;

  /* Check block word count */
  if (pHeader->nWords > partitionId->totalWords)
    valid = FALSE;

  /* Check if block is actually free */
  if (pHeader->free != isFree)
    valid = FALSE;

  /* Check next header */
  if ( pHeader != PREV_HEADER( NEXT_HEADER(pHeader) ) )
    valid = FALSE;

  /* Check prev header */
  if ( pHeader != NEXT_HEADER( PREV_HEADER(pHeader) ) )
    valid = FALSE;

#ifndef  NO_TASKLIB

  semTake(&partitionId->sem, WAIT_FOREVER);
  taskUnlock();

#endif /* NO_TASKLIB */

  return(valid);
}

/*******************************************************************************
* memPartAlignedAlloc - Allocate aligned memory
*
* RETURNS: void *pMem
*******************************************************************************/

void* memPartAlignedAlloc(PART_ID partitionId,
			  unsigned nBytes,
			  unsigned alignment)
{
  unsigned nWords, nWordsPad;
  DL_NODE *pNode;
  BLOCK_HEADER *pHeader, *pNewHeader;

  /* Verify object */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK)
    return (NULL);

  /* Calculate word size including header and round up */
  nWords = ( ROUND_UP(nBytes + sizeof(BLOCK_HEADER), alignment) ) >> 1;

  /* If for overflow */
  if ( (nWords << 1) < nBytes) {

    if (memPartAllocErrorFunc != NULL)
      (* memPartAllocErrorFunc) (partitionId, nBytes);

    errnoSet(ENOMEM);

  /* If block error suspend flag */
  if (partitionId->options & MEM_ALLOC_ERROR_SUSPEND_FLAG) {

#ifndef NO_TASKLIB

    if ( (taskIdCurrent->options & TASK_OPTIONS_UNBREAKABLE) == 0 )
      taskSuspend(0);

#endif /* NO_TASKLIB */

    } /* End if block error suspend flag */

    return (NULL);

  } /* End if overflow */

  /* Ensure that block size is greater or equal than absolute minimum */
  if (nWords < partitionId->minBlockWords)
    nWords = partitionId->minBlockWords;

#ifndef NO_TASKLIB

  semTake(&partitionId->sem, WAIT_FOREVER);

#endif /* NO_TASKLIB */

  /* Calculate block word size plus aligment extra */
  nWordsPad = nWords + alignment / 2;

  /* Start searching free list at head node */
  pNode = DLL_HEAD(&partitionId->freeList);

  /* Outer-loop until break */
  while (1) {

    /* Inner loop while block node is not null */
    while (pNode != NULL) {

      /* Break inner loop if:
       * - block size is greater than size + pad for alignment
       * - block is aligned and has the correct size
       */

      if ( (NODE_TO_HEADER(pNode)->nWords > nWordsPad) ||
           ( (NODE_TO_HEADER(pNode)->nWords == nWords) &&
	     (ALIGNED(HEADER_TO_BLOCK(NODE_TO_HEADER(pNode)), alignment)) ) )
        break;

      /* Advance to next block node in free list */
      pNode = DLL_NEXT(pNode);

    } /* End inner loop while block node is not null */

    /* We will get here if all the nodes it the free list has been processed */
    if (pNode == NULL) {

#ifndef NO_TASKLIB

      semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

    if (memPartAllocErrorFunc != NULL)
      (* memPartAllocErrorFunc) (partitionId, nBytes);

    errnoSet(ENOMEM);

    /* If block error suspend flag */
    if (partitionId->options & MEM_ALLOC_ERROR_SUSPEND_FLAG) {

#ifndef NO_TASKLIB

      if ( (taskIdCurrent->options & TASK_OPTIONS_UNBREAKABLE) == 0 )
        taskSuspend(0);

#endif /* NO_TASKLIB */

      } /* End if block error suspend flag */

      return (NULL);

    } /* End if whole free list has been processed */

    /* Get header from block node just found and store it */
    pHeader = NODE_TO_HEADER(pNode);

    /* Split block just found, so that the end of the block
     * is greater or equal to the requested size
     */
    pNewHeader = memAlignedBlockSplit(partitionId,
				      pHeader,
				      nWords,
				      partitionId->minBlockWords,
				      alignment);

    /* If block succeded */
    if (pNewHeader != NULL) {

      /* Store header to new block as requested header */
      pHeader = pNewHeader;

      /* Break outer-loop */
      break;

    } /* End if block split succeded */

    /* We are here if block split failed, it could not be split so
       that it has the requested size.
       Keep on doing the same procedure as before.
       - Search for block greater or equal to the correct size
       - Try to split it from the end to the requested size
     */

    /* Get next node on free block list for the next run in the outer-loop */
    pNode = DLL_NEXT(pNode);

  } /* End outer-loop until break */

  /* Finally!
     A free block has been found, split up from the end at the correct size.
   */

  /* Mark it as allocated */
  pHeader->free = FALSE;

  /* Update memory partition statistics */
  partitionId->currBlocksAlloced++;
  partitionId->cumBlocksAlloced++;
  partitionId->currWordsAlloced += pHeader->nWords;
  partitionId->cumWordsAlloced += pHeader->nWords;

#ifndef NO_TASKLIB

  semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

  return ( (void *) HEADER_TO_BLOCK(pHeader) );
}

/*******************************************************************************
* memPartAlloc - Allocate memory
*
* RETURNS: void *pMem
*******************************************************************************/

void *memPartAlloc(PART_ID partitionId, unsigned nBytes)
{
  return(memPartAlignedAlloc(partitionId,
			  	  nBytes,
				  partitionId->alignment));
}

/*******************************************************************************
* malloc - Allocate memory
*
* RETURNS: Pointer to memory
*******************************************************************************/

void* malloc(size_t size)
{
  return memPartAlloc(memSysPartId, size);
}

/*******************************************************************************
* memPartRealloc - Allocate buffer of different size
*
* RETURNS: void *pMem
*******************************************************************************/

void *memPartRealloc(PART_ID partitionId, void *ptr, unsigned nBytes)
{
  BLOCK_HEADER *pHeader = NULL, *pNextHeader = NULL;
  unsigned nWords = 0;
  void *pNewBlock = NULL;

  /* Verify object class */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(NULL);
  }

  /* If block is not allocated, then allocate new and return */
  if (ptr == NULL) {
    return( memPartAlloc(partitionId, nBytes) );
  }

  /* If zero bytes, then just free and return */
  if (nBytes == 0) {
    memPartFree(partitionId, ptr);
    return(NULL);
  }

#ifndef  NO_TASKLIB

  semTake(&partitionId->sem, WAIT_FOREVER);

#endif /* NO_TASKLIB */

  /* Calculate size including header and aligned */
  nWords = ( ROUND_UP(nBytes + sizeof(BLOCK_HEADER),
		      partitionId->alignment) ) >> 1;
  if (nWords < partitionId->minBlockWords)
    nWords = partitionId->minBlockWords;

  /* SHOULD do optional block validity check */

  /* Test of this an increase of size */
  pHeader = BLOCK_TO_HEADER(ptr);
  if (nWords > pHeader->nWords) {

    pNextHeader = NEXT_HEADER(pHeader);

    if (!pNextHeader->free ||
        (pHeader->nWords + pNextHeader->nWords) < nWords) {

#ifndef  NO_TASKLIB

      semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

      if ( (pNewBlock = memPartAlloc(partitionId, nBytes)) == NULL) {
        /* errno set by memPartAlloc() */
        return(NULL);
      }

      /* Copy data */
      memcpy(pNewBlock, ptr,
	     (size_t) (2 * pHeader->nWords - sizeof(BLOCK_HEADER)));

      /* Free old block */
      memPartFree(partitionId, ptr);

      return(pNewBlock);
    } else {
      /* Take next block */
      dllRemove(&partitionId->freeList, HEADER_TO_NODE(pNextHeader));
      pHeader->nWords += pNextHeader->nWords;
      partitionId->currWordsAlloced += pNextHeader->nWords;
      partitionId->cumWordsAlloced  += pNextHeader->nWords;
      NEXT_HEADER(pHeader)->pPrevHeader = pHeader;
    }
  }

  /* Split of overhead and give it back */
  pNextHeader = memAlignedBlockSplit(partitionId,
			             pHeader,
				     pHeader->nWords - nWords,
				     partitionId->minBlockWords,
			             partitionId->alignment);

#ifndef  NO_TASKLIB

  semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

  /* Free leftover block */
  if (pNextHeader != NULL) {
    memPartFree(partitionId, HEADER_TO_BLOCK(pNextHeader));
    partitionId->currBlocksAlloced++;
  }

  return(pNewBlock);
}

/*******************************************************************************
* realloc - Re allocate memory
*
* RETURNS: Pointer to memory
*******************************************************************************/

void* realloc(void *buf, size_t size)
{
  return memPartRealloc(memSysPartId, buf, size);
}

/*******************************************************************************
* memPartFree - Free memory block
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS memPartFree(PART_ID partitionId, void *ptr)
{
  unsigned nWords;
  BLOCK_HEADER *pHeader, *pNextHeader;
  char *pBlock;

  /* Verify object */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK)
    return (ERROR);

  /* Store address as a char pointer */
  pBlock = (char *) ptr;

  /* Check if anything to free */
  if (pBlock == NULL)
    return (OK);

  /* Get header to block to free */
  pHeader = BLOCK_TO_HEADER(pBlock);

#ifndef NO_TASKLIB

  semTake(&partitionId->sem, WAIT_FOREVER);

#endif /* NO_TASKLIB */

  /* If block is invalid */
  if ( (partitionId->options & MEM_BLOCK_CHECK) &&
       !memPartBlockValidate(partitionId, pHeader, FALSE) ) {

#ifndef NO_TASKLIB

    semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

    if (memPartBlockErrorFunc != NULL)
      (*memPartBlockErrorFunc) (partitionId, pBlock, "memPartFree");

    errnoSet(S_memPartLib_BLOCK_ERROR);

    /* If block error suspend flag */
    if (partitionId->options & MEM_BLOCK_ERROR_SUSPEND_FLAG) {

#ifndef NO_TASKLIB

      if ( (taskIdCurrent->options & TASK_OPTIONS_UNBREAKABLE) == 0 )
        taskSuspend(0);

#endif /* NO_TASKLIB */

    } /* End if block error suspend flag */

    return (ERROR);

  } /* End if block is invalid */

  /* Get number of words in block */
  nWords = pHeader->nWords;

  /* If the previous block is free */
  if (PREV_HEADER(pHeader)->free) {

    /* This is block is not considered free, the previous already is */
    pHeader->free = FALSE;

    /* Add this block to the previous block */
    pHeader = PREV_HEADER(pHeader);
    pHeader->nWords += nWords;

  } /* End if block the previous block is free */

  /* Else the previous block is not free */
  else {

    /* Mark block as free */
    pHeader->free = TRUE;

    /* Add block to the the free list */
    dllInsert(&partitionId->freeList,
	       (DL_NODE *) NULL,
	       HEADER_TO_NODE(pHeader));

  } /* End else the previoius block is not free */

  /* Get header to next block */
  pNextHeader = NEXT_HEADER(pHeader);

  /* If the next block is free */
  if (pNextHeader->free) {

    /* Add next block to this block */
    pHeader->nWords += pNextHeader->nWords;

    /* Remove next block from the free list */
    dllRemove(&partitionId->freeList, HEADER_TO_NODE(pNextHeader));

    /* This block has already been marked as free and added
       to the free list in the if-else block above this one.
     */

  } /* End if the next block is free */

  /* Setup the next block previous header pointer */
  NEXT_HEADER(pHeader)->pPrevHeader = pHeader;

  /* Update partition statistics */
  partitionId->currBlocksAlloced--;
  partitionId->currWordsAlloced -= nWords;

#ifndef NO_TASKLIB

  semGive(&partitionId->sem);

#endif /* NO_TASKLIB */

  return (OK);
}

/*******************************************************************************
* free - Free allocated memory
*
* RETURNS: N/A
*******************************************************************************/

void free(void *buf)
{
  memPartFree(memSysPartId, buf);
}

/*******************************************************************************
* memPartSemInit - Initialize memory partition semaphore
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void memPartSemInit(PART_ID partitionId)
{
  semBInit(&partitionId->sem, SEM_Q_PRIORITY, SEM_FULL);
}

