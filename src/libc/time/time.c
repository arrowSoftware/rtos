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

/* time.c - Time functions */

#include <stdlib.h>
#include <time.h>

/*******************************************************************************
 * time - Get current calendar time
 *
 * RETURNS: Current time
 ******************************************************************************/

time_t time(time_t *timer)
{
  struct timespec tp;

  if (clock_gettime(CLOCK_REALTIME, &tp) != OK)
     return ERROR;

  if (*timer != NULL)
    *timer = (time_t) tp.tv_sec;

  return ((time_t) tp.tv_sec);
}

