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

/* i8253Timer.c - Intel 8253 Programmable Interrupt Timer (PIT) */

/* Includes */
#include <stdlib.h>
#include <sys/types.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <drv/timer/i8253.h>

/* Defines */

/* Imports */
IMPORT void sysHwInit2(void);

/* Locals */
LOCAL FUNCPTR sysClockFunc = NULL;
LOCAL int sysClockArg = 0;
LOCAL BOOL sysClockRunning = FALSE;
LOCAL BOOL sysClockConnected = FALSE;
LOCAL int sysClockTicksPerSecond = 60;

/* Globals */

/* Functions */

/*******************************************************************************
 * sysClockConnect - Connect an interrupt handler routine to clock
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS sysClockConnect(FUNCPTR func, int arg)
{
  /* If first call */
  if (!sysClockConnected)
    sysHwInit2();

  /* Store interrupt routine and argument */
  sysClockFunc = func;
  sysClockArg = arg;

  /* Mark as connected */
  sysClockConnected = TRUE;

  return OK;
}

/*******************************************************************************
 * sysClockInt - System interrupt clock handler routine
 *
 * RETURNS: N/A
 ******************************************************************************/

void sysClockInt(void)
{
  /* Call interrupt acknoledge routine if set */
  if (sysClockFunc != NULL)
    ( *sysClockFunc) (sysClockArg);
}

/*******************************************************************************
 * sysClockEnable - Start interrupt clock
 *
 * RETURNS: N/A
 ******************************************************************************/

void sysClockEnable(void)
{
  u_int32_t tr;
  int level;

  /* If clock is not running */
  if (!sysClockRunning) {

    tr = PIT_CLOCK / sysClockTicksPerSecond;

    /* Lock interrupts */
    INT_LOCK(level);

    /* Setup clock */
    sysOutByte( PIT_CMD(PIT_BASE_ADR), 0x36);
    sysOutByte( PIT_CNT0(PIT_BASE_ADR), LSB(tr) );
    sysOutByte( PIT_CNT0(PIT_BASE_ADR), MSB(tr) );

    /* Unlock interrupts */
    INT_UNLOCK(level);

    /* Enable clock interrupt */
    sysIntEnablePIC(PIT0_INT_LVL);

    /* Mark clock as running */
    sysClockRunning = TRUE;

  } /* End if clock is not running */
}

/*******************************************************************************
 * sysClockDisable - Disable interrput clock
 *
 * RETURNS: N/A
 ******************************************************************************/

void sysClockDisable(void)
{
  int level;

  /* If clock running */
  if (sysClockRunning) {

    /* Disable interrupts */
    INT_LOCK(level);

    /* Stop clock */
    sysOutByte( PIT_CMD(PIT_BASE_ADR), 0x38);
    sysOutByte( PIT_CNT0(PIT_BASE_ADR), LSB(0));
    sysOutByte( PIT_CNT0(PIT_BASE_ADR), MSB(0));

    /* Note that any interrupts won't occur util sysClockEnable() is called */

    /* Disable clock interrupt */
    sysIntDisablePIC(PIT0_INT_LVL);

    /* Mark clock as not running */
    sysClockRunning = FALSE;

  } /* End if clock is running */
}

/*******************************************************************************
 * sysClockRateSet - Set system clock rate in ticks per second
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS sysClockRateSet(int ticksPerSecond)
{
  /* Check clock rate interval */
  if (ticksPerSecond < SYS_CLOCK_RATE_MIN ||
      ticksPerSecond > SYS_CLOCK_RATE_MAX)
    return ERROR;

  /* Update global tick rate variable */
  sysClockTicksPerSecond = ticksPerSecond;

  /* If clock is running */
  if (sysClockRunning) {

    sysClockDisable();
    sysClockEnable();

  } /* End if clock is running */

  return OK;
}

/*******************************************************************************
 * sysClockRateGet - Get system clock rate in ticks per second
 *
 * RETURNS: Ticks per second
 ******************************************************************************/

int sysClockRateGet(void)
{
  return sysClockTicksPerSecond;
}

