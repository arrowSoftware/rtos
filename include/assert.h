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

/* assert.h - */

#ifndef _assert_h
#define _assert_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

extern void assertLibInit(void);

/* Macros */
#define __STRING(x)	#x

#define assert(expr)							       \
  ((void) ((expr) ? ((void) 0) :					       \
	  __assert(__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__)))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _assert_h */

