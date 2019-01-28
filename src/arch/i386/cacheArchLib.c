/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2010 Surplus Users Ham Society
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

/* cacheArchLib.c - Architecture dependent cache library */

/* Includes */
#include <stdlib.h>
#include <rtos.h>
#include <arch/regs.h>
#include <rtos/memPartLib.h>
#include <rtos/memLib.h>
#include <os/vmLib.h>
#include <os/cacheLib.h>

/* Defines */

/* Imports */
IMPORT CACHE_MODE cacheDataMode;
IMPORT BOOL cacheDataEnabled;
IMPORT BOOL cacheMmuAvailable;

IMPORT STATUS cacheI386Enable(void);
IMPORT STATUS cacheI386Disable(void);
IMPORT STATUS cacheI386Lock(void);
IMPORT STATUS cacheI386Unlock(void);
IMPORT STATUS cacheI386Clear(void);
IMPORT STATUS cacheI386Flush(void);
IMPORT STATUS cacheI386Reset(void);

/* Locals */
LOCAL BOOL cacheArchLibInstalled = FALSE;

LOCAL STATUS cacheArchEnable(CACHE_TYPE cache);
LOCAL STATUS cacheArchDisable(CACHE_TYPE cache);
LOCAL STATUS cacheArchLock(CACHE_TYPE cache, void *addr, size_t bytes);
LOCAL STATUS cacheArchUnlock(CACHE_TYPE cache, void *addr, size_t bytes);
LOCAL STATUS cacheArchClear(CACHE_TYPE cache, void *addr, size_t bytes);
LOCAL STATUS cacheArchFlush(CACHE_TYPE cache, void *addr, size_t bytes);
LOCAL void* cacheArchDmaMalloc(size_t bytes);
LOCAL STATUS cacheArchDmaFree(void *buf);
LOCAL void* cacheArchDmaMallocSnoop(size_t bytes);
LOCAL STATUS cacheArchDmaFreeSnoop(void *buf);

/* Globals */

/* Functions */

/*******************************************************************************
 * cacheArchInit - Initalize architecture dependent cache library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheArchLibInit(CACHE_MODE textMode, CACHE_MODE dataMode)
{
  /* Check if already installed */
  if (cacheArchLibInstalled)
    return OK;

  /* Install cache functions */
  cacheLib.enableFunc = (FUNCPTR) cacheArchEnable;
  cacheLib.disableFunc = (FUNCPTR) cacheArchDisable;

  /* Install text update function */
  cacheLib.textUpdateFunc = (FUNCPTR) NULL;
  cacheLib.pipeFlushFunc = (FUNCPTR) NULL;

  /* Install dma functions */
  cacheLib.dmaVirtToPhysFunc = (FUNCPTR) NULL;
  cacheLib.dmaPhysToVirtFunc = (FUNCPTR) NULL;

  /* If snoop enabled */
  if (dataMode & CACHE_SNOOP_ENABLE) {

    /* Install cache functions */
    cacheLib.lockFunc = (FUNCPTR) NULL;
    cacheLib.unlockFunc = (FUNCPTR) NULL;
    cacheLib.clearFunc = (FUNCPTR) NULL;
    cacheLib.flushFunc = (FUNCPTR) NULL;
    cacheLib.invalidateFunc = (FUNCPTR) NULL;

    /* Install dma functions */
    cacheLib.dmaMallocFunc = (FUNCPTR) cacheArchDmaMallocSnoop;
    cacheLib.dmaFreeFunc = (FUNCPTR) cacheArchDmaFreeSnoop;

  } /* End if snoop enabled */

  /* Else snoop not enabled */
  else {

    /* Install cache functions */
    cacheLib.lockFunc = (FUNCPTR) cacheArchLock;
    cacheLib.unlockFunc = (FUNCPTR) cacheArchUnlock;
    cacheLib.clearFunc = (FUNCPTR) cacheArchClear;
    cacheLib.flushFunc = (FUNCPTR) cacheArchFlush;
    cacheLib.invalidateFunc = (FUNCPTR) cacheArchClear;

    /* Install dma functions */
    cacheLib.dmaMallocFunc = (FUNCPTR) cacheArchDmaMalloc;
    cacheLib.dmaFreeFunc = (FUNCPTR) cacheArchDmaFree;

  } /* End if snopp not enabled */

  /* If error in parameters */
  if ( (textMode & CACHE_WRITEALLOCATE) ||
       (dataMode & CACHE_WRITEALLOCATE) ||
       (textMode & CACHE_NO_WRITEALLOCATE) ||
       (dataMode & CACHE_NO_WRITEALLOCATE) ||
       (textMode & CACHE_BURST_ENABLE) ||
       (dataMode & CACHE_BURST_ENABLE) ||
       (textMode & CACHE_BURST_DISABLE) ||
       (dataMode & CACHE_BURST_DISABLE) )
    return ERROR;

  /* Reset cache */
  cacheI386Reset();

  /* Initailize globals */
  cacheDataMode = dataMode;
  cacheDataEnabled = FALSE;
  cacheMmuAvailable = FALSE;

  /* Mark as installed */
  cacheArchLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * cacheArchEnable - Enable cache
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS cacheArchEnable(CACHE_TYPE cache)
{
  /* Call low-level cache enable */
  cacheI386Enable();

  /* If data cache */
  if (cache == DATA_CACHE) {

    /* Enable data cache */
    cacheDataEnabled = TRUE;
    cacheFuncsSet();

  } /* End if data cache */

  return OK;
}

/*******************************************************************************
 * cacheArchDisable - Disable cache
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS cacheArchDisable(CACHE_TYPE cache)
{
  /* Call low-level cache disable */
  cacheI386Disable();

  /* If data cache */
  if (cache == DATA_CACHE) {

    /* Enable data cache */
    cacheDataEnabled = FALSE;
    cacheFuncsSet();

  } /* End if data cache */

  return OK;
}

/*******************************************************************************
 * cacheArchLock - Lock cache
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS cacheArchLock(CACHE_TYPE cache, void *addr, size_t bytes)
{

  /* Call low-level cache lock */
  cacheI386Lock();

  return OK;
}

/*******************************************************************************
 * cacheArchUnlock - Unlock cache
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS cacheArchUnlock(CACHE_TYPE cache, void *addr, size_t bytes)
{

  /* Call low-level cache unlock */
  cacheI386Unlock();

  return OK;
}

/*******************************************************************************
 * cacheArchClear - Clear cache
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS cacheArchClear(CACHE_TYPE cache, void *addr, size_t bytes)
{
  /* Call low-level cache clear */
  cacheI386Clear();

  return OK;
}

/*******************************************************************************
 * cacheArchFlush - Unlock cache
 *
 * RETURNS: OK
 ******************************************************************************/

LOCAL STATUS cacheArchFlush(CACHE_TYPE cache, void *addr, size_t bytes)
{
  /* Call low-level cache flush */
  cacheI386Flush();

  return OK;
}

/*******************************************************************************
 * cacheArchDmaMalloc - Allocate dma memory
 *
 * RETURNS: Pointer to memory or NULL
 ******************************************************************************/

LOCAL void* cacheArchDmaMalloc(size_t bytes)
{
  void *buf;
  int pageSize;

  /* Get page size */
  pageSize = VM_PAGE_SIZE_GET();
  if (pageSize == ERROR)
    return NULL;

  /* Round up memory to page size */
  bytes = ROUND_UP(bytes, pageSize);

  /* Allocate memory */
  buf = valloc(bytes);
  if (buf == NULL)
    return NULL;

  /* If vm library installed */
  if (vmLibInfo.vmLibInstalled) {

    VM_STATE_SET(NULL, buf, bytes,
	         VM_STATE_MASK_CACHEABLE, VM_STATE_NOT_CACHEABLE);

  } /* End if vm library installed */

  return buf;
}

/*******************************************************************************
 * cacheArchDmaFree - Free allocated dma memory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS cacheArchDmaFree(void *buf)
{
  BLOCK_HEADER *pHeader;

  /* If vm library installed */
  if (vmLibInfo.vmLibInstalled) {

    pHeader = BLOCK_TO_HEADER((char *) buf);
    VM_STATE_SET(NULL, buf, (pHeader->nWords * 2) - sizeof(BLOCK_HEADER),
	         VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE);

  } /* End if vm library installed */

  /* Free memory */
  free(buf);

  return OK;
}

/*******************************************************************************
 * cacheArchDmaMallocSnoop - Allocate cache line aligen buffer
 *
 * RETURNS: Pointer to memory or NULL
 ******************************************************************************/

LOCAL void* cacheArchDmaMallocSnoop(size_t bytes)
{
  void *buf;

  /* Align size */
  bytes = ROUND_UP(bytes, _CACHE_ALIGN_SIZE);

  /* Allocate memory */
  buf = memalign(_CACHE_ALIGN_SIZE, bytes);
  if (buf == NULL)
    return NULL;

  return buf;
}

/*******************************************************************************
 * cacheArchDmaFreeSnoop - Free cache line aligned allocated memory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS cacheArchDmaFreeSnoop(void *buf)
{
  free(buf);

  return OK;
}
