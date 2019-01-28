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

/* cacheALib.s - Cache routines */

#define _ASMLANGUAGE
#include <rtos.h>
#include <arch/asm.h>
#include <arch/regs.h>

	.data

	/* Internals */
	.globl	GTEXT(cacheI386Reset)
	.globl	GTEXT(cacheI386Enable)
	.globl	GTEXT(cacheI386Unlock)
	.globl	GTEXT(cacheI386Disable)
	.globl	GTEXT(cacheI386Lock)
	.globl	GTEXT(cacheI386Clear)
	.globl	GTEXT(cacheI386Flush)

	.text
	.balign	16

/*******************************************************************************
 * cacheI386Reset - Reset cache
 *
 * RETURNS: OK
 ******************************************************************************/

	.balign	16,0x90

FUNC_LABEL(cacheI386Reset)

	movl	%cr0, %eax		/* Get contents of cr0 */
	orl	$CR0_CD, %eax		/* Set cache disable bit */
	andl	$CR0_NW_NOT, %eax	/* And no write trough mask */
	movl	%eax, %cr0		/* Store new contents to cr0 */
	wbinvd				/* Writeback and invalid cache */
	ret

/*******************************************************************************
 * cacheI386Enable - Enable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

	.balign	16,0x90

FUNC_LABEL(cacheI386Enable)
FUNC_LABEL(cacheI386Unlock)

	movl	%cr0, %eax		/* Get contents of cr0 */
	andl	$CR0_CD_NOT, %eax	/* And cache enable mask */
	andl	$CR0_NW_NOT, %eax	/* And write troguh mask */
	movl	%eax, %cr0		/* Store new contets to cr0 */
	ret

/*******************************************************************************
 * cacheI386Disable - Disable cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

	.balign	16,0x90

FUNC_LABEL(cacheI386Disable)

	movl	%cr0, %eax		/* Get contents of cr0 */
	orl	$CR0_CD, %eax		/* Set cache disable bit */
	andl	$CR0_NW_NOT, %eax	/* And write trough mask */
	movl	%eax, %cr0		/* Store new contets in cr0 */
	wbinvd				/* Writeback and invalid cache */
	ret

/*******************************************************************************
 * cacheI386Lock - Lock cache
 *
 * RETURNS: OK
 ******************************************************************************/

	.balign	16,0x90

FUNC_LABEL(cacheI386Lock)

	movl	%cr0, %eax		/* Get contents of cr0 */
	andl	$CR0_NW_NOT, %eax	/* And write trough mask */
	orl	$CR0_CD, %eax		/* Set cache disable bit */
	movl	%eax, %cr0		/* Store new contets in cr0 */
	ret

/*******************************************************************************
 * cacheI386Clear - Clear cache
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

	.balign	16,0x90

FUNC_LABEL(cacheI386Clear)
FUNC_LABEL(cacheI386Flush)

	wbinvd				/* Writeback and invalid cache */
	ret

