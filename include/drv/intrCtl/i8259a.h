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

/* i8259a.h - Programable interrupt controller */

#ifndef _i8259a_h
#define _i8259a_h

#include <rtos.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */
#define PIC1_BASE_ADR		0x20
#define PIC2_BASE_ADR		0xa0
#define PIC_ADRS(base,reg)	(base + reg * PIC_REG_ADDR_INTERVAL)

/* Macros */
#define PIC_Port1(base)		PIC_ADRS(base,0x00)	/* Port 1 */
#define PIC_Port2(base)		PIC_ADRS(base,0x01)	/* Port 2 */
#define PIC_IMASK(base)		PIC_Port2(base)		/* Interrupt msk */
#define PIC_IACK(base)		PIC_Port1(base)		/* Intrrupt ack */
#define PIC_ISR_MASK(base)	PIC_Port1(base)		/* In service reg msk */
#define PIC_IRR_MASK(base)	PIC_Port1(base)		/* Interrupt req reg */

IMPORT void sysIntInitPIC(void);
IMPORT STATUS sysIntEnablePIC(int level);
IMPORT STATUS sysIntDisablePIC(int level);
IMPORT void sysIntLock(void);
IMPORT void sysIntUnlock(void);
IMPORT int sysIntLevel(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _i8259a_h */

