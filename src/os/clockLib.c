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

/* clockLib.c - Clock library */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <rtos/kernTickLib.h>
#include <os/private/timerLibP.h>

/* Defines */

/* Imports */

/* Locals */
LOCAL BOOL clockLibInstalled = FALSE;

/* Globals */
CLOCK _clockRealtime;

/* Functions */

/*******************************************************************************
 * clockLibInit - Initialize clock library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS clockLibInit(void)
{
  if (clockLibInstalled)
    return OK;

  if (sysClockRateGet() < 1)
    return ERROR;

  memset(&_clockRealtime, 0, sizeof(CLOCK));
  _clockRealtime.tickBase = tick64Get();
  _clockRealtime.timeBase.tv_sec = 0;
  _clockRealtime.timeBase.tv_nsec = 0;

  clockLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * clock_settime - Set time
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

int clock_settime(clockid_t id, const struct timespec *tp)
{
  if (id != CLOCK_REALTIME) {

    errnoSet(EINVAL);
    return ERROR;

  }

  if ( (tp == NULL) ||
       (tp->tv_sec < 0) ||
       (tp->tv_nsec >= BILLION) ) {

    errnoSet(EINVAL);
    return ERROR;

  }

  _clockRealtime.tickBase = tick64Get();
  _clockRealtime.timeBase = *tp;

  return OK;
}

/*******************************************************************************
 * clock_gettime - Get time
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

int clock_gettime(clockid_t id, struct timespec *tp)
{
  u_int64_t diff;

  if (id != CLOCK_REALTIME) {

    errnoSet(EINVAL);
    return ERROR;

  }

  if (tp == NULL) {

    errnoSet(EINVAL);
    return ERROR;

  }

  diff = tick64Get() - _clockRealtime.tickBase;
  TV_CONVERT_TO_SEC(*tp, diff);
  TV_ADD(*tp, _clockRealtime.timeBase);

  return OK;
}

