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

/* asmI386.h - Assembler macros for architecture */

#ifndef _asmI386_h_
#define _asmI386_h_

/* Macros to make linking between c and assembler code portable
 * TRUE does not append underscores to symbol names elf/dward
 * while FALSE does that for coff/stabs */

#if	TRUE
#define FUNC(sym)	sym
#define FUNC_LABEL(sym) sym:
#else
#define FUNC(sym)	_##sym
#define FUNC_LABEL(sym) _##sym:
#endif

#define VAR(sym)	FUNC(sym)

/*#if	TRUE*/
#define GTEXT(sym) FUNC(sym)
#define GDATA(sym) FUNC(sym)
/*#else*/
/*#define GTEXT(sym) FUNC(sym) ; .type	FUNC(sym),@function
#define GDATA(sym) FUNC(sym) ; .type	FUNC(sym),@object*/
/*#endif*/

#define SP_ARG1		4
#define SP_ARG1W	6
#define SP_ARG2		8
#define SP_ARG2W	10
#define SP_ARG3		12
#define SP_ARG3W	14
#define SP_ARG4		16
#define SP_ARG5		20
#define SP_ARG6		24
#define SP_ARG7		28
#define SP_ARG8		32

#endif
