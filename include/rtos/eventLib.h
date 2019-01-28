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

/* eventLib.h - Event library header */

#ifndef _eventLib_h
#define _eventLib_h

#include <rtos/private/eventLibP.h>

#define EVENTS_WAIT_ALL		0x00
#define EVENTS_WAIT_ANY		0x01
#define EVENTS_RETURN_ALL	0x02
#define EVENTS_KEEP_UNWANTED	0x04
#define EVENTS_FETCH		0x80

#define EVENTS_OPTIONS_NONE	0x00
#define EVENTS_SEND_ONCE	0x01
#define EVENTS_ALLOW_OVERWRITE	0x02
#define EVENTS_SEND_IF_FREE	0x04

#define S_eventLib_NOT_INSTALLED       (M_eventLib | 0x0001)
#define S_eventLib_NULL_TASKID         (M_eventLib | 0x0002)
#define S_eventLib_SEND_ERROR          (M_eventLib | 0x0003)
#define S_eventLib_NO_EVENTS           (M_eventLib | 0x0004)
#define S_eventLib_ALREADY_REGISTERED  (M_eventLib | 0x0005)
#define S_eventLib_NOT_REGISTERED_TASK (M_eventLib | 0x0006)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <util/objLib.h>
#include <util/classLib.h>

extern STATUS eventLibInit(void);
extern STATUS eventReceive(u_int32_t events,
                    	   u_int8_t options,
                    	   unsigned timeout,
                    	   u_int32_t *pReceivedEvents);
extern STATUS eventSend(int taskId, u_int32_t events);
extern STATUS eventClear(void);
extern void eventResourceInit(EVENTS_RESOURCE *pResource);
extern STATUS eventResourceRegister(OBJ_ID objId,
				    EVENTS_RESOURCE *pResource,
				    FUNCPTR isFreeMethod,
				    u_int32_t events,
				    u_int8_t options);
extern STATUS eventResourceSend(int taskId, u_int32_t events);
extern STATUS eventResourceTerminate(EVENTS_RESOURCE *pEventResource);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _eventLib_h */

