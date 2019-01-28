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

/* memLib.h - Memory library header */

#ifndef _memLib_h
#define _memLib_h

#include <tools/moduleNumber.h>
#include <rtos.h>
#include <rtos/memPartLib.h>

#define S_memLib_PAGE_SIZE_UNAVAILABLE	(M_memLib | 0x0001)

#define MEM_BLOCK_CHECK			0x10
#define MEM_ALLOC_ERROR_LOG_FLAG	0x20
#define MEM_ALLOC_ERROR_SUSPEND_FLAG	0x40
#define MEM_BLOCK_ERROR_LOG_FLAG	0x80
#define MEM_BLOCK_ERROR_SUSPEND_FLAG	0x100

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Functions */
IMPORT STATUS memLibInit(void);
IMPORT STATUS memPartOptionsSet(PART_ID partitionId, unsigned options);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _memLib_h */

