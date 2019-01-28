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

/* vmLibP.h.h - Private virual memory header */

#ifndef _vmLibP_h
#define _vmLibP_h

#include <arch/mmuArchLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  unsigned vmMask;
  unsigned mmuMask;
  unsigned vmState;
  unsigned mmuState;
} VM2MMU_STATE_TRANS;

typedef struct
{
  FUNCPTR mmuLibInit;
  MMU_TRANS_TABLE_ID (*mmuTransTableCreate) ();
  FUNCPTR mmuTransTableDestroy;
  FUNCPTR mmuEnable;
  FUNCPTR mmuStateSet;
  FUNCPTR mmuStateGet;
  FUNCPTR mmuPageMap;
  FUNCPTR mmuGlobalPageMap;
  FUNCPTR mmuTranslate;
  VOIDFUNCPTR mmuCurrentSet;
} MMU_LIB_FUNCTIONS;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _vmLibP_h */

