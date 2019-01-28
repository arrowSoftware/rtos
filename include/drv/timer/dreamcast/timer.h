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

/* timer.h - Timer */

/* This code is largely based on KallistiOS
 * File  arch/dreamcast/include/timer.h
 * Original (c)2000-2001 Dan Potter
 */

#ifndef _timer_h
#define _timer_h

/* Timer sources -- we get four on the SH4 */
#define TMU0	0	/* Off limits during thread operation */
#define TMU1	1	/* Used for timer_spin_sleep() */
#define TMU2	2	/* Used for timer_get_ticks() */
#define WDT	3	/* Not supported yet */

/* The main timer for the task switcher to use */
#define TIMER_ID TMU0

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <arch/regs.h>

/* Pre-initialize a timer; set values but don't start it */
int timer_prime(int which, u_int32_t speed, int interrupts);

/* Start a timer -- starts it running (and interrupts if applicable) */
int timer_start(int which);

/* Stop a timer -- and disables its interrupt */
int timer_stop(int which);

/* Returns the count value of a timer */
u_int32_t timer_count(int which);

/* Clears the timer underflow bit and returns what its value was */
int timer_clear(int which);

/* Spin-loop kernel sleep func: uses the secondary timer in the
   SH-4 to very accurately delay even when interrupts are disabled */
void timer_spin_sleep(int ms);

/* Enable timer interrupts (high priority); needs to move
   to irq.c sometime. */
void timer_enable_ints(int which);

/* Disable timer interrupts; needs to move to irq.c sometime. */
void timer_disable_ints(int which);

/* Check whether ints are enabled */
int timer_ints_enabled(int which);

/* Enable the millisecond timer */
void timer_ms_enable();
void timer_ms_disable();

/* Return the number of ticks since KOS was booted */
void timer_ms_gettime(u_int32_t *secs, u_int32_t *msecs);

/* Does the same as timer_ms_gettime(), but it merges both values
   into a single 64-bit millisecond counter. May be more handy
   in some situations. */
u_int64_t timer_ms_gettime64();

/* Set the callback target for the primary timer. Set to NULL
   to disable callbacks. Returns the address of the previous
   handler. */
typedef void (*timer_primary_callback_t)(REG_SET *);
timer_primary_callback_t timer_primary_set_callback(timer_primary_callback_t callback);

/* Request a wakeup in approximately N milliseconds. You only get one
   simultaneous wakeup. Any subsequent calls here will replace any 
   pending wakeup. */
void timer_primary_wakeup(u_int32_t millis);

/* Init function */
int timer_init();

/* Shutdown */
void timer_shutdown();
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _timer_h */

