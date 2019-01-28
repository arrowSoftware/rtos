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

/* taskLib.h - Task context switching stuff */

#ifndef _taskArchLib_h
#define _taskArchLib_h

#include <types/rtosCpu.h>

#if      CPU_FAMILY==MC680X0
#include <arch/m68k/taskM68k.h>
#elif    CPU_FAMILY==I386
#include <arch/i386/taskI386Lib.h>
#elif    CPU_FAMILY==SH
#include <arch/sh/taskShLib.h>
#elif    CPU_FAMILY==POWERPC
#include <arch/ppc/taskPpcLib.h>
#else
#error "*** CPU_FAMILY not defined!\n"
#endif

#endif /* _taskArchLib_h */

