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

/* mmuLib.h - Memory mapping unit */

#ifndef _mmuArchLib_h
#define _mmuArchLib_h

#include <tools/moduleNumber.h>

#include <types/rtosCpu.h>

#if      CPU_FAMILY==I386
#include <arch/i386/mmuI386Lib.h>
#endif

#define S_mmuLib_NOT_INSTALLED           (M_mmuLib | 0x0001)
#define S_mmuLib_UNSUPPORTED_PAGE_SIZE   (M_mmuLib | 0x0002)
#define S_mmuLib_UNABLE_TO_GET_PTE       (M_mmuLib | 0x0003)
#define S_mmuLib_PAGE_NOT_PRESENT        (M_mmuLib | 0x0004)

#endif /* _mmuArchLib_h */

