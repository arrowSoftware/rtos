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

/* semLib.h - Semaphore header */

#ifndef _semLib_h
#define _semLib_h

#include <rtos/private/semLibP.h>

#define SEM_Q_MASK			0x03
#define SEM_Q_FIFO			0x00
#define SEM_Q_PRIORITY			0x01
#define SEM_DELETE_SAFE			0x04
#define SEM_EVENTSEND_ERROR_NOTIFY	0x08
#define SEM_INVERSION_SAFE              0x10

#define S_semLib_NOT_INSTALLED       (M_semLib | 0x0001)
#define S_semLib_INVALID_STATE       (M_semLib | 0x0002)
#define S_semLib_INVALID_OPTION      (M_semLib | 0x0003)
#define S_semLib_INVALID_Q_TYPE      (M_semLib | 0x0004)
#define S_semLib_INVALID_OPERATION   (M_semLib | 0x0005)
#define S_semLib_NOT_OWNER           (M_semLib | 0x0006)
#define S_semLib_INVALID_MAX_READERS (M_semLib | 0x0007)
#define S_semLib_RECURSION_LIMIT     (M_semLib | 0x0008)
#define S_semLib_LOST_LOCK           (M_semLib | 0x0009)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  SEM_EMPTY = 0,
  SEM_FULL
} SEM_B_STATE;

typedef struct semaphore SEMAHPORE;
typedef struct semaphore *SEM_ID;
typedef struct rw_semaphore *RW_SEM_ID;

/* Master semaphore */
extern STATUS semLibInit(void);
extern SEM_ID semCreate(int type, int options);
extern STATUS semInit(SEM_ID semId, int type, int options);
extern STATUS semGive(SEM_ID semId);
extern STATUS semTake(SEM_ID semId, unsigned timeout);
extern STATUS semDelete(SEM_ID semId);
extern STATUS semTerminate(SEM_ID semId);
extern STATUS semDestroy(SEM_ID semId, BOOL deallocate);
extern STATUS semInvalid(SEM_ID semId);
extern STATUS semQFlush(SEM_ID semId);
extern void semQFlushDefer(SEM_ID semId);

/* Binary semaphore */
extern STATUS semBCoreInit(SEM_ID semId, int options, SEM_B_STATE state);
extern STATUS semBLibInit(void);
extern SEM_ID semBCreate(int options, SEM_B_STATE state);
extern STATUS semBInit(SEM_ID semId, int options, SEM_B_STATE state);
extern STATUS semBGive(SEM_ID semId);
extern STATUS semBTake(SEM_ID semId, unsigned timeout);
extern void semBGiveDefer(SEM_ID semId);

/* Mutex semaphore */
STATUS semMCoreInit(SEM_ID semId, int options);
extern STATUS semMLibInit(void);
extern SEM_ID semMCreate(int options);
extern STATUS semMInit(SEM_ID semId, int options);
extern STATUS semMGive(SEM_ID semId);
extern STATUS semMTake(SEM_ID semId, unsigned timeout);

/* Counting semaphore */
extern STATUS semCCoreInit(SEM_ID semId, int options, int initialCount);
extern STATUS semCLibInit(void);
extern SEM_ID semCCreate(int options, int initialCount);
extern STATUS semCInit(SEM_ID semId, int options, int initialCount);
extern STATUS semCGive(SEM_ID semId);
extern STATUS semCTake(SEM_ID semId, unsigned timeout);
extern void semCGiveDefer(SEM_ID semId);

/* Read-write semaphore */

extern STATUS semRWLibInit (void);
extern SEM_ID semRWCreate (int options, int maxReaders);
extern STATUS semRWInit (SEM_ID semId, int options, int maxReaders);
extern STATUS semWriterTake (SEM_ID semId, unsigned timeout);
extern STATUS semReaderTake (SEM_ID semId, unsigned timeout);
extern STATUS semRWUpgrade (SEM_ID semId, unsigned timeout);
extern STATUS semRWDowngrade (SEM_ID semId);

extern CLASS_ID semClassId;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _semLib_h */

