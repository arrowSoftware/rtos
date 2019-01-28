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

/* rtosCpu.h - Define CPU family from type */

#ifndef _rtosCpu_h
#define _rtosCpu_h

#ifdef __cplusplus
extern "C" {
#endif

/* CPU types */
#define MC680X0		1	/* CPU family */
#define MC68000		2
#define MC68020		3
#define	MC68030		4
#define MC68040		5

#define	I386		10	/* CPU family */
#define I80386		11
#define I80486		12
#define PENTIUM		13
#define PENTIUM2	14
#define PENTIUM3	15
#define PENTIUM4	16

#define SH		20	/* CPU family */
#define SH7750		21	

#define	POWERPC		30	/* CPU family */
#define PPC7447A	31

/* Define CPU_FAMILY MC680X0 */
#if	(CPU==MC68000 || \
	 CPU==MC68020 || \
	 CPU==MC68030 || \
	 CPU==MC68040)
#define CPU_FAMILY	MC680X0
#endif /* CPU_FAMILY==MC680X0 */

/* Define CPU_FAMILY I386 */
#if	(CPU==I80386 || \
	 CPU==I80486 || \
	 CPU==PENTIUM || \
	 CPU==PENTIUM2 || \
	 CPU==PENTIUM3 || \
	 CPU==PENTIUM4)
#define CPU_FAMILY	I386
#endif /* CPU_FAMILY==I386 */

/* Define CPU_FAMILY SH */
#if	(CPU==SH7750)
#define CPU_FAMILY	SH
#endif /* CPU_FAMILY==SH */

/* Define CPU_FAMILY POWERPC */
#if	(CPU==PPC7447A)
#define CPU_FAMILY	POWERPC
#endif /* CPU_FAMILY==POWERPC */

/* Check if CPU family was define correctly */
#if !defined(CPU) || !defined(CPU_FAMILY)
#error CPU not defined correctly
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _rtosCpu_h */

