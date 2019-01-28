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

/* mmuALib.s - Memory mapping unit for 386+ assmebler routines */

#define _ASMLANGUAGE
#include <rtos.h>
#include <arch/asm.h>

        /* Internals */
	.globl	GTEXT(mmuEnable)
	.globl	GTEXT(mmuPdbrSet)
	.globl	GTEXT(mmuPdbrGet)
        .globl  GTEXT(mmuTlbFlush)

        .text
        .balign 16

/**************************************************************
* mmuEnable - Enable/Disable MMU
*
* RETURNS N/A
* PROTOTYPE: BOOL mmuEnable(BOOL enable)
**************************************************************/

	.balign	16,0x90
FUNC_LABEL(mmuEnable)
	pushfl				/* Store flags */
	cli				/* Disable interrupts */
	movl	SP_ARG1+4(%esp), %edx
	movl	%cr0,%eax
	movl	%edx,FUNC(mmuEnabled)
	cmpl	$0,%edx
	je	mmuDisable
	orl	$0x80010000,%eax	/* Set PG/WP */
	jmp	mmuEnable0

mmuDisable:
	andl	$0x7ffeffff,%eax	/* Disable PG/WP */

mmuEnable0:
	movl	%eax,%cr0
	jmp	mmuEnable1

mmuEnable1:
	movl	$0,%eax
	popfl				/* Enable interrupts */
	ret

/**************************************************************
* mmuPdbrSet - Setup page directory register
*
* RETURNS N/A
* PROTOTYPE: void mmuPdbrSet(void *transTable)
**************************************************************/

	.balign	16,0x90
FUNC_LABEL(mmuPdbrSet)
	pushfl				/* Store flags */
	cli				/* Disable interrupts */
	movl	SP_ARG1+4(%esp),%eax
	movl	(%eax),%eax
	movl	%cr3,%edx
	movl	$0xfffff000,%ecx
	andl	$0x00000fff,%edx

#if	(CPU==I80386)
	jmp	mmuPdbrSet0
#endif

	movl	$0xffffffe0,%ecx
	andl	$0x00000007,%edx

mmuPdbrSet0:
	andl	%ecx,%eax
	orl	%edx,%eax
	movl	%eax,%cr3
	jmp	mmuPdbrSet1

mmuPdbrSet1:
	popfl				/* Enable interrupts */
	ret

/**************************************************************
* mmuPdbrGet - Get page directory register
*
* RETURNS N/A
* PROTOTYPE: MMU_TRANS_TABLE *mmuPdbrGet(void)
**************************************************************/

	.balign	16,0x90
FUNC_LABEL(mmuPdbrGet)
	movl	%cr3,%eax
	movl	$0xfffff000,%edx

#if	(CPU==I80386)
	jmp	mmuPdbrGet0
#endif

	movl	$0xffffffe0,%edx

mmuPdbrGet0:
	andl	%edx,%eax
	ret

/**************************************************************
* mmuTlbFlush - Flush MMU translation table
*
* RETURNS N/A
* PROTOTYPE: void mmuTlbFlush(void)
**************************************************************/

        .balign 16,0x90
FUNC_LABEL(mmuTlbFlush)
	pushfl				/* Store flags */
	cli				/* Disable interrputs */
	movl	%cr3,%eax
	movl	%eax,%cr3		/* Flush */
	jmp	mmuTlbFlushEnd
mmuTlbFlushEnd:
	popfl				/* Enable interrupts */
	ret

