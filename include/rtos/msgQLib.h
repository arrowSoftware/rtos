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

/* msgQLib.h - Header for message queues */

#ifndef _msgQLib_h
#define _msgQLib_h

#include <tools/moduleNumber.h>

#include <rtos.h>
#include <util/qLib.h>
#include <rtos/private/msgQLibP.h>

#define MSG_Q_TYPE_MASK			0x01
#define MSG_Q_FIFO			0x00
#define MSG_Q_PRIORITY			0x01
#define MSG_Q_EVENTSEND_ERROR_NOTIFY	0x02

#define MSG_PRI_NORMAL			0
#define MSG_PRI_URGENT			1

#define S_msgQLib_NOT_INSTALLED        (M_msgQLib | 0x0001)
#define S_msgQLib_INVALID_MSG_LENGTH   (M_msgQLib | 0x0002)
#define S_msgQLib_INVALID_TIMEOUT      (M_msgQLib | 0x0003)
#define S_msgQLib_INVALID_Q_TYPE       (M_msgQLib | 0x0004)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_NODE_SIZE(msgLength)	\
	(ROUND_UP((sizeof(MSG_NODE) + msgLength), _ALLOC_ALIGN_SIZE))

typedef struct msg_q *MSG_Q_ID;

extern CLASS_ID msgQClassId;

extern STATUS msgQLibInit(void);
extern MSG_Q_ID msgQCreate(int maxMsg,
			   int maxMsgLength,
			   int options);
extern STATUS msgQInit(MSG_Q_ID msgQId,
                       int maxMsg,
                       int maxMsgLength,
                       int options,
                       void *pMsgPool);
extern STATUS msgQTerminate(MSG_Q_ID msgQId);
extern STATUS msgQDelete(MSG_Q_ID msgQId);
extern STATUS msgQDestroy(MSG_Q_ID msgQId, BOOL deallocate);
extern STATUS msgQSend(MSG_Q_ID msgQId,
		       void *buffer,
		       unsigned nBytes,
		       unsigned timeout,
		       unsigned priority);
extern int msgQReceive(MSG_Q_ID msgQId,
		       void *buffer,
		       unsigned maxBytes,
		       unsigned timeout);
extern int msgQNumMsgs(MSG_Q_ID msgQId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _msgQLib_h */

