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

/* memLib.c - Memory library */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <util/classLib.h>
#include <rtos/taskLib.h>
#include <rtos/semLib.h>
#include <rtos/memPartLib.h>
#include <os/vmLib.h>
#include <rtos/memLib.h>

/* Imports */
IMPORT PART_ID memSysPartId;
IMPORT unsigned memPartOptionsDefault;
IMPORT FUNCPTR _func_logMsg;
IMPORT FUNCPTR memPartAllocErrorFunc;
IMPORT FUNCPTR memPartBlockErrorFunc;

/* Globals */

/* Locals */
LOCAL BOOL	memLibInstalled = FALSE;
LOCAL char	memMsgBlockTooBig[] =
	"memPartAlloc: block too big - %d in partition.\n";
LOCAL char	memMsgBlockError[] =
	"%s: invalid block %#x in partition %#x.\n";

LOCAL void memPartAllocError(PART_ID partitionId, unsigned nBytes);
LOCAL void memPartBlockError(PART_ID partitionId, char *pBlock, char *label);

/*******************************************************************************
 * memLibInit - Initialize memory library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS memLibInit(void)
{
  /* Check if already installed */
  if (memLibInstalled)
    return OK;

  /* Setup */
  memPartAllocErrorFunc = (FUNCPTR) memPartAllocError;
  memPartBlockErrorFunc = (FUNCPTR) memPartBlockError;
  memPartOptionsDefault = MEM_ALLOC_ERROR_LOG_FLAG |
			  MEM_BLOCK_ERROR_LOG_FLAG |
			  MEM_ALLOC_ERROR_SUSPEND_FLAG |
			  MEM_BLOCK_CHECK;

  /* Mark as installed */
  memLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * memPartOptionsSet - Set memory options
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS memPartOptionsSet(PART_ID partitionId, unsigned options)
{
  if (!memLibInstalled)
    return ERROR;

  /* Verify object */
  if (OBJ_VERIFY(partitionId, memPartClassId) != OK)
    return ERROR;

  semTake(&partitionId->sem, WAIT_FOREVER);

  /* Set options */
  partitionId->options |= options;

  /* If block check needed by option */
  if ( options & (MEM_BLOCK_ERROR_LOG_FLAG | MEM_BLOCK_ERROR_SUSPEND_FLAG) )
    partitionId->options |= MEM_BLOCK_CHECK;

  semGive(&partitionId->sem);

  return OK;
}

/*******************************************************************************
 * memalign - Allocate memory aligned to specified size
 *
 * RETURNS: Pointer to memory or null
 ******************************************************************************/

void* memalign(size_t alignment, size_t size)
{
  return memPartAlignedAlloc(memSysPartId, size, alignment);
}

/*******************************************************************************
 * valloc - Allocate memory that begins at page boundary
 *
 * RETURNS: Pointer to memory or null
 ******************************************************************************/

void* valloc(size_t size)
{
  int pageSize;

  /* Get page size */
  pageSize = VM_PAGE_SIZE_GET();
  if (pageSize == ERROR) {

    errnoSet(S_memLib_PAGE_SIZE_UNAVAILABLE);
    return NULL;

  }

  return memalign(pageSize, size);
}

/*******************************************************************************
 * calloc - Allocate numeber of objects
 *
 * RETURNS: Pointer to memory or null
 ******************************************************************************/

void* calloc(size_t num, size_t size)
{
  void *buf;
  size_t nBytes;

  /* Calculate size */
  nBytes = num * size;

  /* Allocate memory */
  buf = memPartAlloc(memSysPartId, nBytes);
  if (buf == NULL)
    return NULL;

  /* Clear memory */
  memset(buf, 0, nBytes);

  return buf;
}

/*******************************************************************************
 * cfree - Free memory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

int cfree(void *buf)
{
  return memPartFree(memSysPartId, buf);
}

/*******************************************************************************
 * memPartAllocError - Allocation error handler
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void memPartAllocError(PART_ID partitionId, unsigned nBytes)
{
  if ( (_func_logMsg != NULL) && MEM_ALLOC_ERROR_LOG_FLAG)
    (* _func_logMsg) (memMsgBlockTooBig, (ARG) nBytes, (ARG) partitionId,
		      (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);
}

/*******************************************************************************
 * memPartBlockError - Block error handler
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void memPartBlockError(PART_ID partitionId, char *pBlock, char *label)
{
  if ( (_func_logMsg != NULL) && MEM_BLOCK_ERROR_LOG_FLAG)
    (* _func_logMsg) (memMsgBlockError, (ARG) label, (ARG) pBlock,
		      (ARG) partitionId, (ARG) 0, (ARG) 0, (ARG) 0);
}

