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

/* rtosTypes.h - Os specific types */

#ifndef _rtosTypes_h
#define _rtosTypes_h

/* Define boolean types */
#if     !defined(FALSE) || (FALSE!=0)
#define FALSE 0
#endif

#if     !defined(TRUE) || (TRUE!=1)
#define TRUE 1
#endif

#define NONE	(-1)
#define EOS	'\0'
#define EOF	EOS

/* Define return types */
#define OK      0
#define ERROR   (-1)

/* Define wait type */
#define WAIT_NONE			0x00000000
#define WAIT_FOREVER			0xffffffff

#ifndef _ASMLANGUAGE

/* Define scope types */
#define FAST    register
#define IMPORT  extern
#define LOCAL   static

/* Macros */
#define OFFSET(structure, member)	/* byte offset to member */\
		((int) &(((structure *) 0 ) -> member))

#define NELEMENTS(array)		/* Number of elements in array */\
		(sizeof (array) / sizeof ((array) [0]))

#define ROUND_UP(x, align)	(((int)(x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align)	((int)(x) & ~(align - 1))
#define ALIGNED(x, align)	(((int)(x) & (align - 1)) == 0)

#define MEM_ROUND_UP(x)		ROUND_UP(x, _ALLOC_ALIGN_SIZE)
#define MEM_ROUND_DOWN(x)	ROUND_DOWN(x, _ALLOC_ALIGN_SIZE)
#define STACK_ROUND_UP(x)	ROUND_UP(x, _STACK_ALIGN_SIZE)
#define STACK_ROUND_DOWN(x)	ROUND_DOWN(x, _ALLOC_ALIGN_SIZE)
#define MEM_ALIGNED(x)		ALIGNED(x, ALLOC_ALIGNED_SIZE)

#define MSB(x)			( ((x) >> 8) & 0xff )
#define LSB(x)			( (x) & 0xff )

#ifdef __cplusplus
extern "C" {
#endif

typedef int			BOOL;
typedef int			STATUS;
typedef int			ARGINT;

typedef void			VOID;

#ifdef __cplusplus
typedef int			(*FUNCPTR) (...);
typedef void			(*VOIDFUNCPTR) (...);
typedef double			(*DBLFUNCPTR) (...);
typedef float			(*FLTFUNCPTR) (...);
#else
typedef int			(*FUNCPTR) ();
typedef void			(*VOIDFUNCPTR) ();
typedef double			(*DBLFUNCPTR) ();
typedef float			(*FLTFUNCPTR) ();
#endif

typedef void*			ARG;

#include <sys/types.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _rtosTypes_h */

