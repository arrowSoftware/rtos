/* sysLib.c - System dependent C code */

#include "config.h"
#include <sys/types.h>
#include <rtos.h>
#include <arch/regs.h>
#include <arch/iv.h>
#include <arch/intArchLib.h>
#include <arch/sysArchLib.h>
#include <arch/intArchLib.h>
#include <arch/mmuArchLib.h>
#include <os/cacheLib.h>
#include <os/vmLib.h>

/* Interrput controller */
#include <drv/intrCtl/i8259Pic.c>

/* Interrupt timer */
#include <drv/timer/i8253Timer.c>

/* Console */
#ifdef	INCLUDE_PC_CONSOLE
#include <drv/input/englishKeymap.c>
#include <drv/input/i8042Kbd.c>
#include <drv/video/latin1CharMap.c>
#include <drv/video/m6845Vga.c>
#include <drv/serial/pcConsole.c>
PC_CON_DEV pcConDev[N_VIRTUAL_CONSOLES];
#endif /* INCLUDE_PC_CONSOLE */

/* I/O devices */
#include <drv/disk/ramDrv.c>

/* Graphics driver */
#include <drv/video/vgahw.c>
#include <drv/netif/if_loop.c>

/* Dma driver */
#include <drv/dma/i8237Dma.c>

/* Floppy driver */
#include <drv/fdisk/nec765Fd.c>

#include <util/logging.h>

#ifdef LOCAL_MEM_AUTOSIZE

#define PHYS_MEM_MAX		0x100000000ull

#define MEM_TEST_PATTERN_0	0x5631ae31
#define MEM_TEST_PATTERN_1	0x21b79415
#define MEM_TEST_PATTERN_2	0x42538c72

#endif /* LOCAL_MEM_AUTOSIZE */

/* Macros */
#define MEM_HALF(x)		( ((x) + 1) >> 1 )

/* Imports */
IMPORT GDT sysGdt[];
IMPORT VOIDFUNCPTR intEoi;

IMPORT void sysLoadGdt(GDT *baseAddr);

/* Globals */
PHYS_MEM_DESC sysPhysMemDesc[] = {

  /* First memory page */
  {
    /* Virtual address */
    (void *) 0x00000000,

    /* Physical address */
    (void *) 0x00000000,

    /* Length */
    VM_PAGE_OFFSET,

#ifdef DEBUG_NULL_ACCESS

    /* Initial mask */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,

    /* Initial state */
    VM_STATE_NOT_VALID  | VM_STATE_NOT_WRITABLE  | VM_STATE_NOT_CACHEABLE

#else

    /* Initial mask */
    VM_STATE_MASK_FOR_ALL,

    /* Initial state */
    VM_STATE_FOR_MEM_OS

#endif /* DEBUG_NULL_ACCESS */

  }, /* End first memory page */

  /* Lower memory valid access */
  {
    /* Virtual address */
    (void *) VM_PAGE_OFFSET,

    /* Physical address */
    (void *) VM_PAGE_OFFSET,

    /* Length */
    0xa0000 - VM_PAGE_OFFSET,

    /* Initial mask */
    VM_STATE_MASK_FOR_ALL,

    /* Initial state */
    VM_STATE_FOR_MEM_OS

  }, /* End lower memory valid access */

  /* Video ram */
  {
    /* Virtual address */
    (void *) 0x000a0000,

    /* Physical address */
    (void *) 0x000a0000,

    /* Length */
    0x00060000,

    /* Initial mask */
    VM_STATE_MASK_FOR_ALL,

    /* Initial state */
    VM_STATE_FOR_IO

  }, /* End lower memory valid access */

  /* Operating system memory */
  {
    /* Virtual address */
    (void *) LOCAL_MEM_LOCAL_ADRS,

    /* Physical address */
    (void *) LOCAL_MEM_LOCAL_ADRS,

    /* Length */
    LOCAL_MEM_SIZE_OS,

    /* Initial mask */
    VM_STATE_MASK_FOR_ALL,

    /* Initial state */
    VM_STATE_FOR_MEM_OS

  }, /* End lower memory valid access */

  /* Application memory */
  {
    /* Virtual address */
    (void *) ((unsigned long)LOCAL_MEM_LOCAL_ADRS + (unsigned long)LOCAL_MEM_SIZE_OS),

    /* Physical address */
    (void *) ((unsigned long)LOCAL_MEM_LOCAL_ADRS + (unsigned long)LOCAL_MEM_SIZE_OS),

    /* Length */
    LOCAL_MEM_SIZE - LOCAL_MEM_SIZE_OS,

    /* Initial mask */
    VM_STATE_MASK_FOR_ALL,

    /* Initial state */
    VM_STATE_FOR_MEM_APPLICATION

  }, /* End lower memory valid access */

  {

    (void *) VM_INVALID_ADDR,
    (void *) VM_INVALID_ADDR,
    0,
    0,
    0

  }

};

unsigned int sysFdBuf = FD_DMA_BUF_ADDR;
unsigned int sysFdBufSize = FD_DMA_BUF_SIZE;

int sysPhysMemDescNumEntries;

u_int32_t sysIntIdtType	= SYS_INT_TRAPGATE;
u_int32_t sysVectorIRQ0	= INT_NUM_IRQ0;
GDT *pSysGdt = (GDT *) (LOCAL_MEM_LOCAL_ADRS + GDT_BASE_OFFSET);
char *memTopPhys = NULL;
char *memTop = NULL;

char* sysPhysMemTop(void);

/*******************************************************************************
 * sysHwInit0 - Very basic system initialization
 *
 * RETURNS: N/A
 ******************************************************************************/
void sysHwInit0(void)
{
    log_debug(sysHwInit0, "Entry", 0);
    log_debug(sysHwInit0, "Exit", 0);
}

/*******************************************************************************
 * sysHwInit - Called before multitasking
 *
 * RETURNS: N/A
 ******************************************************************************/
void sysHwInit(void)
{
    int i;
    PHYS_MEM_DESC *pVm;

    log_debug(sysHwInit, "Entry", 0);

#ifdef INCLUDE_MMU
    /* Initialize number of virtual memory descriptors */
    pVm = &sysPhysMemDesc[0];

    /* For all virtual memory descriptos */
    for (i = 0; i < NELEMENTS(sysPhysMemDesc); i++)
    {
        if ( (int) pVm->vAddr != (int) VM_INVALID_ADDR)
        {
            pVm++;
        }
        else
        {
            break;
        }
    }

    /* Store number of descriptors */
    sysPhysMemDescNumEntries = i;
#endif /* INCLUDE_MMU */

    /* Initialize interrupt controller */
    sysIntInitPIC();
    intEoi = sysIntEOI;
    log_debug(sysHwInit, "Exit", 0);
}

/*******************************************************************************
 * sysHwInit2 - Called after multitasking & memory manager has been initalized
 *
 * RETURNS: N/A
 ******************************************************************************/
void sysHwInit2(void)
{
  /* Connect system timer interrupt handler */
  intConnectFunction(0x20, sysClockInt, NULL);

#ifdef INCLUDE_PC_CONSOLE

  intConnectFunction(0x21, kbdIntr, (void *) 0);

#endif /* INCLUDE_PC_CONSOLE */
}

/*******************************************************************************
 * sysDelay - Delay routine
 *
 * RETURNS: N/A
 ******************************************************************************/

int sysDelay(void)
{
  char c;

  c = sysInByte(UNUSED_ISA_IO_ADDRESS);

  return c;
}

/*******************************************************************************
 * sysMemTop - Get system memory upper limit and load global descriptor table
 *
 * RETURNS: Pointer to system memory end
 ******************************************************************************/

char* sysMemTop(void)
{
  /* If memory already installed */
  if (memTop != NULL)
    return memTop;

  /* Get physical upper memory limit */
  memTop = sysPhysMemTop() - USER_RESERVED_MEM;

  return memTop;
}

#ifdef LOCAL_MEM_AUTOSIZE

/*******************************************************************************
 * memWriteTestPattern - Write test patterns to memory
 *
 * RETURNS: N/A
 ******************************************************************************/

void memWriteTestPattern(int *testAddr, int *saveAddr)
{

  /* Save current contents */
  saveAddr[0] = testAddr[0];
  saveAddr[1] = testAddr[1];
  saveAddr[2] = testAddr[2];

  /* Write test patterns */
  testAddr[0] = MEM_TEST_PATTERN_0;
  testAddr[1] = MEM_TEST_PATTERN_1;
  testAddr[2] = MEM_TEST_PATTERN_2;

  /* Flush cache */
  cacheFlush(DATA_CACHE, testAddr, 16);
}

/*******************************************************************************
 * memVerifyTestPattern - Verify test pattern for memory scan
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS memVerifyTestPattern(int *testAddr)
{
  if (testAddr[0] != MEM_TEST_PATTERN_0)
    return ERROR;

  if (testAddr[1] != MEM_TEST_PATTERN_1)
    return ERROR;

  if (testAddr[2] != MEM_TEST_PATTERN_2)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * memRestoreTestAddr - Restore memory addresses which where tested
 *
 * RETURNS: N/A
 ******************************************************************************/

void memRestoreTestAddr(int *testAddr, int *saveAddr)
{

  /* Restore memory contents */
  testAddr[0] = saveAddr[0];
  testAddr[1] = saveAddr[1];
  testAddr[2] = saveAddr[2];
}

/*******************************************************************************
 * memAutosize - Get maximum memory
 *
 * RETURNS: Pointer to memory upper limit or NULL
 ******************************************************************************/

void* memAutosize(void)
{
  unsigned char *pageAddr, *prevPageAddr, *nextPageAddr;
  unsigned int pageSize, pageNumUsed, pageNumTotal, delta;
  int tmp[4];

  /* Initialize locals */
  pageSize = PAGE_SIZE_4KB;
  pageAddr = (unsigned char *) ROUND_UP(FREE_RAM_ADRS, pageSize);
  pageNumUsed = ( (unsigned int) pageAddr / pageSize);
  pageNumTotal = (PHYS_MEM_MAX / pageSize) - pageNumUsed;
  delta = MEM_HALF(pageNumTotal);

  /* For all memory pages */
  for (pageAddr += (pageSize * delta); delta != 0; delta >>= 1) {

    /* Write test pattern to memory */
    memWriteTestPattern((int *) pageAddr, tmp);

    /* If test pattern was not written sucessfully */
    if (memVerifyTestPattern((int *) pageAddr) != OK) {

      /* Calculate previous page address */
      prevPageAddr = (unsigned char *) ( (unsigned int) pageAddr - pageSize );

      /* Write test pattern */
      memWriteTestPattern((int *) prevPageAddr, tmp);

      /* If previous page test pattern written successfully */
      if (memVerifyTestPattern((int *) prevPageAddr) == OK) {

        /* Restore tested memory area */
        memRestoreTestAddr((int *) prevPageAddr, tmp);

        /* Found memory limit */
        return pageAddr;

      } /* End if previous page test pattern written successfully */

      /* Advance backwards */
      pageAddr -= ( pageSize * MEM_HALF(delta) );

    } /* End if test pattern was not written sucessfully */

    /* Else test pattern was written successfully */
    else {

      /* Restore tested memory */
      memRestoreTestAddr((int *) pageAddr, tmp);

      /* Calculate next page address */
      nextPageAddr = ( (unsigned char *) pageAddr + pageSize );

      /* Write test pattern */
      memWriteTestPattern((int *) nextPageAddr, tmp);

      /* If test pattern not written sucessfully */
      if (memVerifyTestPattern((int *) nextPageAddr) != OK) {

        /* Found memory limit */
        return nextPageAddr;

      } /* End if test pattern not written sucessfully */

      /* Restore tested memory */
      memRestoreTestAddr((int *) nextPageAddr, tmp);

      /* Advance */
      pageAddr += ( pageSize * MEM_HALF(delta) );

    } /* End else test pattern was written successfully */

  } /* End for all memory pages */

  return NULL;
}

#endif /* LOCAL_MEM_AUTOSIZE

/*******************************************************************************
 * sysPhysMemTop - Get memory upper limit and load global descriptor table
 *
 * RETURNS: Pointer to memory upper limit
 ******************************************************************************/

char* sysPhysMemTop(void)
{
  char gdtr[6];

#ifdef LOCAL_MEM_AUTOSIZE

  PHYS_MEM_DESC *pMmu;

#endif /* LOCAL_MEM_AUTOSIZE */

  /* If memory already installed */
  if (memTopPhys != NULL)
    return memTopPhys;

#ifdef LOCAL_MEM_AUTOSIZE

  /* Get memory limit */
  memTopPhys = memAutosize();
  if (memTopPhys == NULL)
    memTopPhys = (char *) (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);

#else /* LOCAL_MEM_AUTOSIZE */

  memTopPhys = (char *) (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);

#endif /* LOCAL_MEM_AUTOSIZE */

  /* Copy global descriptor table to the requested address */
  memcpy(pSysGdt, sysGdt, GDT_ENTRIES * sizeof(GDT));

  /* Size of global descriptor table */
  *(short *) &gdtr[0] = GDT_ENTRIES * sizeof(GDT) - 1;

  /* Address of global descriptor table */
  *(int *) &gdtr[2] = (int) pSysGdt;

  /* Flush GDT */
  sysLoadGdt((GDT *) gdtr);

#ifdef LOCAL_MEM_AUTOSIZE

  /* Align memory according to page size */
  memTopPhys = (unsigned char *) ROUND_DOWN(memTopPhys, VM_PAGE_SIZE);

  /* Get mmu table entry above os memory */
  pMmu = &sysPhysMemDesc[4];

  /* Adjust length in mmu table */
  pMmu->len = (unsigned int) memTopPhys - (unsigned int) pMmu->pAddr;

#endif /* LOCAL_MEM_AUTOSIZE */

  return memTopPhys;
}

