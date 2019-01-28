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

/* localtime.c - Convert calendar time to broken-down time */

/* Includes */
#include <stdlib.h>
#include <time.h>
#include <private/timeP.h>
#include <rtos.h>

/* Defines */

/* Imports */
IMPORT TIMELOCALE *__loctime;

/* Locals */

/* Globals */

/* Functions */

/*******************************************************************************
 * localtime - Convert calendar time to broken-down time
 *
 * RETURNS: Pointer to time struct
 ******************************************************************************/

struct tm* localtime(const time_t *timer)
{
  static struct tm timeBuffer;

  localtime_r(timer, &timeBuffer);

  return (&timeBuffer);
}

/*******************************************************************************
 * localtime_r - Convert calendar time to broken-down time
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

int localtime_r(const time_t *timer, struct tm *timeBuffer)
{
  int dstOffset;
  char zoneBuffer[sizeof(ZONEBUFFER)];

  /* Get zone info */
  __getZoneInfo(zoneBuffer, TIMEOFFSET, __loctime);

  /* Create borken-down time structure */
  __getTime(*timer - ((atoi(zoneBuffer)) * SECSPERMIN), timeBuffer);

  /* Correct for daylight savings */
  dstOffset = __getDstInfo(timeBuffer, __loctime);
  timeBuffer->tm_isdst = dstOffset;
  if (dstOffset)
    __getTime((*timer - ((atoi(zoneBuffer)) * SECSPERMIN)) +
	      (dstOffset * SECSPERHOUR), timeBuffer);

  return OK;
}

