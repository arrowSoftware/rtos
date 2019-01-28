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

/* msgQShow.h - Message queue info show header */

#ifndef _msgQShow_h
#define _msgQShow_h

#include <rtos.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int numMsg;			/* Output: Number of messages */
  int numTask;			/* Output: Number of tasks waiting */
  int sendTimeouts;		/* Output: Number of send timeouts */
  int reciveTimeouts;		/* Output: Number of receive timeouts */
  int options;			/* Output: Options */
  int maxMsg;			/* Output: Maximum messages */
  int maxMsgLength;		/* Output: Max byte length of each message */

  int taskIdListMax;		/* Input: Max tasks for taskIdList */
  int *taskIdList;		/* Input: Pointer to task id list */
  int msgListMax;		/* Input: Max messages for message list */
  char **msgPtrList;		/* Input: Pointer to message list */
  int *msgLengthList;		/* Input: Pointer to num messages in list */
} MSG_Q_INFO;

IMPORT STATUS msgQShowInit(void);
IMPORT STATUS msgQInfoGet(MSG_Q_ID msgQId, MSG_Q_INFO *pInfo);
IMPORT STATUS msgQShow(MSG_Q_ID msgQId, int mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _msgQShow_h */

