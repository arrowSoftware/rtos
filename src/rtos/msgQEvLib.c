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

/* msgQEvLib.c - Message queue library */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <util/qLib.h>
#include <util/qMsgLib.h>
#include <rtos/eventLib.h>
#include <rtos/rtosLib.h>
#include <arch/intArchLib.h>
#include <rtos/msgQEvLib.h>

/* Imports */
TCB_ID taskIdCurrent;

/* Locals */
LOCAL BOOL msgQEvIsFree(MSG_Q_ID msgQId);

/*******************************************************************************
* msgQEvRegister - Register event for message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQEvRegister(MSG_Q_ID msgQId,
		      u_int32_t events,
		      u_int8_t options)
{
  if (events == 0x00)
  {
    errnoSet (S_eventLib_NO_EVENTS);
    return(ERROR);
  }

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  taskLock();

  if (OBJ_VERIFY(msgQId, msgQClassId) != OK)
  {

    taskUnlock();
    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  return( eventResourceRegister( (OBJ_ID) msgQId,
				 &msgQId->events,
				 msgQEvIsFree,
				 events,
				 options) );
}

/*******************************************************************************
* msgQEvIsFree - Check if message is present on queue
*
* RETURNS: TRUE, FALSE
*******************************************************************************/

LOCAL BOOL msgQEvIsFree(MSG_Q_ID msgQId)
{
  if (msgQId->msgQ.first != NULL)
    return(TRUE);

  return(FALSE);
}

/*******************************************************************************
* msgQEvUnregister - Unregister event for message queue
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS msgQEvUnregister(MSG_Q_ID msgQId)
{
  int level;

  if (INT_RESTRICT() != OK)
  {
    errnoSet (S_intLib_NOT_ISR_CALLABLE);
    return(ERROR);
  }

  INT_LOCK(level);

  if (OBJ_VERIFY(msgQId, msgQClassId) != OK)
  {

    INT_UNLOCK(level);
    /* errno set by OBJ_VERIFY() */
    return(ERROR);
  }

  if (msgQId->events.taskId != (int) taskIdCurrent)
  {
    INT_UNLOCK(level);
    errnoSet (S_eventLib_NOT_REGISTERED_TASK);
    return(ERROR);
  }

  msgQId->events.taskId = NULL;

  return(OK);
}

