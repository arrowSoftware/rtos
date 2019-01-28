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

/* typesI386.h - System types */

#ifndef _typesI386_h
#define _typesI386_h

#define _QUAD_HIGHWORD		1
#define _QUAD_LOWWORD		0

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Signed types */
typedef char            	int8_t;
typedef short           	int16_t;
typedef int             	int32_t;
typedef long long       	int64_t;

/* Unsigned types */
typedef unsigned char           u_int8_t;
typedef unsigned short          u_int16_t;
typedef unsigned int            u_int32_t;
typedef unsigned long long      u_int64_t;

/* Pointer types */
typedef char    		*addr_t;

/* Misc types */
typedef unsigned int    	size_t;
typedef int			ssize_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _typesI386_h */

