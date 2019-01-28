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

/* asmM68k.h - */

#ifndef _asmM68k_h
#define _asmM68k_h

#ifdef __ELF__
#define FUNC(sym)	sym
#define FUNC_LABEL(sym) sym:
#else
#define FUNC(sym)	_##sym
#define FUNC_LABEL(sym) _##sym:
#endif

#define GTEXT(sym)			FUNC(sym)
#define GDATA(sym)			FUNC(sym)

#endif /* _asmM68k_h */

