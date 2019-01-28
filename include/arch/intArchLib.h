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

/* intLib.h - Interrupt library */

#ifndef _intArchLib_h
#define _intArchLib_h

#include <types/rtosCpu.h>

#if    CPU_FAMILY==MC680X0
#include <arch/m68k/intM68k.h>
#elif  CPU_FAMILY==I386
#include <arch/i386/intI386Lib.h>
#elif  CPU_FAMILY==SH
#include <arch/sh/intShLib.h>
#elif  CPU_FAMILY==POWERPC
#include <arch/ppc/intPpcLib.h>
#else
#error "*** Unknown CPU_FAMILY\n"
#endif

#define INT_CONTEXT()		(intCnt > 0)
#define INT_RESTRICT()		((intCnt > 0) ? ERROR : OK)

#define S_intLib_NOT_ISR_CALLABLE      (M_intLib | 0x001)

#endif /* _intArchLib_h */

