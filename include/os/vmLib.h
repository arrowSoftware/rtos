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

/* vmLib.h - Virtual memory library */

#ifndef _vmLib_h
#define _vmLib_h

#include <tools/moduleNumber.h>

#include <rtos.h>
#include <util/objLib.h>
#include <util/classLib.h>
#include <os/private/vmLibP.h>

#define S_vmLib_NOT_INSTALLED          (M_vmLib | 0x0001)
#define S_vmLib_ZERO_PAGE_SIZE         (M_vmLib | 0x0002)
#define S_vmLib_NULL_CONTEXT_ID        (M_vmLib | 0x0003)
#define S_vmLib_INVALID_STATE          (M_vmLib | 0x0004)
#define S_vmLib_INVALID_STATE_MASK     (M_vmLib | 0x0005)
#define S_vmLib_VIRT_ADDR_NOT_ALIGNED  (M_vmLib | 0x0006)
#define S_vmLib_PAGE_SIZE_NOT_ALIGNED  (M_vmLib | 0x0006)

/* State masks */
#define VM_STATE_MASK_VALID		0x03
#define VM_STATE_MASK_WRITABLE		0x0c
#define VM_STATE_MASK_CACHEABLE		0x30
#define VM_STATE_MASK_MEM_COHERENCY	0x40
#define VM_STATE_MASK_GUARDED		0x80

/* States */
#define VM_STATE_VALID			0x01
#define VM_STATE_NOT_VALID		0x00
#define VM_STATE_WRITABLE		0x04
#define VM_STATE_NOT_WRITABLE		0x00
#define VM_STATE_CACHEABLE		0x10
#define VM_STATE_NOT_CACHEABLE		0x00
#define VM_STATE_MEM_COHERENCY		0x40
#define VM_STATE_NOT_MEM_COHERENCY	0x00
#define VM_STATE_GUARDED		0x80
#define VM_STATE_NOT_GUARDED		0x00

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  void *vAddr;
  void *pAddr;
  unsigned len;
  unsigned initialMask;
  unsigned initialState;
} PHYS_MEM_DESC;

typedef struct
{
  OBJ_CORE objCore;
  MMU_TRANS_TABLE_ID mmuTransTable;
} VM_CONTEXT;

typedef struct {
  BOOL vmLibInstalled;
  BOOL protectTextSegs;
  FUNCPTR vmStateSetFunc;
  FUNCPTR vmStateGetFunc;
  FUNCPTR vmEnableFunc;
  FUNCPTR vmPageSizeGetFunc;
  FUNCPTR vmTranslateFunc;
  FUNCPTR vmTextProtectFunc;
} VM_LIB_INFO;

/*******************************************************************************
 * VM_STATE_SET - Set virtual memory state
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

#define VM_STATE_SET(context, addr, len, stateMask, state)		       \
  ( (vmLibInfo.vmStateSetFunc == NULL) ? (ERROR) :			       \
    ( (*vmLibInfo.vmStateSetFunc) ((context), (addr), (len), (state)) ) )

/*******************************************************************************
 * VM_STATE_GET - Get virtual memory state
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

#define VM_STATE_GET(context, addr, state)				       \
  ( (vmLibInfo.vmStateGetFunc == NULL) ? (ERROR) :			       \
    ( (*vmLibInfo.vmStateGetFunc) ((context), (addr), (state)) ) )

/*******************************************************************************
 * VM_ENABLE - Enable virtual memory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

#define VM_ENABLE(enable)						       \
  ( (vmLibInfo.vmEnableFunc == NULL) ? (ERROR) :			       \
    ( (*vmLibInfo.vmEnableFunc) ((enable)) ) )

/*******************************************************************************
 * VM_PAGE_SIZE_GET - Get virtual memory page size
 *
 * RETURNS: Page size
 ******************************************************************************/

#define VM_PAGE_SIZE_GET()						       \
  ( (vmLibInfo.vmPageSizeGetFunc == NULL) ? (ERROR) :			       \
    ( (*vmLibInfo.vmPageSizeGetFunc) () ) )

/*******************************************************************************
 * VM_TRANSLATE - Translate between virtual and physical address
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

#define VM_TRANSLATE(context, vAddr, pAddr)				       \
  ( (vmLibInfo.vmTranslateFunc == NULL) ? (ERROR) :			       \
    ( (*vmLibInfo.vmTranslateFunc) ((context), (vAddr), (pAddr)) ) )

/*******************************************************************************
 * VM_TEXT_PAGE_PROTECT - Write protext text page
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

#define VM_TEXT_PAGE_PROTECT(addr, protect)				       \
  ( (vmLibInfo.vmTextProtectFunc == NULL) ? (ERROR) :			       \
    ( (*vmLibInfo.vmTextProtectFunc) ((addr), (protect)) ) )

typedef VM_CONTEXT *VM_CONTEXT_ID;
IMPORT CLASS_ID vmContextClassId;
IMPORT VM_LIB_INFO vmLibInfo;

extern STATUS vmLibInit(int pageSize);
extern VM_CONTEXT_ID vmGlobalMapInit(PHYS_MEM_DESC *pMemDescTable,
                                     int numElements,
                                     BOOL enable);
extern VM_CONTEXT_ID vmContextCreate(void);
extern STATUS vmContextInit(VM_CONTEXT_ID context);
extern STATUS vmContextDestroy(VM_CONTEXT_ID context);
extern STATUS vmStateSet(VM_CONTEXT_ID context,
                  	 void *vAddr,
                  	 int len,
                  	 unsigned mask,
                  	 unsigned state);
extern STATUS vmStateGet(VM_CONTEXT_ID context,
                         void *vAddr,
                         unsigned *pState);
extern STATUS vmGlobalMap(void *vAddr,
			  void *pAddr,
			  unsigned len);
extern STATUS vmMap(VM_CONTEXT_ID context,
             	    void *vAddr,
             	    void *pAddr,
             	    unsigned len);
extern STATUS vmCurrentSet(VM_CONTEXT_ID context);
extern STATUS vmEnable(BOOL enable);
extern int vmPageSizeGet(void);
extern STATUS vmTranslate(VM_CONTEXT_ID context, void *vAddr, void **pAddr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _vmLib_h */

