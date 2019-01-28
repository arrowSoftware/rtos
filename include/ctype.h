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

/* ctype.h - Character types for the C library */

#ifndef _ctype_h
#define _ctype_h

#include <rtos.h>

/* Defines */
#define _C_UPPER		0x01
#define _C_LOWER		0x02
#define _C_NUMBER		0x04
#define _C_WHITE_SPACE		0x08
#define _C_PUNCT		0x10
#define _C_CONTROL		0x20
#define _C_HEX_NUMBER		0x40
#define _C_B			0x80

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Imports */
IMPORT const unsigned char *__ctype;

/* Macros */
#define __isascii(c)		((unsigned) (c) <= 177)
#define __toascii(c)		((c) & 177)
#define __isalpha(c)		(__ctype[c] & (_C_UPPER | _C_LOWER))
#define __isupper(c)		(__ctype[c] & (_C_UPPER))
#define __islower(c)		(__ctype[c] & (_C_LOWER))
#define __isdigit(c)		(__ctype[c] & (_C_NUMBER))
#define __isxdigit(c)		(__ctype[c] & (_C_HEX_NUMBER))
#define __isspace(c)		(__ctype[c] & (_C_WHITE_SPACE | _C_CONTROL))
#define __ispunct(c)		(__ctype[c] & (_C_PUNCT))
#define __isalnum(c)		(__ctype[c] & (_C_UPPER | _C_LOWER | _C_NUMBER))
#define __isprint(c)		(__ctype[c] & (_C_PUNCT | _C_UPPER | _C_LOWER |\
					_C_WHITE_SPACE | _C_NUMBER))
#define __isgraph(c)		(__ctype[c] & (_C_PUNCT | _C_UPPER | _C_LOWER |\
					_C_NUMBER))
#define __iscntrl(c)		(__ctype[c] & (_C_CONTROL | _C_B))
#define __toupper(c)		(					       \
				 (('a' <= (c)) && ((c) <= 'z')) ?	       \
				  ((c) - 'a' + 'A') :			       \
				  (c)					       \
				)
#define __tolower(c)		(					       \
				 (('A' <= (c)) && ((c) <= 'Z')) ?	       \
				  ((c) - 'A' + 'a') :			       \
				  (c)					       \
				)

/* Functions */
extern int isalnum(int c);
extern int isalpha(int c);
extern int iscntrl(int c);
extern int isdigit(int c);
extern int isgraph(int c);
extern int islower(int c);
extern int isprint(int c);
extern int ispunct(int c);
extern int isspace(int c);
extern int isupper(int c);
extern int isxdigit(int c);
extern int tolower(int c);
extern int toupper(int c);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ctype_h */

