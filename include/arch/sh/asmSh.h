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

/* asmSh.h - SuperH assebler macros */

#ifndef _asmSh_h
#define _asmSh_h

/* Macros to make linking between c and assembler code portable
 * TRUE does not append underscores to symbol names elf/dwarf
 * while FALSE does that for coff/stabs */

#ifdef __STDC__
#define FUNC(sym)			_##sym
#define FUNC_LABEL(sym)			_##sym:
#else
#define FUNC(sym)			_/**/sym
#define FUNC_LABEL(sym)			_/**/sym:
#endif

#define GTEXT(sym)			FUNC(sym)
#define GDATA(sym)			FUNC(sym)

#define ALIGN_TEXT			4
#define ALIGN_DATA			2

#endif /* _asmSh_h */

