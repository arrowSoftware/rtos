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

/* sysALib.s - System dependent assebler code */

#define _ASMLANGUAGE
#include "config.h"

#include <rtos.h>
#include <arch/asm.h>

#include "config.h"

	/* Internals */
	.globl	GTEXT(sysInit)

	.globl	GTEXT(sysGdtr)
	.globl	GTEXT(sysGdt)

	.globl	GDATA(sysCsSuper)
	.globl	GDATA(sysCsExc)
	.global	GDATA(sysCsInt)

	/* Externals */
	.globl	GTEXT(usrInit)

	.text
	.balign	16

/******************************************************************************
* sysInit - This is the os entry point, fix stack load gdt transfer
* control to usrInit in usrConfig.c
*
* RETURNS:   N/A
* PROTOTYPE: void sysInit(void)
*
******************************************************************************/

FUNC_LABEL(sysInit)
	/* Initialize stack */

	cli				/* Disable interrupts */
	movl	$ FUNC(sysInit),%esp	/* Initialize stack pointer */
	xorl	%ebp,%ebp		/* Initialize stack frame */

	/* Start os */

	call	FUNC(usrInit)		/* Call usrInit */
	iret

/*****************************************************************************
* For some mysteriout reason this two files has to be inlucded here
*****************************************************************************/

#include <arch/i386/kernALib.s>
#include <arch/i386/memcpy.s>

/*******************************************************************************
 * Global descriptor table (GDT) template
 ******************************************************************************/

	.text
	.balign	16, 0x90

FUNC_LABEL(sysGdtr)

	/* Fallback GDT header */
	.word	0x0027			/* Size of GDT table */
	.long	FUNC(sysGdt)		/* Address to GDT table */

	.balign	16

FUNC_LABEL(sysGdt)

	/* Null segment descriptor 0x0000 */
	.word	0x0000				/* Limit */
	.word	0x0000				/* Base */
	.byte	0x00				/* Base */
	.byte	0x00				/* Code, present */
	.byte	0x00				/* Limit, page gra. */
	.byte	0x00				/* Base */

	/* Code segment descriptor 0x0008 */
	.word	0xffff				/* Limit */
	.word	0x0000				/* Base */
	.byte	0x00				/* Base */
	.byte	0x9a				/* Code, present */
	.byte	0xcf				/* Limit, page gra. */
	.byte	0x00				/* Base */

	/* Data segment descriptor 0x0010 */
	.word	0xffff				/* Limit */
	.word	0x0000				/* Base */
	.byte	0x00				/* Base */
	.byte	0x92				/* Code, present */
	.byte	0xcf				/* Limit, page gr. */
	.byte	0x00				/* Base */

	/* Code segment descriptor for exception 0x0018 */
	.word	0xffff				/* Limit */
	.word	0x0000				/* Base */
	.byte	0x00				/* Base */
	.byte	0x9a				/* Code, present */
	.byte	0xcf				/* Limit, page gr. */
	.byte	0x00				/* Base */

	/* Code segment descriptor for interrupt 0x0020 */
	.word	0xffff				/* Limit */
	.word	0x0000				/* Base */
	.byte	0x00				/* Base */
	.byte	0x9a				/* Code, present */
	.byte	0xcf				/* Limit, page gra. */
	.byte	0x00				/* Base */

	.data

FUNC_LABEL(sysCsSuper)

	.long	0x00000008

FUNC_LABEL(sysCsExc)

	.long	0x00000018

FUNC_LABEL(sysCsInt)

	.long	0x00000020

