/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2009 - 2010 Surplus Users Ham Society
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

#ifndef __ERFLIB_H
#define __ERFLIB_H

#include <tools/moduleNumber.h>

#define S_erfLib_INVALID_PARAMETER   (M_erfLib | 0x0001)
#define S_erfLib_HANDLER_NOT_FOUND   (M_erfLib | 0x0002)
#define S_erfLib_TOO_MANY_USER_CATS  (M_erfLib | 0x0003)
#define S_erfLib_TOO_MANY_USER_TYPES (M_erfLib | 0x0004)

#define ERF_ASYNC_PROCESS    0x00000001
#define ERF_SYNC_PROCESS     0x00000002

extern STATUS erfLibInit (int maxCategories, int maxTypes);
extern STATUS erfHandlerRegister (int eventCategory, int eventType,
                                  void (*func)(int, int, void *, void*),
                                  void *pUserData, int flags);
extern STATUS erfHandlerUnregister (int eventCategory, int eventType,
                                    void (*func)(int, int, void *, void*),
                                    void *pUserData);
extern STATUS erfEventRaise (int eventCategory, int eventType, int processType,
                             void *pEventData, void (*freeFunc)(void *));

extern STATUS erfCategoryAllocate (int *pCategory);
extern STATUS erfTypeAllocate (int category, int *pType);

#endif
