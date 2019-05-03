#include <stdlib.h>
#include <rtos.h>
#include <os/cacheLib.h>
#include <util/logging.h>

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
    log_debug((void *)cacheLibInit, "ENTRY", 0);

    /* If installed */
    if (cacheLibInstalled)
    {
        log_debug((void *)cacheLibInit, "cacheLib already installed", 0);
        return OK;
    }

    /* Install arch cache library */
    if (cacheArchLibInit(textMode, dataMode) != OK)
    {
        log_debug((void *)cacheLibInit, "Failed to initialize cacheArchLib", 0);
        return ERROR;
    }

    /* Mark as installed */
    cacheLibInstalled = TRUE;

    log_debug((void *)cacheLibInit, "Exit", 0);

    return OK;
}

/*******************************************************************************
 * cacheEnable - Enable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheEnable(CACHE_TYPE cache)
{
    log_debug(cacheEnable, "Entry %d", cache);
    return CACHE_ENABLE(cache);
}

/*******************************************************************************
 * cacheDisable - Disable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheDisable(CACHE_TYPE cache)
{
    log_debug(cacheDisable, "Entry %d", cache);
    return CACHE_DISABLE(cache);
}

/*******************************************************************************
 * cacheLock - Lock cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheLock(CACHE_TYPE cache, void *addr, size_t bytes)
{
    log_debug(cacheLock, "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_LOCK(cache, addr, bytes);
}

/*******************************************************************************
 * cacheUnlock - Unlock cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheUnlock(CACHE_TYPE cache, void *addr, size_t bytes)
{
    log_debug(cacheUnlock, "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_UNLOCK(cache, addr, bytes);
}

/*******************************************************************************
 * cacheFlush - Flush cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheFlush(CACHE_TYPE cache, void *addr, size_t bytes)
{
    log_debug(cacheFlush, "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_FLUSH(cache, addr, bytes);
}

/*******************************************************************************
 * cacheInvalidate - Invalidate cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheInvalidate(CACHE_TYPE cache, void *addr, size_t bytes)
{
    log_debug(cacheInvalidate, "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_INVALIDATE(cache, addr, bytes);
}

/*******************************************************************************
 * cacheClear - Clear cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/
STATUS cacheClear(CACHE_TYPE cache, void *addr, size_t bytes)
{
    log_debug(cacheClear, "Entry %d, 0x%x, %d", cache, addr, bytes);
    return CACHE_CLEAR(cache, addr, bytes);
}

/*******************************************************************************
 * cacheTextUpdate - Text segment update cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheTextUpdate(void *addr, size_t bytes)
{
    log_debug(cacheTextUpdate, "Entry 0x%x, %d", addr, bytes);
    return CACHE_TEXT_UPDATE(addr, bytes);
}

/*******************************************************************************
 * cachePipeFlush - Flush pipe cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cachePipeFlush(void)
{
    log_debug(cachePipeFlush, "Entry");
    return CACHE_PIPE_FLUSH();
}

/*******************************************************************************
 * cacheDrvFlush - Flush cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDrvFlush(CACHE_FUNCS *pFunc, void *addr, size_t bytes)
{
    log_debug(cacheDrvFlush, "Entry 0x%x, 0x%x, %d", pFunc, addr, bytes);
    return CACHE_DRV_FLUSH(pFunc, addr, bytes);
}

/*******************************************************************************
 * cacheDrvInvalidate - Invalidate cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDrvInvalidate(CACHE_FUNCS *pFunc, void *addr, size_t bytes)
{
    log_debug(cacheDrvInvalidate, "Entry 0x%x, 0x%x, %d", pFunc, addr, bytes);
    return CACHE_DRV_INVALIDATE(pFunc, addr, bytes);
}

/*******************************************************************************
 * cacheDrvVirtyToPhys - Virtual to physical memory
 *
 * RETURNS: Physical address
 ******************************************************************************/
void* cacheDrvVirtToPhys(CACHE_FUNCS *pFunc, void *addr)
{
    log_debug(cacheDrvVirtToPhys, "Entry 0x%x, 0x%x", pFunc, addr);
    return CACHE_DRV_VIRT_TO_PHYS(pFunc, addr);
}

/*******************************************************************************
 * cacheDrvPhysToVirt - Physical to virtual memory
 *
 * RETURNS: Virtual address
 ******************************************************************************/
void* cacheDrvPhysToVirt(CACHE_FUNCS *pFunc, void *addr)
{
    log_debug(cacheDrvPhysToVirt, "Entry 0x%x, 0x%x", pFunc, addr);
    return CACHE_DRV_PHYS_TO_VIRT(pFunc, addr);
}

/*******************************************************************************
 * cacheDrvIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDrvIsWriteCoherent(CACHE_FUNCS *pFunc)
{
    log_debug(cacheDrvIsWriteCoherent, "Entry 0x%x", pFunc);
    return CACHE_DRV_IS_WRITE_COHERENT(pFunc);
}

/*******************************************************************************
 * cacheDrvIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDrvIsReadCoherent(CACHE_FUNCS *pFunc)
{
    log_debug(cacheDrvIsReadCoherent, "Entry 0x%x", pFunc);
    return CACHE_DRV_IS_READ_COHERENT(pFunc);
}

/*******************************************************************************
 * cacheDmaFlush - Flush dma cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDmaFlush(void *addr, size_t bytes)
{
    log_debug(cacheDmaFlush, "Entry 0x%x, %d", addr, bytes);
    return CACHE_DMA_FLUSH(addr, bytes);
}

/*******************************************************************************
 * cacheDmaInvalidate - Invalidate dma cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheDmaInvalidate(void *addr, size_t bytes)
{
    log_debug(cacheDmaInvalidate, "Entry 0x%x, %d", addr, bytes);
    return CACHE_DMA_INVALIDATE(addr, bytes);
}

/*******************************************************************************
 * cacheDmaVirtyToPhys - Virtual to physical memory
 *
 * RETURNS: Physical address
 ******************************************************************************/
void* cacheDmaVirtToPhys(void *addr)
{
    log_debug(cacheDmaVirtToPhys, "Entry 0x%x", addr);
    return CACHE_DMA_VIRT_TO_PHYS(addr);
}

/*******************************************************************************
 * cacheDmaPhysToVirt - Physical to virtual memory
 *
 * RETURNS: Virtual address
 ******************************************************************************/
void* cacheDmaPhysToVirt(void *addr)
{
    log_debug(cacheDmaPhysToVirt, "Entry 0x%x",addr);
    return CACHE_DMA_PHYS_TO_VIRT(addr);
}

/*******************************************************************************
 * cacheDmaIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDmaIsWriteCoherent(void)
{
    log_debug(cacheDmaIsWriteCoherent, "Entry");
    return CACHE_DMA_IS_WRITE_COHERENT();
}

/*******************************************************************************
 * cacheDmaIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheDmaIsReadCoherent(void)
{
    log_debug(cacheDmaIsReadCoherent, "Entry");
    return CACHE_DMA_IS_READ_COHERENT();
}

/*******************************************************************************
 * cacheUsrFlush - Flush user cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheUsrFlush(void *addr, size_t bytes)
{
    log_debug(cacheUsrFlush, "Entry 0x%x, %d", addr, bytes);
    return CACHE_USR_FLUSH(addr, bytes);
}

/*******************************************************************************
 * cacheUsrInvalidate - Invalidate user cache
 *
 * RETURNS: OK
 ******************************************************************************/
STATUS cacheUsrInvalidate(void *addr, size_t bytes)
{
    log_debug(cacheUsrInvalidate, "Entry 0x%x, %d", addr, bytes);
    return CACHE_USR_INVALIDATE(addr, bytes);
}

/*******************************************************************************
 * cacheUsrIsWriteCoherent - Get cache write coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheUsrIsWriteCoherent(void)
{
    log_debug(cacheUsrIsWriteCoherent, "Entry");
    return CACHE_USR_IS_WRITE_COHERENT();
}

/*******************************************************************************
 * cacheUsrIsReadCoherent - Get cache read coherency
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/
BOOL cacheUsrIsReadCoherent(void)
{
    log_debug(cacheUsrIsReadCoherent, "Entry");
    return CACHE_USR_IS_READ_COHERENT();
}

/*******************************************************************************
 * cacheFuncsSet - Set cache functions
 *
 * RETURNS: N/A
 ******************************************************************************/
void cacheFuncsSet(void)
{
    log_debug(cacheFuncsSet, "Entry");

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
    log_debug(cacheFuncsSet, "Exit");
}
