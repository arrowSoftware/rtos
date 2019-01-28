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

/* i8253.h - Intel 8253 Programmable Interrupt Timer (PIT) */

#ifndef _i8253_h
#define _i8253_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */
#define PIT_ADRS(base, reg)		( base + reg * PIT_REG_ADDR_INTERVAL )
#define PIT_CNT0(base)			PIT_ADRS(base, 0x00)
#define PIT_CNT1(base)			PIT_ADRS(base, 0x01)
#define PIT_CNT2(base)			PIT_ADRS(base, 0x02)
#define PIT_CMD(base)			PIT_ADRS(base, 0x03)

/* Functions */
IMPORT STATUS sysClockConnect(FUNCPTR func, int arg);
IMPORT void sysClockInt(void);
IMPORT void sysClockEnable(void);
IMPORT void sysClockDisable(void);
IMPORT STATUS sysClockRateSet(int ticksPerSecond);
IMPORT int sysClockRateGet(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _i8253_h */

