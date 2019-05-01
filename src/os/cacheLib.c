#include <stdlib.h>
#include <rtos.h>
#include <os/cacheLib.h>
#include <util/historyLog.h>

extern STATUS cacheArchLibInit(CACHE_MODE textMode, CACHE_MODE dataMode);

BOOL cacheLibInstalled = FALSE;
CACHE_MODE cacheDataMode = CACHE_DISABLED;
BOOL cacheDataEnabled = FALSE;
BOOL cacheMmuAvailable = FALSE;
FUNCPTR cacheDmaMallocFunc = NULL;
FUNCPTR cacheDmaFreeFunc = NULL;

/*******************************************************************************
 * cacheLibInit - Initialize cache library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheLibInit(CACHE_MODE textMode, CACHE_MODE dataMode)
{
    historyLogStr((void *)cacheLibInit, "cacheLibInit", "ENTRY", 0);

    /* If installed */
    if (cacheLibInstalled)
    {
        historyLogStr((void *)cacheLibInit, "cacheLibInit", "cacheLib already installed", 0);
        return OK;
    }

    /* Install arch cache library */
    if (cacheArchLibInit(textMode, dataMode) != OK)
    {
        historyLogStr((void *)cacheLibInit, "cacheLibInit", "Failed to initialize cacheArchLib", 0);
        return ERROR;
    }

    /* Mark as installed */
    cacheLibInstalled = TRUE;

    historyLogStr((void *)cacheLibInit, "cacheLibInit", "Exit", 0);

    return OK;
}

/*******************************************************************************
 * cacheEnable - Enable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheEnable(CACHE_TYPE cache)
{
    historyLogStr((void*)cacheEnable, "cacheEnable", "Entry %d", cache);
    return CACHE_ENABLE(cache);
}

/*******************************************************************************
 * cacheDisable - Disable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheDisable(CACHE_TYPE cache)
{
    historyLogStr((void*)cacheDisable, "cacheDisable", "Entry %d", cache);
    return CACHE_DISABLE(cache);
}

/*******************************************************************************
 * cacheLock - Lock cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheLock(CACHE_TYPE cache, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheLock, "cacheLock", "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_LOCK(cache, addr, bytes);
}

/*******************************************************************************
 * cacheUnlock - Unlock cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheUnlock(CACHE_TYPE cache, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheUnlock, "cacheUnlock", "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_UNLOCK(cache, addr, bytes);
}

/*******************************************************************************
 * cacheFlush - Flush cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheFlush(CACHE_TYPE cache, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheFlush, "cacheFlush", "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_FLUSH(cache, addr, bytes);
}

/*******************************************************************************
 * cacheInvalidate - Invalidate cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheInvalidate(CACHE_TYPE cache, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheInvalidate, "cacheInvalidate", "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_INVALIDATE(cache, addr, bytes);
}

/*******************************************************************************
 * cacheClear - Clear cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheClear(CACHE_TYPE cache, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheClear, "cacheClear", "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_CLEAR(cache, addr, bytes);
}

/*******************************************************************************
 * cacheTextUpdate - Text segment update cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheTextUpdate(void *addr, size_t bytes)
{
    historyLogStr((void*)cacheTextUpdate, "cacheTextUpdate", "Entry 0x%x, %d", addr, bytes);
    return CACHE_TEXT_UPDATE(addr, bytes);
}

/*******************************************************************************
 * cachePipeFlush - Flush pipe cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cachePipeFlush(void)
{
    historyLogStr((void*)cachePipeFlush, "cachePipeFlush", "Entry");
    return CACHE_PIPE_FLUSH();
}

/*******************************************************************************
 * cacheDrvFlush - Flush cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDrvFlush(CACHE_FUNCS *pFunc, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheDrvFlush, "cacheDrvFlush", "Entry 0x%x, 0x%x, %d", pFunc, addr, bytes);
    return CACHE_DRV_FLUSH(pFunc, addr, bytes);
}

/*******************************************************************************
 * cacheDrvInvalidate - Invalidate cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDrvInvalidate(CACHE_FUNCS *pFunc, void *addr, size_t bytes)
{
    historyLogStr((void*)cacheDrvInvalidate, "cacheDrvInvalidate", "Entry 0x%x, 0x%x, %d", pFunc, addr, bytes);
    return CACHE_DRV_INVALIDATE(pFunc, addr, bytes);
}

/*******************************************************************************
 * cacheDrvVirtyToPhys - Virtual to physical memory
 *
 * RETURNS: Physical address
 ******************************************************************************/
void* cacheDrvVirtToPhys(CACHE_FUNCS *pFunc, void *addr)
{
    historyLogStr((void*)cacheDrvVirtToPhys, "cacheDrvVirtToPhys", "Entry 0x%x, 0x%x", pFunc, addr);
    return CACHE_DRV_VIRT_TO_PHYS(pFunc, addr);
}

/*******************************************************************************
 * cacheDrvPhysToVirt - Physical to virtual memory
 *
 * RETURNS: Virtual address
 ******************************************************************************/
void* cacheDrvPhysToVirt(CACHE_FUNCS *pFunc, void *addr)
{
    historyLogStr((void*)cacheDrvPhysToVirt, "cacheDrvPhysToVirt", "Entry 0x%x, 0x%x", pFunc, addr);
    return CACHE_DRV_PHYS_TO_VIRT(pFunc, addr);
}

/*******************************************************************************
 * cacheDrvIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDrvIsWriteCoherent(CACHE_FUNCS *pFunc)
{
    historyLogStr((void*)cacheDrvIsWriteCoherent, "cacheDrvIsWriteCoherent", "Entry 0x%x", pFunc);
    return CACHE_DRV_IS_WRITE_COHERENT(pFunc);
}

/*******************************************************************************
 * cacheDrvIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDrvIsReadCoherent(CACHE_FUNCS *pFunc)
{
    historyLogStr((void*)cacheDrvIsReadCoherent, "cacheDrvIsReadCoherent", "Entry 0x%x", pFunc);
    return CACHE_DRV_IS_READ_COHERENT(pFunc);
}

/*******************************************************************************
 * cacheDmaFlush - Flush dma cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDmaFlush(void *addr, size_t bytes)
{
    historyLogStr((void*)cacheDmaFlush, "cacheDmaFlush", "Entry 0x%x, %d", addr, bytes);
    return CACHE_DMA_FLUSH(addr, bytes);
}

/*******************************************************************************
 * cacheDmaInvalidate - Invalidate dma cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDmaInvalidate(void *addr, size_t bytes)
{
    historyLogStr((void*)cacheDmaInvalidate, "cacheDmaInvalidate", "Entry 0x%x, %d", addr, bytes);
    return CACHE_DMA_INVALIDATE(addr, bytes);
}

/*******************************************************************************
 * cacheDmaVirtyToPhys - Virtual to physical memory
 *
 * RETURNS: Physical address
 ******************************************************************************/
void* cacheDmaVirtToPhys(void *addr)
{
    historyLogStr((void*)cacheDmaVirtToPhys, "cacheDmaVirtToPhys", "Entry 0x%x", addr);
    return CACHE_DMA_VIRT_TO_PHYS(addr);
}

/*******************************************************************************
 * cacheDmaPhysToVirt - Physical to virtual memory
 *
 * RETURNS: Virtual address
 ******************************************************************************/
void* cacheDmaPhysToVirt(void *addr)
{
    historyLogStr((void*)cacheDmaPhysToVirt, "cacheDmaPhysToVirt", "Entry 0x%x",addr);
    return CACHE_DMA_PHYS_TO_VIRT(addr);
}

/*******************************************************************************
 * cacheDmaIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDmaIsWriteCoherent(void)
{
    historyLogStr((void*)cacheDmaIsWriteCoherent, "cacheDmaIsWriteCoherent", "Entry");
    return CACHE_DMA_IS_WRITE_COHERENT();
}

/*******************************************************************************
 * cacheDmaIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDmaIsReadCoherent(void)
{
    historyLogStr((void*)cacheDmaIsReadCoherent, "cacheDmaIsReadCoherent", "Entry");
    return CACHE_DMA_IS_READ_COHERENT();
}

/*******************************************************************************
 * cacheUsrFlush - Flush user cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheUsrFlush(void *addr, size_t bytes)
{
    historyLogStr((void*)cacheUsrFlush, "cacheUsrFlush", "Entry 0x%x, %d", addr, bytes);
    return CACHE_USR_FLUSH(addr, bytes);
}

/*******************************************************************************
 * cacheUsrInvalidate - Invalidate user cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheUsrInvalidate(void *addr, size_t bytes)
{
    historyLogStr((void*)cacheUsrInvalidate, "cacheUsrInvalidate", "Entry 0x%x, %d", addr, bytes);
    return CACHE_USR_INVALIDATE(addr, bytes);
}

/*******************************************************************************
 * cacheUsrIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheUsrIsWriteCoherent(void)
{
    historyLogStr((void*)cacheUsrIsWriteCoherent, "cacheUsrIsWriteCoherent", "Entry");
    return CACHE_USR_IS_WRITE_COHERENT();
}

/*******************************************************************************
 * cacheUsrIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheUsrIsReadCoherent(void)
{
    historyLogStr((void*)cacheUsrIsReadCoherent, "cacheUsrIsReadCoherent", "Entry");
    return CACHE_USR_IS_READ_COHERENT();
}

/*******************************************************************************
 * cacheFuncsSet - Set cache functions
 *
 * RETURNS: N/A
 ******************************************************************************/
void cacheFuncsSet(void)
{
    historyLogStr((void*)cacheFuncsSet, "cacheFuncsSet", "Entry");

    /* If no cache or fully coherent cache */
    if ( (cacheDataEnabled == FALSE) || (cacheDataMode & CACHE_SNOOP_ENABLE) )
    {
        cacheUsrFunc = cacheNullFunc;
        cacheDmaFunc = cacheNullFunc;
        cacheDmaMallocFunc = (FUNCPTR)NULL;
        cacheDmaFreeFunc = (FUNCPTR)NULL;
    }
    else
    {
        cacheUsrFunc.invalidateFunc = cacheLib.invalidateFunc;
        cacheUsrFunc.flushFunc = cacheLib.flushFunc;

        /* If mmu is avaliable */
        if (cacheMmuAvailable)
        {
            cacheDmaFunc.flushFunc = (FUNCPTR) NULL;
            cacheDmaFunc.invalidateFunc = (FUNCPTR) NULL;
            cacheDmaFunc.virtToPhysFunc = cacheLib.dmaVirtToPhysFunc;
            cacheDmaFunc.physToVirtFunc = cacheLib.dmaPhysToVirtFunc;
            cacheDmaMallocFunc = cacheLib.dmaMallocFunc;
            cacheDmaFreeFunc = cacheLib.dmaFreeFunc;
        }
        else
        {
            cacheDmaFunc = cacheUsrFunc;
            cacheDmaMallocFunc = (FUNCPTR) NULL;
            cacheDmaFreeFunc = (FUNCPTR) NULL;
        }
    }
    historyLogStr((void*)cacheFuncsSet, "cacheFuncsSet", "Exit");
}
