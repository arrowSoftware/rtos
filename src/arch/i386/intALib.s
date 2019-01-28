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

/* intALib.s - Interrupt assmebly routines */

#define _ASMLANGUAGE
#include <rtos.h>
#include <arch/asm.h>
#include <arch/regs.h>

	/* Internals */
	.globl	GTEXT(intLevelSet)
	.globl	GTEXT(intLock)
	.globl	GTEXT(intUnlock)
	.globl	GTEXT(intVBRSet)

	.text
	.balign	16

/*******************************************************************************
* intLevelSet - Setup interrupt lockout level
* PROTOTYPE: int intLevelSet(int level)
*
* RETURNS: Zero
*******************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(intLevelSet)

	/* Zero return value */
	xorl	%eax, %eax
	ret

/*******************************************************************************
* intLock - Lock interrupts
* PROTOTYPE: int intLock(void)
*
* RETURNS: Interrupt lock level
*******************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(intLock)

	/* Push EFLAGS on stack */
	pushf

	/* EFLAGS & EFLAGS_IF (interrupt enable flag), return value */
	popl	%eax
	andl	$EFLAGS_IF, %eax

	/* Lock interrupts and return */
	cli
	ret

/*******************************************************************************
* intUnlock - Unlock interrupts
* PROTOTYPE: void intUnlock(int level)
*
* RETURNS: N/A
*******************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(intUnlock)

	/* Get argument */
	movl	SP_ARG1(%esp), %eax

	/* level & EFLAGS_IF (interrupt enable flag) */
	andl	$EFLAGS_IF ,%eax

	/* If EFLAGS_IF (interrupt enable flag) is set goto intUnlock0 */
	jz	intUnlock0

	/* Else enable interrupts */
	sti

intUnlock0:

	/* Return */
	ret

/*******************************************************************************
* intVBRSet - Setup interrupt table descriptor
* PROTOTYPE: void intVBRSet(FUNCPTR *baseAddress)
*
* RETURNS: N/A
*******************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(intVBRSet)

	movl	SP_ARG1(%esp), %eax
	lidt	(%eax)
	ret
