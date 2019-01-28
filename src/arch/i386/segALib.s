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

/* segALib.s - System dependent segment initialization code */

#define _ASMLANGUAGE
#include <rtos.h>
#include <arch/asm.h>

	/* Internals */
	.globl	GTEXT(sysGDTSet)

	.text
	.balign	16

/*****************************************************************************
* sysGDTSet - Routine to be called by C program to setup GDT Segment table
*
* RETURNS:   N/A
* PROTOTYPE: void sysGDTSet(SEGDESC *baseAddr)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysGDTSet)
	movl    SP_ARG1(%esp),%eax
        lgdt    (%eax)
	movw	$0x0010,%ax	/* Offset in GDT to data segment */
	movw	%ax,%dx
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	jmp	$0x08,$flushCs
flushCs:
	ret

