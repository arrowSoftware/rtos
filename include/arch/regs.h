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

/* regs.h - Architecture specific CPU regs */

#ifndef _regs_h_
#define _regs_h_

#include <types/rtosCpu.h>

#if	 CPU_FAMILY==MC680X0
#include <arch/m68k/regsM68k.h>
#endif

#if	 CPU_FAMILY==I386
#include <arch/i386/regsI386.h>
#endif

#if	 CPU_FAMILY==SH
#include <arch/sh/regsSh.h>
#endif

#if	 CPU_FAMILY==POWERPC
#include <arch/ppc/regsPpc.h>
#endif

#ifndef _ASMLANGUAGE

typedef struct regindex {
  char *regName;		/* Register name */
  int regOffset;		/* Register offset */
} REG_INDEX;

#endif /* _ASMLANGUANGE */

#endif

