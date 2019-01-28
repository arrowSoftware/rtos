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

/* eventLibP.h - Event library private header */

#ifndef _eventLibP_h
#define _eventLibP_h

#define EVENTS_WAIT_MASK		0x01

#define EVENTS_SYSFLAGS_WAITING		0x01
#define EVENTS_SYSFLAGS_DELETE_RESOURCE	0x02

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

typedef struct events
{
  u_int32_t wanted;
  volatile u_int32_t received;
  u_int8_t options;
  u_int8_t sysflags;
} EVENTS;

typedef struct events_resource
{
  u_int32_t registered;
  int taskId;
  u_int8_t options;
} EVENTS_RESOURCE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _eventLibP_h */

