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

/* selectLib.h - Select library header */

#ifndef _selectLib_h
#define _selectLib_h

#include <tools/moduleNumber.h>
#define S_selectLib_NO_SELECT_CONTEXT			(M_selectLib | 0x0001)
#define S_selectLib_NO_SELECT_SUPPORT_IN_DRIVER		(M_selectLib | 0x0002)
#define S_selectLib_WIDTH_OUT_OF_RANGE			(M_selectLib | 0x0003)

#include <sys/time.h>

#ifndef FD_SETSIZE
#define FD_SETSIZE		2048
#endif

#ifndef _ASMLANGUAGE

typedef long fd_mask;
#define NFDBITS		(sizeof(fd_mask) * 8)
#ifndef howmany
#define howmany(x, y)	((unsigned int)(((x)+((y)-1)))/(unsigned int)(y))
#endif

typedef struct fd_set {
  fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	memset((p), 0, sizeof(*(p)))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SELREAD,
  SELWRITE
} SELECT_TYPE;

#include <os/private/selectLibP.h>

typedef struct selContext *SEL_CONTEXT_ID;

extern STATUS selectLibInit(void);
extern STATUS selectLibInitDelete(void);
extern int select(int width,
		  fd_set *pReadFds,
		  fd_set *pWriteFds,
		  fd_set *pExceptFds,
		  struct timeval *pTimeOut);
extern STATUS selWakeupListInit(SEL_WAKEUP_LIST *pList);
extern void selWakeupListTerminate(SEL_WAKEUP_LIST *pList);
extern int selWakeupListLen(SEL_WAKEUP_LIST *pList);
extern STATUS selNodeAdd(SEL_WAKEUP_LIST *pList, SEL_WAKEUP_NODE *pNode);
extern STATUS selNodeDelete(SEL_WAKEUP_LIST *pList, SEL_WAKEUP_NODE *pNode);
extern void selWakeup(SEL_WAKEUP_NODE *pNode);
extern void selWakeupAll(SEL_WAKEUP_LIST *pList, SELECT_TYPE type);
extern SELECT_TYPE selWakeupType(SEL_WAKEUP_NODE *pNode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _selectLib_h */

