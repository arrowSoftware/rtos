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

/* moduleLib.c - Object code module library */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <util/classLib.h>
#include <util/dllLib.h>
#include <util/cksumLib.h>
#include <rtos/semLib.h>
#include <rtos/memPartLib.h>
#include <io/pathLib.h>
#include <tools/moduleLib.h>

/* Defines */
//#define DEBUG_MOD

/* Types */
typedef struct {
  DL_NODE node;
  FUNCPTR func;
} MODULE_HOOK;

/* Imports */
IMPORT PART_ID memSysPartId;

/* Locals */
LOCAL BOOL moduleLibInstalled = FALSE;
LOCAL OBJ_CLASS moduleClass;
LOCAL DL_LIST moduleList;
LOCAL SEM_ID moduleListSem;
LOCAL SEM_ID moduleSegSem;

LOCAL BOOL moduleCreateHookInstalled = FALSE;
LOCAL DL_LIST moduleCreateHookList;
LOCAL SEM_ID moduleCreateHookSem;

LOCAL BOOL moduleSegVerify(SEGMENT_ID segId);
LOCAL BOOL moduleVerify(MODULE_ID modId, int options);
LOCAL STATUS moduleInsert(MODULE_ID modId);

/* Globals */
CLASS_ID moduleClassId = &moduleClass;
int moduleListSemOptions = SEM_DELETE_SAFE;
int moduleSegSemOptions = SEM_DELETE_SAFE;
int moduleCreateHookSemOptions = SEM_DELETE_SAFE;

/* Functions */

/*******************************************************************************
 * moduleLibInit - Inititalize module library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleLibInit(void)
{
  STATUS status;

  /* Check if installed */
  if (moduleLibInstalled)
    return OK;

  /* Inititalize list */
  dllInit(&moduleList);

  /* Inititalize list semaphore */
  moduleListSem = semMCreate(moduleListSemOptions);
  if (moduleListSem == NULL)
    return ERROR;

  /* Inititalize module segment semaphore */
  moduleSegSem = semMCreate(moduleSegSemOptions);
  if (moduleSegSem == NULL) {

    semDelete(moduleListSem);
    return ERROR;

  }

  /* Initialize module create hook list */
  dllInit(&moduleCreateHookList);

  /* Initialize moudle create hook semaphore */
  moduleCreateHookSem = semMCreate(moduleCreateHookSemOptions);
  if (moduleCreateHookSem == NULL) {

    semDelete(moduleSegSem);
    semDelete(moduleListSem);
    return ERROR;

  }

  /* Mark module create hooks as installed */
  moduleCreateHookInstalled = TRUE;

  /* Initialize object class */
  status = classInit(moduleClassId,
		     sizeof(MODULE),
		     OFFSET(MODULE, objCore),
		     memSysPartId,
		     (FUNCPTR) moduleCreate,	/* Create */
		     (FUNCPTR) moduleInit,	/* Init */
		     (FUNCPTR) moduleDestroy);	/* Destroy */

  /* Mark module library as installed */
  moduleLibInstalled = TRUE;

  return status;
}

/*******************************************************************************
 * moduleCreate - Create module
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID moduleCreate(char *name, int format, int flags)
{
  MODULE_ID modId;

  /* Allocate object */
  modId = objAlloc(moduleClassId);
  if (modId == NULL)
    return NULL;

  /* Inititalize module */
  if (moduleInit(modId, name, format, flags) != OK) {

    objFree(moduleClassId, modId);
    return NULL;

  }

  return modId;
}

/*******************************************************************************
 * moduleInit - Initialize module
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleInit(MODULE_ID modId, char *name, int format, int flags)
{
  static unsigned int moduleGroupNum = 1;

  DL_NODE *pNode;
  MODULE_ID oldMod, lastMod;
  MODULE_HOOK *createHook;

  /* Split path and filename */
  pathSplit(name, modId->path, modId->name);

  semTake(moduleListSem, WAIT_FOREVER);

  /* If module with same name */
  oldMod = moduleFindByName(modId->name);
  if (oldMod != NULL)
    oldMod->flags |= MODULE_REPLACED;

  /* Get last module */
  pNode = DLL_TAIL(&moduleList);
  if (pNode != NULL) {

    lastMod = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));
    moduleGroupNum = lastMod->group;
    moduleGroupNum++;

  }

  /* If group max not reached */
  if (moduleGroupNum < MODULE_GROUP_MAX) {

    modId->group = moduleGroupNum++;
    dllAdd(&moduleList, &modId->moduleNode);

#ifdef DEBUG_MOD
    printf("Module added at list end.\n");
#endif

  } /* End if group max not reached */

  /* Else group max reached */
  else {

    if (moduleInsert(modId) != OK) {

      fprintf(stderr, "No free group number. Abort load operation.\n");
      errnoSet(S_moduleLib_MAX_MODULES_LOADED);
      return ERROR;

    }

#ifdef DEBUG_MOD
    printf("Module inserted into list.\n");
#endif

  } /* End else group max reached */

  semGive(moduleListSem);

  /* Setup struct */
  modId->flags = flags;
  modId->format = format;
  modId->ctors = NULL;
  modId->dtors = NULL;

  /* Initialize segment list */
  dllInit(&modId->segmentList);

  /* Inititalize object class */
  objCoreInit(&modId->objCore, moduleClassId);

  semTake(moduleCreateHookSem, WAIT_FOREVER);

  /* For all module create hooks */
  for (createHook = (MODULE_HOOK *) DLL_HEAD(&moduleCreateHookList);
       createHook != NULL;
       createHook = (MODULE_HOOK *) DLL_NEXT((DL_NODE *) createHook))
    ( *createHook->func) (modId);

  semGive(moduleCreateHookSem);

  return OK;
}

/*******************************************************************************
 * moduleDestroy - Destroy module
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleDestroy(MODULE_ID modId, BOOL dealloc)
{
  /* Verify object class */
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return ERROR;

  /* Terminate object */
  objCoreTerminate(&modId->objCore);

  /* Remove from list */
  semTake(moduleListSem, WAIT_FOREVER);
  dllRemove(&moduleList, &modId->moduleNode);
  semGive(moduleListSem);

  /* Deallocate if requested */
  if (dealloc)
    objFree(moduleClassId, modId);

  return OK;
}

/*******************************************************************************
 * moduleTerminate - Terminate module
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleTerminate(MODULE_ID modId)
{
  return moduleDestroy(modId, FALSE);
}

/*******************************************************************************
 * moduleDelete - Delete module
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleDelete(MODULE_ID modId)
{
  return moduleDestroy(modId, TRUE);
}

/*******************************************************************************
 * moduleIdFigure - Get module id from module name or id
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID moduleIdFigure(void *modNameOrId)
{
  MODULE_ID modId;

  if (modNameOrId == NULL)
    return NULL;

  /* If not correct object class */
  if (OBJ_VERIFY(modNameOrId, moduleClassId) != OK) {

    /* Find by name */
    modId = moduleFindByName(modNameOrId);
    if (modId == NULL) {

      errnoSet(S_moduleLib_MODULE_NOT_FOUND);
      return NULL;

    }

    return modId;

  } /* End if not corret object class */

  return ((MODULE_ID) modNameOrId);
}

/*******************************************************************************
 * moduleIdListGet - Get a list of modules
 *
 * RETURNS: Number of items
 ******************************************************************************/

int moduleIdListGet(MODULE_ID idList[], int max)
{
  DL_NODE *pNode;
  MODULE_ID modId;
  int i;

  semTake(moduleListSem, WAIT_FOREVER);

  /* For all modules */
  for (pNode = DLL_HEAD(&moduleList), i = 0;
       (pNode != NULL) && (i < max);
       pNode = DLL_NEXT(pNode), i++ ) {

    /* Get module id */
    modId = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));

    idList[i] = modId;

  } /* End for all modules */

  semGive(moduleListSem);

  return i;
}

/*******************************************************************************
 * moduleNameGet - Get module name
 *
 * RETURNS: Pointer to module name
 ******************************************************************************/

char *moduleNameGet(MODULE_ID modId)
{
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return NULL;

  return modId->name;
}

/*******************************************************************************
 * moduleGroupGet - Get module group
 *
 * RETURNS: Group number or zero
 ******************************************************************************/

int moduleGroupGet(MODULE_ID modId)
{
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return 0;

  return ((int) modId->group);
}

/*******************************************************************************
 * moduleFlagsGet - Get module flags
 *
 * RETURNS: Flags or zero
 ******************************************************************************/

int moduleFlagsGet(MODULE_ID modId)
{
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return 0;

  return modId->flags;
}

/*******************************************************************************
 * moduleFormatGet - Get module format
 *
 * RETURNS: Format or zero
 ******************************************************************************/

int moduleFormatGet(MODULE_ID modId)
{
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return 0;

  return modId->format;
}

/*******************************************************************************
 * moduleFindByName - Find module by name
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID moduleFindByName(char *name)
{
  DL_NODE *pNode;
  MODULE_ID modId;
  char mPath[NAME_MAX], mName[PATH_MAX];

  /* Split name into directory and name */
  pathSplit(name, mPath, mName);

  semTake(moduleListSem, WAIT_FOREVER);

  /* For all modules */
  for (pNode = DLL_TAIL(&moduleList);
       pNode != NULL;
       pNode = DLL_PREV(pNode) ) {

    /* Get module id */
    modId = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));

#ifdef DEBUG_MOD
    printf("%s ", modId->name);
#endif

    /* If match */
    if (strcmp(modId->name, mName) == 0) {

#ifdef DEBUG_MOD
      printf("Match.\n");
#endif

      semGive(moduleListSem);
      return modId;

    } /* End if match */

  } /* End for all modules */

  semGive(moduleListSem);

#ifdef DEBUG_MOD
  printf("No match.\n");
#endif

  return NULL;
}

/*******************************************************************************
 * moduleFindByAndPath - Find module by name and path
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID moduleFindByNameAndPath(char *name, char *path)
{
  DL_NODE *pNode;
  MODULE_ID modId;

  semTake(moduleListSem, WAIT_FOREVER);

  /* For all modules */
  for (pNode = DLL_TAIL(&moduleList);
       pNode != NULL;
       pNode = DLL_PREV(pNode) ) {

    /* Get module id */
    modId = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));

#ifdef DEBUG_MOD
    printf("%s/%s ", modId->path, modId->name);
#endif

    /* If match */
    if ((strcmp(modId->name, name) == 0) &&
	(strcmp(modId->path, path) == 0) ){

#ifdef DEBUG_MOD
      printf("Match.\n");
#endif

      semGive(moduleListSem);
      return modId;

    } /* End if match */

  } /* End for all modules */

  semGive(moduleListSem);

#ifdef DEBUG_MOD
  printf("No match.\n");
#endif

  return NULL;
}

/*******************************************************************************
 * moduleFindByGroup - Find module by group
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID moduleFindByGroup(int group)
{
  DL_NODE *pNode;
  MODULE_ID modId;

  semTake(moduleListSem, WAIT_FOREVER);

  /* For all modules */
  for (pNode = DLL_TAIL(&moduleList);
       pNode != NULL;
       pNode = DLL_PREV(pNode) ) {

    /* Get module id */
    modId = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));

#ifdef DEBUG_MOD
    printf("%d ", (int) modId->group);
#endif

    /* If match */
    if (group == (int) modId->group) {

#ifdef DEBUG_MOD
      printf("Match.\n");
#endif

      semGive(moduleListSem);
      return modId;

    } /* End if match */

  } /* End for all modules */

  semGive(moduleListSem);

#ifdef DEBUG_MOD
  printf("No match.\n");
#endif

  return NULL;
}

/*******************************************************************************
 * moduleEach - Run function for each module
 *
 * RETURNS: MODULE_ID where if ended or NULL
 ******************************************************************************/

MODULE_ID moduleEach(FUNCPTR func, ARG arg)
{
  DL_NODE *pNode;
  MODULE_ID modId;

  semTake(moduleListSem, WAIT_FOREVER);

  /* For all modules */
  for (pNode = DLL_TAIL(&moduleList);
       pNode != NULL;
       pNode = DLL_PREV(pNode) ) {

    /* Get module id */
    modId = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));

    /* If function returns false */
    if ( ( *func) (modId, arg) == FALSE ) {

      semGive(moduleListSem);
      return modId;

    } /* End if function returns false */

  } /* End for all modules */

  semGive(moduleListSem);

  return NULL;
}

/*******************************************************************************
 * moduleInfoGet - Get module information
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleInfoGet(MODULE_ID modId, MODULE_INFO *pModInfo)
{
  /* Verify object class */
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return ERROR;

  /* Copy name */
  strcpy(pModInfo->name, modId->name);

  /* Copy group and format */
  pModInfo->format = modId->format;
  pModInfo->group = (int) modId->group;

  /* Get segment info */
  return moduleSegInfoGet(modId, &pModInfo->segInfo);
}

/*******************************************************************************
 * moduleCheck - Check all modules
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleCheck(int options)
{
  if (moduleEach((FUNCPTR) moduleVerify, (ARG) options) == NULL)
    return OK;

  return ERROR;
}

/*******************************************************************************
 * moduleCreateHookAdd - Add module create hook
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleCreateHookAdd(FUNCPTR func)
{
  MODULE_HOOK *pHook;

  if (!moduleCreateHookInstalled)
    return ERROR;

  /* Allocate module hook struct */
  pHook = (MODULE_HOOK *) malloc(sizeof(MODULE_HOOK));
  if (pHook == NULL)
    return ERROR;

  /* Setup struct */
  pHook->func = func;

  /* Insert hook in list */
  semTake(moduleCreateHookSem, WAIT_FOREVER);
  dllAdd(&moduleCreateHookList, &pHook->node);
  semGive(moduleCreateHookSem);

  return OK;
}

/*******************************************************************************
 * moduleCreateHookDelete - Delete module create hook
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleCreateHookDelete(FUNCPTR func)
{
  MODULE_HOOK *pNode;

  /* For all hooks */
  for (pNode = (MODULE_HOOK *) DLL_HEAD(&moduleCreateHookList);
       pNode != NULL;
       pNode = (MODULE_HOOK *) DLL_NEXT((DL_NODE *) pNode) ) {

    /* If hook match */
    if (pNode->func == func) {

      dllRemove(&moduleCreateHookList, (DL_NODE *) pNode);
      free(pNode);
      return OK;

    } /* End if hook match */

  } /* End for all hooks */

  errnoSet(S_moduleLib_HOOK_NOT_FOUND);

  return ERROR;
}

/*******************************************************************************
 * moduleSegAdd - Add segment to module
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleSegAdd(MODULE_ID modId,
		    int type,
		    void *location,
		    int length,
		    int flags)
{
  SEGMENT_ID segId;

  /* Verify object class */
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return ERROR;

  /* Allocate segment */
  segId = (SEGMENT_ID) malloc(sizeof(SEGMENT));
  if (segId == NULL)
    return ERROR;

  /* Setup struct */
  segId->address = location;
  segId->size = length;
  segId->type = type;
  segId->flags = flags;
  segId->checksum = checksum(location, length);

  /* Add segment to segment list */
  semTake(moduleSegSem, WAIT_FOREVER);
  dllAdd(&modId->segmentList, &segId->segmentNode);
  semGive(moduleSegSem);

  return OK;
}

/*******************************************************************************
 * moduleSegGet - Get module segment
 *
 * RETURNS: SEGMENT_ID or NULL
 ******************************************************************************/

SEGMENT_ID moduleSegGet(MODULE_ID modId)
{
  SEGMENT_ID segId;

  /* Verify object class */
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return NULL;

  /* Get segment */
  semTake(moduleSegSem, WAIT_FOREVER);
  segId = (SEGMENT_ID) dllGet(&modId->segmentList);
  semGive(moduleSegSem);

  return segId;
}

/*******************************************************************************
 * moduleSegFirst - Get first segment in module
 *
 * RETURNS: SEGMENT_ID or NULL
 ******************************************************************************/

SEGMENT_ID moduleSegFirst(MODULE_ID modId)
{
  SEGMENT_ID segId;

  /* Verify object class */
  if (OBJ_VERIFY(modId, moduleClassId) != OK)
    return NULL;

#ifdef DEBUG_MOD
  printf("Number of segments: %d\n", dllCount(&modId->segmentList));
#endif

  /* Get segment */
  semTake(moduleSegSem, WAIT_FOREVER);
  segId = (SEGMENT_ID) DLL_HEAD(&modId->segmentList);
  semGive(moduleSegSem);

  return segId;
}

/*******************************************************************************
 * moduleSegNext - Get next segment in module
 *
 * RETURNS: SEGMENT_ID or NULL
 ******************************************************************************/

SEGMENT_ID moduleSegNext(SEGMENT_ID segId)
{
  SEGMENT_ID nextSeg;

  /* Get segment */
  semTake(moduleSegSem, WAIT_FOREVER);
  nextSeg = (SEGMENT_ID) DLL_NEXT((DL_NODE *) segId);
  semGive(moduleSegSem);

  return nextSeg;
}

/*******************************************************************************
 * moduleSegEach - Execute function on all segments in module
 *
 * RETURNS: SEGMENT_ID where it ended or NULL
 ******************************************************************************/

SEGMENT_ID moduleSegEach(MODULE_ID modId, FUNCPTR func, ARG arg)
{
  SEGMENT_ID segId;

  semTake(moduleSegSem, WAIT_FOREVER);

  /* For all segments */
  for (segId = moduleSegFirst(modId);
       segId != NULL;
       segId = moduleSegNext(segId) ) {

#ifdef DEBUG_MOD
    printf("Call function on seg: %#x...", segId);
#endif

    /* If function returns false */
    if ( (*func) (segId, modId, arg) == FALSE ) {

#ifdef DEBUG_MOD
    printf("End.\n");
#endif
      semGive(moduleSegSem);
      return segId;

    } /* End if function returns false */

#ifdef DEBUG_MOD
    printf("Continue.\n");
#endif

  } /* End for all segments */

  semGive(moduleSegSem);

  return NULL;
}

/*******************************************************************************
 * moduleSegInfoGet - Get module segment info
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS moduleSegInfoGet(MODULE_ID modId, MODULE_SEG_INFO *pSegInfo)
{
  SEGMENT_ID segId;

  /* Clear */
  memset(pSegInfo, 0, sizeof(MODULE_SEG_INFO));

  semTake(moduleSegSem, WAIT_FOREVER);

  /* For all segments */
  for (segId = moduleSegFirst(modId);
       segId != NULL;
       segId = moduleSegNext(segId) ) {

    /* Select segment type */
    switch (segId->type) {

      case SEGMENT_TEXT:
        pSegInfo->textAddr = segId->address;
        pSegInfo->textSize = segId->size;
      break;

      case SEGMENT_DATA:
        pSegInfo->dataAddr = segId->address;
        pSegInfo->dataSize = segId->size;
      break;

      case SEGMENT_BSS:
        pSegInfo->bssAddr = segId->address;
        pSegInfo->bssSize = segId->size;
      break;

      default:
      break;

    } /* End select segment type */

  } /* End for all segments */

  semGive(moduleSegSem);

  return OK;
}

/*******************************************************************************
 * moduleSegVerify - Verify module segment checksum
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL moduleSegVerify(SEGMENT_ID segId)
{
  unsigned short sum;

  /* Calculate checksum */
  sum = checksum(segId->address, segId->size);

  /* Check sum */
  if (sum != segId->checksum) {

    errnoSet(S_moduleLib_BAD_CHECKSUM);
    return FALSE;

  }

  return TRUE;
}

/*******************************************************************************
 * moduleVerify - Verify module
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL moduleVerify(MODULE_ID modId, int options)
{
  SEGMENT_ID segId;
  BOOL result;

  /* Initialize locals */
  result = TRUE;

  /* Default options */
  if (options == 0)
    options = MODCHECK_TEXT;

  /* For all segments */
  for (segId = moduleSegFirst(modId);
       segId != NULL;
       segId = moduleSegNext(segId) ) {

    /* If segment check in options */
    if (segId->type & options) {

      /* If check fails */
      if (!moduleSegVerify(segId)) {

        /* If print */
        if ((options & MODCHECK_NOPRINT) == 0) {

          fprintf(stderr, "Checksum error in segment type %d, ", segId->type);
          fprintf(stderr, "module %#x (%s)\n", (int) modId, modId->name);

        } /* End if print */

        result = FALSE;

      } /* End if check fails */

    } /* End if segment check */

  } /* End for all segments */

  return result;
}

/*******************************************************************************
 * moduleInsert - Insert module
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS moduleInsert(MODULE_ID modId)
{
  DL_NODE *pNode;
  MODULE_ID currMod, nextMod;

  /* For all modules */
  for (pNode = DLL_TAIL(&moduleList);
       pNode != NULL;
       pNode = DLL_PREV(pNode) ) {

    /* Get module id */
    currMod = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));

    /* Get next module */
    pNode = DLL_NEXT(pNode);
    nextMod = (MODULE_ID) ((char *) pNode - OFFSET(MODULE, moduleNode));
    if (nextMod == NULL)
      break;

    /* If module group greater */
    if (nextMod->group > (currMod->group + 1) ) {

      /* Insert */
      dllInsert(&moduleList, &currMod->moduleNode, &modId->moduleNode);
      modId->group = currMod->group + 1;

#ifdef DEBUG_MOD
      printf("Module inserted.\n");
#endif

      break;

    } /* End if module group greater */

  } /* End for all modules */

  if (nextMod == NULL)
    return ERROR;

  return OK;
}

