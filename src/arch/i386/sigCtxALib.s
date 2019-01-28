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

/* sigCtxALib.s - Support for software signals */

#define _ASMLANGUAGE
#include <rtos.h>
#include <arch/asm.h>
#include <arch/regs.h>

	/* Imports */
	.globl	GDATA(sysCsSuper)

	/* Globals */
	.globl	GTEXT(sigsetjmp)
	.globl	GTEXT(setjmp)
	.globl	GTEXT(_sigCtxSave)
	.globl	GTEXT(_sigCtxLoad)

	/* Locals */

	.text
	.balign	16

/*****************************************************************************
* setjmp - Set non-local goto and possibility to store signal mask
*
* RETURNS: value from longjmp or 0
*****************************************************************************/

FUNC_LABEL(sigsetjmp)

	pushl	SP_ARG2(%esp)
	pushl	SP_ARG1+4(%esp)
	call	FUNC(_setjmpSetup)
	addl	$8, %esp
	jmp	FUNC(_sigCtxSave)

/*****************************************************************************
* setjmp - Set non-local goto
*
* RETURNS: value from longjmp or 0
*****************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(setjmp)

	pushl	$1
	pushl	8(%esp)
	call	FUNC(_setjmpSetup)
	addl	$8, %esp

	/* FALLTROUGH */

/*****************************************************************************
* _sigCtxSave - Save current task context
*
* RETURNS: 0
*****************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(_sigCtxSave)

	movl	SP_ARG1(%esp), %eax
	movl	(%esp), %edx
	movl	%edx, 0x24(%eax)
	pushfl
	popl	0x20(%eax)

	/* Save all registers */
	movl	%edi, 0x00(%eax)
	movl	%esi, 0x04(%eax)
	movl	%ebp, 0x08(%eax)
	movl	%ebx, 0x10(%eax)
	movl	%edx, 0x14(%eax)
	movl	%ecx, 0x18(%eax)
	movl	%eax, 0x1c(%eax)
	movl	%esp, %edx
	addl	$4, %edx
	movl	%edx, 0x0c(%eax)

	/* Return 0 */
	xorl	%eax, %eax
	ret

/*****************************************************************************
* _sigCtxLoad - Load task context
*
* RETURNS: N/A
*****************************************************************************/

	.balign	16, 0x90

FUNC_LABEL(_sigCtxLoad)

	movl	SP_ARG1(%esp), %eax

	/* Load registers */
	movl	0x00(%eax), %edi
	movl	0x04(%eax), %esi
	movl	0x08(%eax), %ebp
	movl	0x10(%eax), %ebx
	movl	0x18(%eax), %ecx

	/* LOCK INTERRUPTS */
	cli

	/* Load stack pointer */
	movl	0x0c(%eax), %esp

	/* Push EFLAGS, task CS and PC */
	pushl	0x20(%eax)
	pushl	FUNC(sysCsSuper)
	pushl	0x24(%eax)

	/* Load remaining registers */
	movl	0x14(%eax), %edx
	movl	0x1c(%eax), %eax

	/* UNLOCK INTERRUPTS and return */
	iret

