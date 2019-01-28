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

/* semLibP.h - Private semaphore header */

#ifndef _semLibP_h
#define _semLibP_h

#include <util/classLib.h>
#include <util/qLib.h>
#include <rtos/private/eventLibP.h>

#define MAX_SEM_TYPE		4
#define SEM_TYPE_MASK		0x03
#define SEM_TYPE_BINARY		0x00
#define SEM_TYPE_MUTEX		0x01
#define SEM_TYPE_COUNTING	0x02
#define SEM_TYPE_RW             0x03

/* Mutex kernel give states */
#define SEM_M_Q_GET		0x01
#define SEM_M_SAFE_Q_FLUSH	0x02
#define SEM_M_SEND_EVENTS	0x04

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#define SEM_OWNER_GET(semId) 		((TCB_ID) semId->state.owner)
#define SEM_OWNER_SET(semId, val)	(semId->state.owner = (void *) val)
#define SEM_COUNT(semId)		(semId->state.count)

typedef struct semaphore
{
  OBJ_CORE objCore;
  unsigned char semType;
  unsigned char options;
  unsigned short recurse;
  Q_HEAD qHead;
  union {
    unsigned count;
    void *owner;
  } state;
  EVENTS_RESOURCE events;
} SEMAPHORE;

/*
 * New structures for multiple reader single writer semaphore.
 * 
 * Note that there are two extra structures instead of one.  The 'basic'
 * structure identifies the minimum information that is required.  The 'extra'
 * structure is used as an array to track information specific to each reader,
 * if required.  It is used if a non-zero number of [maxReaders] is needed.
 * A non-zero number of [maxReaders] is needed if one desires any of the
 * following multiple reader single writer semaphore options:
 *     - priority inheritance                   (not yet supported)
 *     - recursive takes for the reader(s)
 *     - upgrade reader-lock to a writer lock
 */

typedef struct sem_rw_basic {
    Q_HEAD       qHead;      /* Reader Q_HEAD */
    unsigned     maxReaders; /* Max # of simultaneous readers: 0 = no limit */
    unsigned     nReaders;   /* Current number of readers */
} SEM_RW_BASIC;

typedef struct sem_rw_extra {
    unsigned short   recurse;  /* Reader recursion count */
    unsigned char    flags;    /* Flags such as 'requested lock upgrade' */
    unsigned char    pad;      /* Padding */
    void *  owner;             /* Reader task owning semaphore */
} SEM_RW_EXTRA;

typedef struct rw_semaphore {
    SEMAPHORE     semaphore;   /* Original semaphore type */
    SEM_RW_BASIC  basic;       /* Basic reader fields */
    SEM_RW_EXTRA  extra[];     /* Extra reader fields */
} RW_SEMAPHORE;

#define  semCount  state.count
#define  semOwner  state.owner

extern OBJ_CLASS semClass;

extern FUNCPTR semGiveTable[];
extern FUNCPTR semTakeTable[];

extern STATUS semQInit (Q_HEAD * pQHead, int options);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _semLibP_h */

