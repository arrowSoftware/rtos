#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <private/stdioP.h>
#include <private/timeP.h>
#include <rtos.h>
#include <arch/iv.h>
#include <arch/sysArchLib.h>
#include <arch/intArchLib.h>
#include <arch/excArchLib.h>
#include <arch/taskArchLib.h>
#include <arch/mmuArchLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskShow.h>
#include <rtos/kernLib.h>
#include <rtos/rtosLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/taskVarLib.h>
#include <rtos/eventLib.h>
#include <rtos/msgQLib.h>
#include <rtos/kernTickLib.h>
#include <rtos/memPartLib.h>
#include <rtos/memLib.h>
#include <rtos/memShow.h>
#include <rtos/wdLib.h>
#include <rtos/wdShow.h>
#include <io/ioLib.h>
#include <io/iosLib.h>
#include <io/pathLib.h>
#include <io/iosShow.h>
#include <io/ttyLib.h>
#include <io/cbioLib.h>
#include <io/cbioShow.h>
#include <os/logLib.h>
#include <os/excLib.h>
#include <arch/excArchShow.h>
#include <os/sigLib.h>
#include <os/selectLib.h>
#include <util/hashLib.h>
#include <os/symLib.h>
#include <os/symShow.h>
#include <os/envLib.h>
#include <os/envShow.h>
#include <os/private/timerLibP.h>
#include <util/qPrioLib.h>
#include <util/qPriBmpLib.h>
#include <util/ringLib.h>
#include <signal.h>
#include <drv/serial/pcConsole.h>
#include <usr/usrLib.h>
#include <tools/shellLib.h>
#include <tools/debugLib.h>
#include <tools/moduleLib.h>
#include <tools/moduleShow.h>
#include <tools/loadLib.h>
#include <tools/loadElfLib.h>
#include <os/pipeDrv.h>
#include <os/cacheLib.h>
#include <usr/usrLib.h>
#include <usr/usrFsLib.h>
#include "../../drv/serial/serial.c"
#include "util/logging.h"

#define MEM_POOL_START_ADRS FREE_RAM_ADRS

extern int symTblAllSize;
extern SYMBOL symTblAll[];
extern void sysHwInit0(void);
extern void sysHwInit(void);
extern char* sysMemTop(void);
extern STATUS sysClockConnect(FUNCPTR func, int arg);
extern STATUS sysClockRateSet(int tickePerSecond);
extern void sysClockEnable(void);
extern STATUS excVecInit(void);
extern void usrKernelInit(void);
extern void usrMmuInit(void);
extern STATUS pipeDrvInit(void);
extern STATUS sigqueueInit(int nQueues);
extern STATUS erfLibInit(int  maxCategories, int  maxTypes);
extern STATUS envLibInit(BOOL installHooks);
extern STATUS fsEventUtilInit(void); // TYLER
extern STATUS xbdLibInit(int maxXbds); // TYLER
extern STATUS usrNetInit(char *bootstring);
extern STATUS usrLibInit(void);

#ifdef INCLUDE_DEMO
extern int startDemo(void);
extern void addDemo();
#endif /* INCLUDE_DEMO */

struct mem_part memHeapPart;
PART_ID memHeapPartId = &memHeapPart;
int consoleFd;
char consoleName[20];
SYMTAB_ID sysSymTable;

void usrRoot(char *pMemPoolStart, unsigned memPoolSize);
void usrInit(void);

/*******************************************************************************
*   Function:
*       userClock.
*
*   Description:
*       Clock ISR.
*
*   Arguments:
*        None.
*
*   Returns:
*       None.
*******************************************************************************/
void usrClock(void)
{ kernTickAnnounce(); }

/*******************************************************************************
*   Function:
*       usrInit.
*
*   Description:
*       The main entry point when the system boots.
*
*   Arguments:
*        None.
*
*   Returns:
*       None.
*******************************************************************************/
void usrInit(void)
{
    log_init();
    log_debug(usrInit, "Entry",0);

#ifdef INCLUDE_STDIO
    stdioLibInit();
#endif /* INCLUDE_STDIO */

    sysHwInit0();

#ifdef INCLUDE_CACHE_SUPPORT
    cacheLibInit(USER_I_CACHE_MODE, USER_D_CACHE_MODE);
#endif /* INCLUDE_CACHE_SUPPORT */

    /* Set exception base vector */
    intVecBaseSet((FUNCPTR *)VEC_BASE_ADRS);

#ifdef INCLUDE_EXC_HANDELING
    excVecInit();
#endif /* INCLUDE_EXC_HANDELING */

    /* Initialize hardware */
    sysHwInit();

    /* Initialize kernel libraries */
    usrKernelInit();

#ifdef INCLUDE_CACHE_SUPPORT
    #ifdef USER_I_CACHE_ENABLE
        cacheEnable(INSTRUCTION_CACHE);
    #endif /* USER_I_CACHE_ENABLE */
    #ifdef USER_D_CACHE_ENABLE
        cacheEnable(DATA_CACHE);
    #endif /* USER_D_CACHE_ENABLE */
#endif /* INCLUDE_CACHE_SUPPORT

    /* Initialize kernel */
    kernelInit((FUNCPTR)usrRoot,            /* Startup-function */
               ROOT_STACK_SIZE,             /* Root task stack size */
               (char *)MEM_POOL_START_ADRS, /* Start of memory pool */
               sysMemTop(),                 /* End of memory pool */
               ISR_STACK_SIZE,              /* Interrupt stack size */
               INT_LOCK_LEVEL);             /* Interrupt lockout level */

    log_debug(usrInit, "Exit", 0);
}

/*******************************************************************************
 * usrRoot - The root task executed as the first task when the system starts
 *
 * RETURNS: N/A
 ******************************************************************************/
void usrRoot(char *pMemPoolStart, unsigned memPoolSize)
{
    char ttyName[20];
    int i;

    log_debug(usrRoot, "Entry: 0x%x, %d", pMemPoolStart, memPoolSize);

    /* Initialize memory */
    memPartLibInit(pMemPoolStart, memPoolSize);

#ifdef INCLUDE_MEM_MGR_FULL
    memLibInit();
#endif /* INCLUDE_MEM_MGR_FULL */

#ifdef INCLUDE_SHOW_ROUTINES
    memShowInit();
#endif /* INCLUDE_SHOW_ROUTINES */

    memPartInit(&memHeapPart, sysMemTop(), USER_RESERVED_MEM);

#ifdef INCLUDE_WDOG
    wdLibInit();
    #ifdef INCLUDE_SHOW_ROUTINES
        wdShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_WDOG */

#ifdef INCLUDE_MMU
    usrMmuInit();
#endif /* INCLUDE_MMU */

    /* Install and start system clock interrupt */
    sysClockConnect((FUNCPTR) usrClock, 0);
    sysClockRateSet(SYS_CLOCK_RATE);
    sysClockEnable();

#ifdef INCLUDE_TASK_VAR
    taskVarLibInit();
#endif /* INCLUDE_TASK_VAR */

#ifdef INCLUDE_SELECT
    selectLibInit();
#endif /* INCLUDE_SELECT */

#ifdef INCLUDE_IO_SYSTEM
    pathLibInit();
    iosLibInit(NUM_DRIVERS, NUM_FILES, "/null");
    #ifdef INCLUDE_SHOW_ROUTINES
        iosShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */

    consoleFd = 0;

    #ifdef INCLUDE_TTY_DRV
        if (NUM_TTY > 0)
        {
            ttyDrv();
            for (i = 0; i < NUM_TTY; i++)
            {
                sprintf(ttyName, "%s%d", "/tyCo/", i);
                ttyDevCreate(ttyName, i, 512, 512);
                if (i == CONSOLE_TTY)
                {
                    strcpy(consoleName, ttyName);
                }
            }
            consoleFd = open(consoleName, O_RDWR, 0);

            ioctl(consoleFd, FIOBAUDRATE, CONSOLE_BUAD_RATE);
            ioctl(consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
        }
    #endif

    #ifdef INCLUDE_PC_CONSOLE
        /* Initailize pc console driver */
        pcConDrvInit();

        /* For all virtual consoles */
        for (i = 0; i < N_VIRTUAL_CONSOLES; i++)
        {
            /* Create name for device */
            strcpy(ttyName, "/pcConsole/");
            ttyName[strlen(ttyName)] = i + '0';
            ttyName[strlen(ttyName) + 1] = EOS;

            /* Create device */
            pcConDevCreate(ttyName, i, 512, 512);

            /* If pc console number */
            if (i == PC_CONSOLE)
            {
                /* Copy to global console name */
                strcpy(consoleName, ttyName);

                /* Open file and set console options */
                consoleFd = open(consoleName, O_RDWR, 0);
                ioctl(consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
            } /* End if pc console number */
        } /* End for all virtual consoles */
    #endif /* INCLUDE_PC_CONSOLE */

    /* Set standard file descriptors */
    ioGlobalStdSet(STDIN_FILENO, consoleFd);
    ioGlobalStdSet(STDOUT_FILENO, consoleFd);
    ioGlobalStdSet(STDERR_FILENO, consoleFd);
#endif /* INCLUDE_IO_SYSTEM */

#ifdef INCLUDE_SYM_TBL
    hashLibInit();
    symLibInit();
    #ifdef INCLUDE_SHOW_ROUTINES
        symShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_SYM_TBL */

#ifdef INCLUDE_TIME
    clockLibInit();
    timeLibInit();
#endif /* INCLUDE_TIME */

#if defined(INCLUDE_EXC_HANDELING) && defined(INCLUDE_EXC_TASK)
    #ifdef INCLUDE_EXC_SHOW
        excShowInit();
    #endif /* INCLUDE_EXC_SHOW */
    excLibInit();
#endif /* INCLUDE_EXC_HANDELING && INCLUDE_EXC_TASK */

#ifdef INCLUDE_LOGGING
    logLibInit(consoleFd, MAX_LOG_MSGS);
    #ifdef INCLUDE_LOG_STARTUP
        logMsg("logging started to %s [%d], queue size %d\n",
               (ARG) consoleName, (ARG) consoleFd, (ARG) MAX_LOG_MSGS,
               (ARG)4, (ARG)5, (ARG)6);
        taskDelay(2);
    #endif /* INCLUDE_LOG_STARTUP */
#endif /* INCLUDE_LOGGING */

#ifdef INCLUDE_SIGNALS
    sigLibInit();
#endif /* INCLUDE_SIGNALS */

#ifdef INCLUDE_DEBUG
    debugLibInit();
#endif /* INCLUDE_DEBUG */

#ifdef INCLUDE_PIPES
    pipeDrvInit();
#endif /* INCLUDE_PIPES */

#if defined(INCLUDE_POSIX_SIGNALS) && defined(INCLUDE_SIGNALS)
    sigqueueInit(NUM_SIGNAL_QUEUES);
#endif /* INCLUDE_POSIX_SIGNALS && INCLUDE_SIGNALS */

#ifdef INCLUDE_ENV_VARS
    envLibInit(ENV_VAR_USE_HOOKS);
    #ifdef INCLUDE_SHOW_ROUTINES
        envShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_END_VARS */

#ifdef INCLUDE_CBIO
    cbioLibInit();
    #ifdef INCLUDE_SHOW_ROUTINES
        cbioShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_CBIO */

#ifdef INCLUDE_FILESYSTEMS
    erfLibInit (20, 20); /* Initialize the event reporting framework */
    fsEventUtilInit();   /* Initialize the F/S event utilities */
    xbdLibInit(64);      /* Max of 64 different XBDs in system */
#endif /* INCLUDE_FILESYSTEMS */

#ifdef INCLUDE_SELECT
    selectLibInitDelete();
#endif /* INCLUDE_SELECT */

sysSymTable = symTableCreate(SYM_TABLE_HASH_SIZE_LOG2, TRUE, memSysPartId);

printf("Adding %d symbols for symTblAll.\n", symTblAllSize);

for (i = 0; i < symTblAllSize; i++)
{
    symTableAdd(sysSymTable, &symTblAll[i]);
}

#ifdef INCLUDE_LOADER
    moduleLibInit();
    #ifdef INCLUDE_SHOW_ROUTINES
        moduleShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
    loadLibInit();

    #ifdef INCLUDE_ELF
        loadElfLibInit();
    #endif /* INCLUDE_ELF */
#endif /* INCLUDE_LOADER */

#ifdef INCLUDE_NET
    usrNetInit("");
#endif /* INCLUDE_NET */

    printLogo();

#ifdef INCLUDE_SHELL
    shellLibInit(SHELL_STACK_SIZE, (ARG) TRUE);
#endif /* INCLUDE_SHELL */

#ifdef INCLUDE_USR_LIB
    usrLibInit();
    usrFsLibInit();
#endif /* INCLUDE_USR_LIB */

#ifdef INCLUDE_ASSERT
    assertLibInit();
#endif /* INCLUDE_ASSERT */

#ifdef INCLUDE_DEMO
    addDemo();
#endif /* INCLUDE_DEMO */

    init_serial();
    log_debug(usrRoot, "Exit", 0);
}
