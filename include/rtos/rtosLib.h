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

/* rtosLib.h - Task scheduling library */

#ifndef _rtosLib_h
#define _rtosLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include <util/qLib.h>
#include <rtos/taskLib.h>
#include <rtos/wdLib.h>

/* defines */

#define rtos_TIMEOUT   1    /* unpend handler called due to a timeout */

/* imports */

IMPORT void rtosSpawn(TCB_ID pTcb);
IMPORT STATUS rtosDelete(TCB_ID pTcb);
IMPORT void rtosSuspend(TCB_ID pTcb);
IMPORT void rtosResume(TCB_ID pTcb);
IMPORT void rtosTickAnnounce(void);
IMPORT STATUS rtosDelay(unsigned timeout);
IMPORT STATUS rtosUndelay(TCB_ID pTcb);
IMPORT void rtosPrioritySet(TCB_ID pTcb, unsigned priority);
IMPORT void rtosSemDelete(SEM_ID semId);

IMPORT void rtosPendQGet(Q_HEAD *pQHead);
IMPORT void rtosReadyQPut(TCB_ID pTcb);
IMPORT void rtosReadQRemove(Q_HEAD *pQHead, unsigned timeout);
IMPORT void rtosPendQFlush(Q_HEAD *pQHead);
IMPORT STATUS rtosPendQPut(Q_HEAD *pQHead, unsigned timeout);
IMPORT void rtosPendQWithHandlerPut(Q_HEAD *pQHead, unsigned timeout,
                                     void (*handler)(void *, int),
                                     void *pObj, int info);
IMPORT STATUS rtosPendQRemove(TCB_ID pTcb);
IMPORT void rtosPendQTerminate(Q_HEAD *pQHead);
IMPORT STATUS rtosWdStart(WDOG_ID wdId, unsigned timeout);
IMPORT void rtosWdCancel(WDOG_ID wdId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _rtosLib_h */

