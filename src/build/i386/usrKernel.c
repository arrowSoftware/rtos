#include <rtos.h>
#include <util/classLib.h>
#include <util/classShow.h>
#include <util/qLib.h>
#include <util/qFifoLib.h>
#include <util/qPrioLib.h>
#include <util/qPriBmpLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/taskHookShow.h>
#include <rtos/semLib.h>
#include <rtos/semShow.h>
#include <rtos/eventLib.h>
#include <rtos/msgQLib.h>
#include <rtos/msgQShow.h>
#include <rtos/taskShow.h>
#include <rtos/kernQLib.h>
#include <rtos/kernTickLib.h>
#include "config.h"
#include "util/logging.h"

IMPORT Q_HEAD kernActiveQ;
IMPORT Q_HEAD kernTickQ;
IMPORT Q_HEAD kernReadyQ;
extern STATUS taskShowInit(void);
extern STATUS mmuLibInit(int pageSize);
extern void printExc(char *fmt, ARG arg0, ARG arg1, ARG arg2, ARG arg3, ARG arg4);

#ifdef INCLUDE_CONSTANT_RDY_Q
DL_LIST kernReadyLst[256];
unsigned kernReadyBmp[8];
#endif /* INCLUDE_CONSTANT_RDY_Q */

/*******************************************************************************
 * userKernelInit - Initialize user kernel
 *
 * RETURNS: N/A
 ******************************************************************************/
void usrKernelInit(void)
{
    log_debug(usrKernelInit, "Entry", 0);

    classLibInit();
    taskLibInit();

#ifdef INCLUDE_TASK_HOOKS
    taskHookLibInit();
    #ifdef INCLUDE_SHOW_ROUTINES
        taskHookShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_TASK_HOOK */

#ifdef INCLUDE_SEM_BINARY
    semLibInit();
    semBLibInit();
#endif /* INCLUDE_SEM_BINARY */

#ifdef INCLUDE_SEM_MUTEX
    semLibInit();
    semMLibInit();
#endif /* INCLUDE_SEM_MUTEX */

#ifdef INCLUDE_SEM_COUNTING
    semLibInit();
    semCLibInit();
#endif /* INCLUDE_SEM_COUNTING */

#ifdef INCLUDE_SEM_RW
    semLibInit();
    semRWLibInit();
#endif /* INCLUDE_SEM_RW */

#ifdef INCLUDE_rtos_EVENTS
    eventLibInit();
#endif /* INCLUDE_rtos_EVENTS */

#ifdef INCLUDE_MSG_Q
    msgQLibInit();
    #ifdef INCLUDE_SHOW_ROUTINES
        msgQShowInit();
    #endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_MSG_Q */

#ifdef INCLUDE_SHOW_ROUTINES
    classShowInit();
    taskShowInit();
    semShowInit();
#endif /* INCLUDE_SHOW_ROUTINES */

    /* Initialize kernel queues */
    qInit(&kernActiveQ, qFifoClassId);
    qInit(&kernTickQ, qPrioClassId);

#ifdef INCLUDE_CONSTANT_RDY_Q
    qInit(&kernReadyQ, qPriBmpClassId, 256, kernReadyLst, kernReadyBmp);
#else
    qInit(&kernReadyQ, qPrioClassId);
#endif /* INCLUDE_CONSTANT_RDY_Q */

    /* Initialize kernel work queue */
    kernQLibInit();

    /* Initialize kernel clock to zero */
    kernTickLibInit();

    log_debug(usrKernelInit, "Exit", 0);
}

