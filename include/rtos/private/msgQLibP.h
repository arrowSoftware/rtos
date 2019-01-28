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

/* msgQLibP.h - Private header for message queues */

#ifndef _msgQLibP_h
#define _msgQLibP_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <util/objLib.h>
#include <util/classLib.h>
#include <util/qMsgLib.h>
#include <rtos/eventLib.h>

typedef struct msg_q
{
  OBJ_CORE objCore;
  Q_MSG_HEAD msgQ;
  Q_MSG_HEAD freeQ;
  int options;
  int maxMsg;
  int maxMsgLength;
  int sendTimeouts;
  int reciveTimeouts;
  EVENTS_RESOURCE events;
} MSG_Q;

typedef struct
{
  Q_MSG_NODE node;
  int msgLength;
} MSG_NODE;

#define MSG_NODE_DATA(pNode)	(((char *)pNode) + sizeof(MSG_NODE))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _msgQLibP_h */

