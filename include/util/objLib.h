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

/* objLib.h - Object Library header */

#ifndef _objLib_h
#define _objLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include <tools/moduleNumber.h>

#include <rtos.h>
#include <util/classLib.h>
#include <util/private/objLibP.h>
#include <rtos/errnoLib.h>

/* defines */

#define S_objLib_NOT_INSTALLED  (M_objLib | 0x0001)
#define S_objLib_ID_ERROR       (M_objLib | 0x0002)
#define S_objLib_UNAVAILABLE    (M_objLib | 0x0003)
#define S_objLib_DELETED        (M_objLib | 0x0004)
#define S_objLib_TIMEOUT        (M_objLib | 0x0005)

/* typedefs */

typedef struct obj_core *OBJ_ID;

/* macros */

/*******************************************************************************
 * CLASS_TO_OBJ_ID - Get object core from class object
 *
 * RETURNS: Pointer to object core
 ******************************************************************************/

#define CLASS_TO_OBJ_ID(objId, classId)					       \
  (OBJ_ID) ( (int) (objId) + (classId)->coreOffset )

/*******************************************************************************
 * CLASS_RESOLVE - Resolve class
 *
 * RETURNS: Class id or pointer to object core
 ******************************************************************************/

#define CLASS_RESOLVE(objId, classId)					       \
  ( (CLASS_TO_OBJ_ID(objId, classId))->pObjClass == (classId) )		       \
    ? (classId)								       \
    : (objId)->pObjClass

/*******************************************************************************
* OBJ_VERIFY - Verify object class
*
* RETURNS: OK or ERROR
*******************************************************************************/

#define OBJ_VERIFY(objId, classId)					       \
 ( (CLASS_TO_OBJ_ID(objId, classId))->pObjClass == (classId) )		       \
   ? OK									       \
   : (errno = S_objLib_ID_ERROR, ERROR)

/* functions */

IMPORT void* objAllocPad(CLASS_ID classId, unsigned nPadBytes, void **pPad);
IMPORT void* objAlloc(CLASS_ID classId);
IMPORT STATUS objShow(CLASS_ID classId, OBJ_ID objId, int mode);
IMPORT STATUS objFree(CLASS_ID classId, void *pObj);
IMPORT void objCoreInit(OBJ_CORE *pObjCore, CLASS_ID pObjClass);
IMPORT void objCoreTerminate(OBJ_CORE *pObjCore);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASMLANGUAGE */

#endif /* _objLib_h */
