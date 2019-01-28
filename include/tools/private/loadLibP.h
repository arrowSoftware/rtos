/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2010 Surplus Users Ham Society
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

/* loadLibP.h - Load library private header*/

#ifndef _loadLibP_h
#define _loadLibP_h

#include <rtos.h>
#include <os/symbol.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef void *SYM_ADDR;

typedef struct {
  char		*symName;		/* Symbol name */
  BOOL		symMatched;		/* TRUE if syms are included */
  void		*pSymAddrBss;		/* Bss symbol address */
  SYM_TYPE	bssSymType;		/* Bss symbol type */
  void		*pSymAddrData;		/* Data symbol address */
  SYM_TYPE	dataSymType;		/* Data symbol type */
} COMMON_INFO;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _loadLibP_h */

