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

/* rtosArch.h - Architecture specific */

#ifndef _rtosArch_h
#define _rtosArch_h

#define _BIG_ENDIAN			1234
#define _LITTLE_ENDIAN			4321
#define _STACK_GROWS_DOWN		(-1)
#define _STACK_GROWS_UP			1

#if CPU_FAMILY==MC680X0
#include <arch/m68k/archM68k.h>
#endif /* CPU_FAMILY==MC680X0 */

#if CPU_FAMILY==I386
#include <arch/i386/archI386.h>
#endif /* CPU_FAMILY==I386 */

#if CPU_FAMILY==SH
#include <arch/sh/archSh.h>
#endif /* CPU_FAMILY==SH */

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* End symbol of executable */
extern unsigned char end[];

/* Start of free memory */
#define FREE_RAM_ADRS			(end)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _rtosArch_h */

