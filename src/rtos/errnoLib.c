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

/* errnoLib.c - Error types */

#include <stdlib.h>
#include <rtos.h>
#include <errno.h>
#include <rtos/errnoLib.h>
#include <rtos/taskLib.h>

int errno;

/**************************************************************
* errnoSet - Set error number
*
* RETURNS: OK or ERROR
**************************************************************/

STATUS errnoSet(int val)
{
  errno = val;

  return(OK);
}

/**************************************************************
* errnoGet - Get error number
*
* RETURNS: Error code
**************************************************************/

int errnoGet(void)
{
  return(errno);
}

/**************************************************************
* errnoOfTaskGet - Get error number from task
*
* RETURNS: Error code
**************************************************************/

int errnoOfTaskGet(int taskId)
{
  TCB_ID tcbId;

  if ( (taskId == 0) || (taskId == taskIdSelf()) )
    return errno;

  tcbId = taskTcb(taskId);
  if ( tcbId == NULL )
    return ERROR;

  return tcbId->errorStatus;
}

