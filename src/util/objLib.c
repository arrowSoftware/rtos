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

/* objLib.c - Object Library */

#include <rtos.h>
#include <rtos/errnoLib.h>
#include <util/classLib.h>
#include <util/objLib.h>
#include <rtos/memPartLib.h>
#include <stdlib.h>

/*******************************************************************************
* objAllocPad - Allocated object with padding bytes
*
* RETURNS: Pointer to object, or NULL
*******************************************************************************/

void *objAllocPad(CLASS_ID classId, unsigned nPadBytes, void **ppPad)
{
  void *pObj;

  /* Verify object */
  if (OBJ_VERIFY(classId, rootClassId) != OK) {

    errnoSet (S_classLib_INVALID_INSTANCE);
    return(NULL);
  }

  /* Allocate memory for object */
  pObj = memPartAlloc(classId->objPartId, classId->objSize + nPadBytes);

  if (pObj == NULL) {

    /* errno set by memPartAlloc() */
    return(NULL);
  }

  classId->objAllocCount++;
  if (ppPad != NULL)
    *ppPad = (void *) (((char *) pObj) + classId->objSize);

  return(pObj);
}

/*******************************************************************************
* objAlloc - Allocated an object
*
* RETURNS: Pointer to object, or NULL
*******************************************************************************/

void *objAlloc(CLASS_ID classId)
{
  return(objAllocPad(classId, 0, (void **) NULL));
}

/*******************************************************************************
* objShow - Call objects show method
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS objShow(CLASS_ID classId, OBJ_ID objId, int mode)
{
  CLASS_ID pObjClass;

  /* Resolve object class */
  pObjClass = CLASS_RESOLVE(objId, classId);

  /* Verify object */
  if (OBJ_VERIFY(pObjClass, rootClassId) != OK)
    return ERROR;

  /* If now show method specified */
  if (pObjClass->showMethod == NULL) {

    errnoSet(S_classLib_OBJ_NO_METHOD);
    return ERROR;

  } /* End if no show method specified */

  /* Call function and return */
  return ( ( *pObjClass->showMethod) (objId, mode) );
}

/*******************************************************************************
* objFree - Free object memory
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS objFree(CLASS_ID classId, void *pObj)
{
  /* Verify object */
  if (OBJ_VERIFY(classId, rootClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(NULL);
  }

  memPartFree(classId->objPartId, pObj);

  classId->objFreeCount++;

  return(OK);
}

/*******************************************************************************
* objCoreInit - Initialize object core
*
* RETURNS: N/A
*******************************************************************************/

void objCoreInit(OBJ_CORE *pObjCore, CLASS_ID pObjClass)
{
  /* Initialize object core */
  pObjCore->pObjClass = pObjClass;

  /* Increase intialized objects counter in used class */
  pObjClass->objInitCount++;
}

/*******************************************************************************
* objCoreTerminate - Terminate object core
*
* RETURNS: N/A
*******************************************************************************/

void objCoreTerminate(OBJ_CORE *pObjCore)
{
  pObjCore->pObjClass->objTerminateCount++;
  pObjCore->pObjClass = NULL;
}

