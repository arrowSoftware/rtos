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

/* vmLib.c - Virtual memory library */

#include <stdlib.h>
#include <rtos.h>
#include <arch/mmuArchLib.h>
#include <util/classLib.h>
#include <util/objLib.h>
#include <rtos/memPartLib.h>
#include <os/cacheLib.h>
#include <os/vmLib.h>

#define NUM_PAGE_STATES 256

/* Imports */
IMPORT BOOL cacheMmuAvailable;

/* Locals */
LOCAL unsigned vmStateTransTable[NUM_PAGE_STATES];
LOCAL unsigned vmMaskTransTable[NUM_PAGE_STATES];
LOCAL int vmPageSize;
LOCAL VM_CONTEXT sysVmContext;
LOCAL OBJ_CLASS vmContextClass;

/* Globals */
CLASS_ID vmContextClassId = &vmContextClass;
VM_CONTEXT *currVmContext = NULL;
char *globalPageBlockTable;

/* Globals - Maybe IMPORTS */
VM2MMU_STATE_TRANS *mmuStateTransTable;
int mmuStateTransTableSize;
int mmuPageBlockSize;
MMU_LIB_FUNCTIONS mmuLibFunctions;

/*******************************************************************************
* vmLibInit - Initialize virtual memory library
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmLibInit(int pageSize)
{
  int i, j, tableSize;
  unsigned state, mask;
  VM2MMU_STATE_TRANS *thisElement;
  unsigned vmState, mmuState, vmMask, mmuMask;

  /* Check if already installed */
  if (vmLibInfo.vmLibInstalled) {
    return(OK);
  }

  /* Check for zero pagesize */
  if (pageSize == 0) {
    return(ERROR);
  }

  /* Store page size */
  vmPageSize = pageSize;

  /* Initialize architecture independent page state table */
  for (i = 0; i < NUM_PAGE_STATES; i++) {
    state = 0;
    for (j = 0; j < mmuStateTransTableSize; j++) {
      thisElement = &mmuStateTransTable[j];
      vmState = thisElement->vmState;
      mmuState = thisElement->mmuState;
      vmMask = thisElement->vmMask;

      if ((i & vmMask) == vmState)
        state |= mmuState;
    }

    vmStateTransTable[i] = state;
  }

  /* Initialize architecture independent page state mask table */
  for (i = 0; i < NUM_PAGE_STATES; i++) {
    mask = 0;
    for (j = 0; j < mmuStateTransTableSize; j++) {
      thisElement = &mmuStateTransTable[j];
      vmMask = thisElement->vmMask;
      mmuMask = thisElement->mmuMask;

      if ((i & vmMask) == vmMask)
        mask |= mmuMask;
    }

    vmMaskTransTable[i] = mask;
  }

  /* Global page block table size */
  tableSize = (unsigned) 0x80000000 / (mmuPageBlockSize / 2);
  globalPageBlockTable = (char *) memPartAlloc(memSysPartId,
				 	tableSize *
				    	sizeof(globalPageBlockTable[0]));

  /* Initialize class */
  classInit(vmContextClassId, sizeof(VM_CONTEXT),
	    OFFSET(VM_CONTEXT, objCore),
	    memSysPartId,
	    (FUNCPTR) vmContextCreate,
	    (FUNCPTR) vmContextInit,
	    (FUNCPTR) vmContextDestroy);

  /* Install vmLibInfo functions */
  vmLibInfo.vmStateSetFunc = (FUNCPTR) vmStateSet;
  vmLibInfo.vmStateGetFunc = (FUNCPTR) vmStateGet;
  vmLibInfo.vmEnableFunc = (FUNCPTR) vmEnable;
  vmLibInfo.vmPageSizeGetFunc = (FUNCPTR) vmPageSizeGet;
  vmLibInfo.vmTranslateFunc = (FUNCPTR) vmTranslate;

  /* Enable mmu for cache library */
  cacheMmuAvailable = TRUE;
  cacheFuncsSet();

  /* Set as installed */
  vmLibInfo.vmLibInstalled = TRUE;

  return(OK);
}

/*******************************************************************************
* vmGlobalMapInit - Initialize global page map
*
* RETURNS: VM_CONTEXT_ID for global vmContext
*******************************************************************************/

VM_CONTEXT_ID vmGlobalMapInit(PHYS_MEM_DESC *pMemDescTable,
			      int numElements,
			      BOOL enable)
{
  int i;
  PHYS_MEM_DESC *thisDesc;

  if (!vmLibInfo.vmLibInstalled) {
     errnoSet (S_vmLib_NOT_INSTALLED);
     return(NULL);
  }

  /* Setup default map, copied by all new maps */
  for (i = 0; i < numElements; i++) {
    thisDesc = &pMemDescTable[i];

    if (vmGlobalMap((void *) thisDesc->vAddr,
		    (void *) thisDesc->pAddr,
		    thisDesc->len) != OK) {
      /* errno set by vmGlobalMap() */
      return(NULL);
    }
  }

  /* Intialize system virtual context */
  if (vmContextInit(&sysVmContext) != OK) {
    /* errno set by vmContextInit() */
    return(NULL);
  }

  /* Setup page states for elements */
  for (i = 0; i < numElements; i++) {

    thisDesc = &pMemDescTable[i];

    if (vmStateSet(&sysVmContext, thisDesc->vAddr, thisDesc->len,
		   thisDesc->initialMask, thisDesc->initialState) != OK) {
      /* errno set by vmStateSet() */
      return(NULL);
    }
  }

  /* Set current context */
  currVmContext = &sysVmContext;

  if (vmCurrentSet(&sysVmContext) != OK) {
    /* errno set by vmCurrentSet() */
    return(NULL);
  }

  if (enable) {
    if (vmEnable(TRUE) != OK) {
      /* errno set by vmEnable() */
      return(NULL);
    }
  }

  return(&sysVmContext);
}

/*******************************************************************************
* vmContextCreate - Create virtual memory context
*
* RETURNS: VM_CONTEXT_ID virtual memory context
*******************************************************************************/

VM_CONTEXT_ID vmContextCreate(void)
{
  VM_CONTEXT_ID context;

  if (!vmLibInfo.vmLibInstalled) {
     errnoSet (S_vmLib_NOT_INSTALLED);
     return(NULL);
  }

  /* Allocate object */
  context = (VM_CONTEXT *) objAlloc(vmContextClassId);

  if (context == NULL) {

    /* errno set by objAlloc() */
    return(NULL);
  }

  /* Initialize structure */
  if (vmContextInit(context) != OK) {

    /* errno set by vmContextInit() */
    objFree(vmContextClassId, context);
    return(NULL);
  }

  return(context);
}

/*******************************************************************************
* vmContextInit - Initialize virtual memory context
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmContextInit(VM_CONTEXT_ID context)
{
  if (!vmLibInfo.vmLibInstalled) {
     errnoSet (S_vmLib_NOT_INSTALLED);
     return(ERROR);
  }

  objCoreInit(&context->objCore, vmContextClassId);

  context->mmuTransTable =
	  (*mmuLibFunctions.mmuTransTableCreate)(vmPageSize);
  if (context->mmuTransTable == NULL)
    return(ERROR);

  return(OK);
}

/*******************************************************************************
* vmContextDestroy - Destroy virtual memory context
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmContextDestroy(VM_CONTEXT_ID context)
{
  if (!vmLibInfo.vmLibInstalled) {
     errnoSet (S_vmLib_NOT_INSTALLED);
     return(ERROR);
  }

  if (context == NULL) {
    errnoSet (S_vmLib_NULL_CONTEXT_ID);
    return(ERROR);
  }

  /* Verify object class */
  if (OBJ_VERIFY(context, vmContextClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  /* Terminate object */
  objCoreTerminate(&context->objCore);

  if ( ((*mmuLibFunctions.mmuTransTableDestroy)(context->mmuTransTable)) != OK)
    return(ERROR);

  /* Free datastructure */
  memPartFree(memSysPartId, context);

  return(OK);
}

/*******************************************************************************
* vmStateSet - Setup pages states
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmStateSet(VM_CONTEXT_ID context,
		  void *vAddr,
                  int len,
                  unsigned mask,
		  unsigned state)
{
  int pageSize;
  char *thisPage;
  unsigned numBytesDone = 0;
  unsigned mmuState, mmuMask;

  if (!vmLibInfo.vmLibInstalled) {
     errnoSet (S_vmLib_NOT_INSTALLED);
     return(ERROR);
  }

  /* Check if context should be current */
  if (context == NULL)
    context = currVmContext;

  /* Verify object class */
  if (OBJ_VERIFY(context, vmContextClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  if (state > NUM_PAGE_STATES) {
    errnoSet (S_vmLib_INVALID_STATE);
    return(ERROR);
  }

  if (mask > NUM_PAGE_STATES) {
    errnoSet (S_vmLib_INVALID_STATE_MASK);
    return(ERROR);
  }

  /* Get mmu state */
  mmuState = vmStateTransTable[state];
  mmuMask = vmMaskTransTable[mask];

  /* Setup states */
  thisPage = (char *) vAddr;
  pageSize = vmPageSize;

  while (numBytesDone < len) {

    if ( ((*mmuLibFunctions.mmuStateSet)(context->mmuTransTable, thisPage,
			mmuMask, mmuState)) != OK)
      return(ERROR);

    /* Move to next page */
    thisPage += pageSize;
    numBytesDone += pageSize;
  }

  return(OK);
}

/*******************************************************************************
* vmStateGet - Get pages states
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmStateGet(VM_CONTEXT_ID context,
		  void *vAddr,
		  unsigned *pState)
{
  unsigned mmuState;
  int i;
  VM2MMU_STATE_TRANS *thisState;
  unsigned mmuTMask, mmuTState, vmTState;

  if (!vmLibInfo.vmLibInstalled) {
    errnoSet (S_vmLib_NOT_INSTALLED);
    return(ERROR);
  }

  /* Check if context should be current */
  if (context == NULL)
    context = currVmContext;

  /* Verify object class */
  if (OBJ_VERIFY(context, vmContextClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  *pState = 0;

  if ( ((*mmuLibFunctions.mmuStateGet)(context->mmuTransTable,
				       vAddr, &mmuState)) != OK)
     return(ERROR);

  /* Translate state */
  for (i = 0; i < mmuStateTransTableSize; i++) {
    thisState = &mmuStateTransTable[i];
    mmuTMask = thisState->mmuMask;
    mmuTState = thisState->mmuState;
    vmTState = thisState->vmState;

    if ((mmuState & mmuTMask) == mmuTState)
      *pState |= vmTState;
  }

  return(OK);
}

/*******************************************************************************
* vmMap - Map physical page(s)
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmMap(VM_CONTEXT_ID context,
	     void *vAddr,
	     void *pAddr,
	     unsigned len)
{
  int pageSize;
  char *thisVirtPage;
  char *thisPhysPage;
  int numBytesDone;

  if (!vmLibInfo.vmLibInstalled) {
    errnoSet (S_vmLib_NOT_INSTALLED);
     return(ERROR);
  }

  /* Check if context should be current */
  if (context == NULL)
    context = currVmContext;

  /* Verify object class */
  if (OBJ_VERIFY(context, vmContextClassId) != OK) {

    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  pageSize = vmPageSize;

  /* Check if virtual address is ok */
  if (((unsigned) vAddr % pageSize) != 0) {
    errnoSet (S_vmLib_VIRT_ADDR_NOT_ALIGNED);
    return(ERROR);
  }

  /* Check if page size is ok */
  if ((len % pageSize) != 0) {
    errnoSet (S_vmLib_PAGE_SIZE_NOT_ALIGNED);
    return(ERROR);
  }

  /* Map page(s) */
  thisVirtPage = (char *) vAddr;
  thisPhysPage = (char *) pAddr;
  numBytesDone = 0;

  while (numBytesDone < len) {

    /* Setup page */
    if ( ((*mmuLibFunctions.mmuPageMap)(context->mmuTransTable,
			 		thisVirtPage,
			 		thisPhysPage)) == ERROR)
      return(ERROR);

    /* Set page state */
    if (vmStateSet(context, thisVirtPage, pageSize,
		   VM_STATE_MASK_VALID|
		   VM_STATE_MASK_WRITABLE|
		   VM_STATE_MASK_CACHEABLE,
		   VM_STATE_VALID|
		   VM_STATE_WRITABLE|
		   VM_STATE_CACHEABLE) != OK) {
      /* errno set by vmStateSet() */
      return(ERROR);
    }

    /* Update */
    thisVirtPage += pageSize;
    thisPhysPage += pageSize;
    numBytesDone += pageSize;
  }

  return(OK);
}

/*******************************************************************************
* vmGlobalMap - Map physical page(s) to global page map
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmGlobalMap(void *vAddr,
		   void *pAddr,
		   unsigned len)
{
  int pageSize;
  char *thisVirtPage;
  char *thisPhysPage;
  int numBytesDone;

  if (!vmLibInfo.vmLibInstalled) {
    errnoSet (S_vmLib_NOT_INSTALLED);
    return(ERROR);
  }

  pageSize = vmPageSize;

  /* Check if virtual address is ok */
  if (((unsigned) vAddr % pageSize) != 0) {
    errnoSet (S_vmLib_VIRT_ADDR_NOT_ALIGNED);
    return(ERROR);
  }

  /* Check if page size is ok */
  if ((len % pageSize) != 0) {
    errnoSet (S_vmLib_PAGE_SIZE_NOT_ALIGNED);
    return(ERROR);
  }

  /* Map page(s) */
  thisVirtPage = (char *) vAddr;
  thisPhysPage = (char *) pAddr;
  numBytesDone = 0;

  while (numBytesDone < len) {

    /* Setup page */
    if ( ((*mmuLibFunctions.mmuGlobalPageMap)(thisVirtPage,
			 		      thisPhysPage)) == ERROR)
      return(ERROR);

    /* Mark page as used */
    globalPageBlockTable[(unsigned) thisVirtPage / mmuPageBlockSize] = TRUE;

    /* Update */
    thisVirtPage += pageSize;
    thisPhysPage += pageSize;
    numBytesDone += pageSize;
  }

  return(OK);
}

/*******************************************************************************
* vmCurrentSet - Set current page map
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmCurrentSet(VM_CONTEXT_ID context)
{
  if (!vmLibInfo.vmLibInstalled) {
    errnoSet (S_vmLib_NOT_INSTALLED);
    return(ERROR);
  }

  if (context == NULL) {
    errnoSet (S_vmLib_NULL_CONTEXT_ID);
    return(ERROR);
  }

  /* Verify object class */
  if (OBJ_VERIFY(context, vmContextClassId) != OK) {

    /* errno set by OBJ_VERIFY () */
    return(ERROR);
  }

  /* Set as current */
  currVmContext = context;
  (*mmuLibFunctions.mmuCurrentSet)(context->mmuTransTable);

  return(OK);
}

/*******************************************************************************
* vmEnable - Enable MMU
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmEnable(BOOL enable)
{
  if (!vmLibInfo.vmLibInstalled) {
    errnoSet (S_vmLib_NOT_INSTALLED);
     return(ERROR);
  }

  return((*mmuLibFunctions.mmuEnable)(enable));
}

/*******************************************************************************
* vmPageSizeGet - Get virual memory page size
*
* RETURNS: Page size
*******************************************************************************/

int vmPageSizeGet(void)
{
  return vmPageSize;
}

/*******************************************************************************
* vmTranslate - Translate from virtual to physical memory address
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS vmTranslate(VM_CONTEXT_ID context, void *vAddr, void **pAddr)
{
  if (!vmLibInfo.vmLibInstalled) {
    errnoSet (S_vmLib_NOT_INSTALLED);
    return(ERROR);
  }

  /* Check if context should be current */
  if (context == NULL)
    context = currVmContext;

  /* Verify object class */
  if (OBJ_VERIFY(context, vmContextClassId) != OK) {

    /* errno set by OBJ_VERIFY () */
    return(ERROR);
  }

  return((*mmuLibFunctions.mmuTranslate)(context->mmuTransTable, vAddr, pAddr));
}

