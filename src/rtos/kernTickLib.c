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

/* kernTickLib.c - Kernel tick library */

/* includes */

#include <sys/types.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <rtos/rtosLib.h>
#include <rtos/kernTickLib.h>

/* imports */

IMPORT BOOL kernelState;

/* globals */

volatile unsigned long kernTicks = 0;
volatile u_int64_t kernAbsTicks = 0;

/******************************************************************************
* kernTickLibInit - Initialize kernel tick library
*
* RETURNS: N/A
******************************************************************************/

void kernTickLibInit(void)
{
   /* Initialize variables */
   kernTicks = 0;
   kernAbsTicks = 0;
}

/***************************************************************************
 *
 * kernTickAnnounce - timer tick handler
 *
 * RETURNS: N/A
 */

void kernTickAnnounce (void)
    {
    if (kernelState)
        {
        kernQAdd0 (rtosTickAnnounce);
        return;
        }

    kernelState = TRUE;
    rtosTickAnnounce ();
    kernExit ();
    }

/******************************************************************************
* tickSet - Set time
*
* RETURNS: N/A
******************************************************************************/

void tickSet(unsigned long ticks)
{
  int level;

  INT_LOCK(level);

  kernAbsTicks = ticks;

  INT_UNLOCK(level);
}

/******************************************************************************
* tickGet - Get time
*
* RETURNS: N/A
******************************************************************************/

unsigned long tickGet(void)
{
  return (unsigned long) (kernAbsTicks & 0xffffffffull);
}

/******************************************************************************
* tick64Set - Set time
*
* RETURNS: N/A
******************************************************************************/

void tick64Set(u_int64_t ticks)
{
  int level;

  INT_LOCK(level);

  kernAbsTicks = ticks;

  INT_UNLOCK(level);
}

/******************************************************************************
* tick64Get - Get time
*
* RETURNS: N/A
******************************************************************************/

u_int64_t tick64Get(void)
{
  int level;
  u_int64_t ticks;

  INT_LOCK(level);

  ticks = kernAbsTicks;

  INT_UNLOCK(level);

  return ticks;
}

