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

/* classLibP.h - Private class header */

#ifndef _classLibP_h
#define _classLibP_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <rtos.h>
#include <util/classLib.h>
#include <util/private/objLibP.h>

struct mem_part;

typedef struct obj_class
{
  OBJ_CORE		objCore;
  struct mem_part	*objPartId;
  unsigned		objSize;
  unsigned		objAllocCount;
  unsigned		objFreeCount;
  unsigned		objInitCount;
  unsigned		objTerminateCount;
  int			coreOffset;

  FUNCPTR		createMethod;
  FUNCPTR		initMethod;
  FUNCPTR		destroyMethod;
  FUNCPTR		showMethod;
} OBJ_CLASS;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _classLibP_h */

