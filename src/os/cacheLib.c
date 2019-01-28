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

/* cacheLib.c - Cache library */

/* Includes */
#include <stdlib.h>
#include <rtos.h>
#include <os/cacheLib.h>

/* Defines */

/* Imports */
IMPORT STATUS cacheArcLibInit(CACHE_MODE textMode, CACHE_MODE dataMode);

/* Locals */

/* Globals */
BOOL cacheLibInstalled = FALSE;
CACHE_MODE cacheDataMode = CACHE_DISABLED;
BOOL cacheDataEnabled = FALSE;
BOOL cacheMmuAvailable = FALSE;
FUNCPTR cacheDmaMallocFunc = NULL;
FUNCPTR cacheDmaFreeFunc = NULL;

/* Functions */

/*******************************************************************************
 * cacheLibInit - Initalize cache library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheLibInit(CACHE_MODE textMode, CACHE_MODE dataMode)
{
  /* If installed */
  if (cacheLibInstalled)
    return OK;

  /* Install arch cache library */
  if ( cacheArchLibInit(textMode, dataMode) != OK)
    return ERROR;

  /* Mark as installed */
  cacheLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * cacheEnable - Enable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheEnable(CACHE_TYPE cache)
{
  return CACHE_ENABLE(cache);
}

/*******************************************************************************
 * cacheDisable - Disable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheDisable(CACHE_TYPE cache)
{
  return CACHE_DISABLE(cache);
}

/*******************************************************************************
 * cacheLock - Lock cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheLock(CACHE_TYPE cache, void *addr, size_t bytes)
{
  return CACHE_LOCK(cache, addr, bytes);
}

/*******************************************************************************
 * cacheUnlock - Unlock cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheUnlock(CACHE_TYPE cache, void *addr, size_t bytes)
{
  return CACHE_UNLOCK(cache, addr, bytes);
}

/*******************************************************************************
 * cacheFlush - Flush cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheFlush(CACHE_TYPE cache, void *addr, size_t bytes)
{
  return CACHE_FLUSH(cache, addr, bytes);
}

/*******************************************************************************
 * cacheInvalidate - Invalidate cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheInvalidate(CACHE_TYPE cache, void *addr, size_t bytes)
{
  return CACHE_INVALIDATE(cache, addr, bytes);
}

/*******************************************************************************
 * cacheClear - Clear cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS cacheClear(CACHE_TYPE cache, void *addr, size_t bytes)
{
  return CACHE_CLEAR(cache, addr, bytes);
}

/*******************************************************************************
 * cacheTextUpdate - Text segment update cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheTextUpdate(void *addr, size_t bytes)
{
  return CACHE_TEXT_UPDATE(addr, bytes);
}

/*******************************************************************************
 * cachePipeFlush - Flush pipe cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cachePipeFlush(void)
{
  return CACHE_PIPE_FLUSH();
}

/*******************************************************************************
 * cacheDrvFlush - Flush cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheDrvFlush(CACHE_FUNCS *pFunc, void *addr, size_t bytes)
{
  return CACHE_DRV_FLUSH(pFunc, addr, bytes);
}

/*******************************************************************************
 * cacheDrvInvalidate - Invalidate cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheDrvInvalidate(CACHE_FUNCS *pFunc, void *addr, size_t bytes)
{
  return CACHE_DRV_INVALIDATE(pFunc, addr, bytes);
}

/*******************************************************************************
 * cacheDrvVirtyToPhys - Virtual to physical memory
 *
 * RETURNS: Physical address
 ******************************************************************************/

void* cacheDrvVirtToPhys(CACHE_FUNCS *pFunc, void *addr)
{
  return CACHE_DRV_VIRT_TO_PHYS(pFunc, addr);
}

/*******************************************************************************
 * cacheDrvPhysToVirt - Physical to virtual memory
 *
 * RETURNS: Virtual address
 ******************************************************************************/

void* cacheDrvPhysToVirt(CACHE_FUNCS *pFunc, void *addr)
{
  return CACHE_DRV_PHYS_TO_VIRT(pFunc, addr);
}

/*******************************************************************************
 * cacheDrvIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL cacheDrvIsWriteCoherent(CACHE_FUNCS *pFunc)
{
  return CACHE_DRV_IS_WRITE_COHERENT(pFunc);
}

/*******************************************************************************
 * cacheDrvIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL cacheDrvIsReadCoherent(CACHE_FUNCS *pFunc)
{
  return CACHE_DRV_IS_READ_COHERENT(pFunc);
}

/*******************************************************************************
 * cacheDmaFlush - Flush dma cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheDmaFlush(void *addr, size_t bytes)
{
  return CACHE_DMA_FLUSH(addr, bytes);
}

/*******************************************************************************
 * cacheDmaInvalidate - Invalidate dma cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheDmaInvalidate(void *addr, size_t bytes)
{
  return CACHE_DMA_INVALIDATE(addr, bytes);
}

/*******************************************************************************
 * cacheDmaVirtyToPhys - Virtual to physical memory
 *
 * RETURNS: Physical address
 ******************************************************************************/

void* cacheDmaVirtToPhys(void *addr)
{
  return CACHE_DMA_VIRT_TO_PHYS(addr);
}

/*******************************************************************************
 * cacheDmaPhysToVirt - Physical to virtual memory
 *
 * RETURNS: Virtual address
 ******************************************************************************/

void* cacheDmaPhysToVirt(void *addr)
{
  return CACHE_DMA_PHYS_TO_VIRT(addr);
}

/*******************************************************************************
 * cacheDmaIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL cacheDmaIsWriteCoherent(void)
{
  return CACHE_DMA_IS_WRITE_COHERENT();
}

/*******************************************************************************
 * cacheDmaIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL cacheDmaIsReadCoherent(void)
{
  return CACHE_DMA_IS_READ_COHERENT();
}

/*******************************************************************************
 * cacheUsrFlush - Flush user cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheUsrFlush(void *addr, size_t bytes)
{
  return CACHE_USR_FLUSH(addr, bytes);
}

/*******************************************************************************
 * cacheUsrInvalidate - Invalidate user cache
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS cacheUsrInvalidate(void *addr, size_t bytes)
{
  return CACHE_USR_INVALIDATE(addr, bytes);
}

/*******************************************************************************
 * cacheUsrIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL cacheUsrIsWriteCoherent(void)
{
  return CACHE_USR_IS_WRITE_COHERENT();
}

/*******************************************************************************
 * cacheUsrIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL cacheUsrIsReadCoherent(void)
{
  return CACHE_USR_IS_READ_COHERENT();
}

/*******************************************************************************
 * cacheFuncsSet - Set cache functions
 *
 * RETURNS: N/A
 ******************************************************************************/

void cacheFuncsSet(void)
{
  /* If no cache or fully coherent cache */
  if ( (cacheDataEnabled == FALSE) || (cacheDataMode & CACHE_SNOOP_ENABLE) ) {

    cacheUsrFunc = cacheNullFunc;
    cacheDmaFunc = cacheNullFunc;
    cacheDmaMallocFunc = (FUNCPTR) NULL;
    cacheDmaFreeFunc = (FUNCPTR) NULL;

  } /* End if no cache or fully coherent */

  /* Else cache or not fully coherent */
  else {

    cacheUsrFunc.invalidateFunc = cacheLib.invalidateFunc;
    cacheUsrFunc.flushFunc = cacheLib.flushFunc;

    /* If mmu is avaliable */
    if (cacheMmuAvailable) {

      cacheDmaFunc.flushFunc = (FUNCPTR) NULL;
      cacheDmaFunc.invalidateFunc = (FUNCPTR) NULL;
      cacheDmaFunc.virtToPhysFunc = cacheLib.dmaVirtToPhysFunc;
      cacheDmaFunc.physToVirtFunc = cacheLib.dmaPhysToVirtFunc;
      cacheDmaMallocFunc = cacheLib.dmaMallocFunc;
      cacheDmaFreeFunc = cacheLib.dmaFreeFunc;

    } /* End if mmu is avilable */

    /* Else mmu not available */
    else {

      cacheDmaFunc = cacheUsrFunc;
      cacheDmaMallocFunc = (FUNCPTR) NULL;
      cacheDmaFreeFunc = (FUNCPTR) NULL;

    } /* End else mmu not available */

  } /* End else cache or not fully coherent */
}

