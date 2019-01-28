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

/* intShLib.h - Interrupt library header */

/* This code is largely based on KallistiOS
 * File arch/dreamcast/include/irq.h
 * Original (c)2000-2001 Dan Potter
 */

#ifndef _intShLib_h
#define _intShLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <arch/regs.h>
#include <arch/excArchLib.h>

#define INT_LOCK(oldLevel)	((oldLevel) = intLock())
#define INT_UNLOCK(oldLevel)	(intUnlock((oldLevel)))

/* Set a handler, or remove a handler (see codes above) */
int intConnectHandler(u_int32_t inum, HANDLERPTR hnd);
/**  TEMOORARY REMOVAL OF CONNECT FUNCTION CALL ERROR */
#define intConnectFunction(inum, hnd, arg) \
	intConnectHandler(inum, (HANDLERPTR) hnd)

/* Init routine */
int intVecBaseSet(FUNCPTR *baseAddr);

/* Enable/Disable interrupts */
int intLock();			/* Will leave exceptions enabled */
void intUnlock(int v);		/* Restore interrupt level */
void intEnable();		/* Enables all ints including external */

/* Shutdown */
void irq_shutdown();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _intShLib_h */

