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

/* classLib.h - Class library header */

#ifndef _classLib_h
#define _classLib_h

#include <tools/moduleNumber.h>

#define S_classLib_NOT_INSTALLED     (M_classLib | 0x0001)
#define S_classLib_ID_ERROR          (M_classLib | 0x0002)
#define S_classLib_NO_CLASS_DESTROY  (M_classLib | 0x0003)
#define S_classLib_INVALID_INSTANCE  (M_classLib | 0x0004)
#define S_classLib_OBJ_NO_METHOD     (M_classLib | 0x0005)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif


typedef struct obj_class *CLASS_ID;

extern CLASS_ID rootClassId;

#include <rtos.h>
#include <util/objLib.h>
#include <util/private/classLibP.h>

/* Structs */
struct mem_part;

/* Functions */

IMPORT STATUS classLibInit(void);
IMPORT CLASS_ID classCreate(unsigned objSize,
			    int coreOffset,
			    struct mem_part *partId,
			    FUNCPTR createMethod,
			    FUNCPTR initMethod,
			    FUNCPTR destroyMethod);
IMPORT STATUS classInit(OBJ_CLASS *pObjClass,
			unsigned objSize,
			int coreOffset,
			struct mem_part *partId,
			FUNCPTR createMethod,
			FUNCPTR initMethod,
			FUNCPTR destroyMethod);
IMPORT STATUS classShowConnect(CLASS_ID classId, FUNCPTR showMethod);
IMPORT STATUS classDestroy(CLASS_ID classId);

IMPORT CLASS_ID classRootId;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASMLANGUAGE */

#endif /* _classLib_h */

