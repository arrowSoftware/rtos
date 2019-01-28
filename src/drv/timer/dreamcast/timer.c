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

/* timer.c - Dreamcast timer */

/* This code is largely based on KallistiOS
 * File arch/dreamcast/kernel/timer.c
 * Original Copyright (c)2000, 2001, 2002 Dan Potter
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <drv/timer/dreamcast/timer.h>
#include <arch/intArchLib.h>

/* Quick access macros */
#define TIMER8(o) ( *((volatile u_int8_t*)(0xffd80000 + (o))) )
#define TIMER16(o) ( *((volatile u_int16_t*)(0xffd80000 + (o))) )
#define TIMER32(o) ( *((volatile u_int32_t*)(0xffd80000 + (o))) )
#define TOCR	0x00
#define TSTR	0x04
#define TCOR0	0x08
#define TCNT0	0x0c
#define TCR0	0x10
#define TCOR1	0x14
#define TCNT1	0x18
#define TCR1	0x1c
#define TCOR2	0x20
#define TCNT2	0x24
#define TCR2	0x28
#define TCPR2	0x2c

static int tcors[] = { TCOR0, TCOR1, TCOR2 };
static int tcnts[] = { TCNT0, TCNT1, TCNT2 };
static int tcrs[] = { TCR0, TCR1, TCR2 };

/* Pre-initialize a timer; set values but don't start it */
int timer_prime(int which, u_int32_t speed, int interrupts) {
	/* P0/64 scalar, maybe interrupts */
	if (interrupts)
		TIMER16(tcrs[which]) = 32 | 2;
	else
		TIMER16(tcrs[which]) = 2;
		
	/* Initialize counters; formula is P0/(tps*64) */
	/* SHOULD INVESTIGATE LATER */
	/* Causes undefined reference to __udivsi3_i4 */
	TIMER32(tcnts[which]) = 50000000 / (speed*64);
	TIMER32(tcors[which]) = 50000000 / (speed*64);
	
	if (interrupts)
		timer_enable_ints(which);
	
	return 0;
}

/* Works like timer_prime, but takes an interval in milliseconds
   instead of a rate. Used by the primary timer stuff. */
static int timer_prime_wait(int which, u_int32_t millis, int interrupts) {
	/* Calculate the countdown, formula is P0 * millis/64000. We
	   rearrange the math a bit here to avoid integer overflows. */
	u_int32_t cd = (50000000 / 64) * millis / 1000;

	/* P0/64 scalar, maybe interrupts */
	if (interrupts)
		TIMER16(tcrs[which]) = 32 | 2;
	else
		TIMER16(tcrs[which]) = 2;

	/* Initialize counters */
	TIMER32(tcnts[which]) = cd;
	TIMER32(tcors[which]) = cd;
	
	if (interrupts)
		timer_enable_ints(which);
	
	return 0;
}

/* Start a timer -- starts it running (and interrupts if applicable) */
int timer_start(int which) {
	TIMER8(TSTR) |= 1 << which;
	return 0;
}

/* Stop a timer -- and disables its interrupt */
int timer_stop(int which) {
	timer_disable_ints(which);
	
	/* Stop timer */
	TIMER8(TSTR) &= ~(1 << which);

	return 0;
}

/* Returns the count value of a timer */
u_int32_t timer_count(int which) {
	return TIMER32(tcnts[which]);
}

/* Clears the timer underflow bit and returns what its value was */
int timer_clear(int which) {
	u_int16_t value = TIMER16(tcrs[which]);
	TIMER16(tcrs[which]) &= ~0x100;
	
	return (value & 0x100) ? 1 : 0;
}

/* Spin-loop kernel sleep func: uses the secondary timer in the
   SH-4 to very accurately delay even when interrupts are disabled */
void timer_spin_sleep(int ms) {
	timer_prime(TMU1, 1000, 0);
	timer_clear(TMU1);
	timer_start(TMU1);

	while (ms > 0) {
		while (!(TIMER16(tcrs[TMU1]) & 0x100))
			;
		timer_clear(TMU1);
		ms--;
	}

	timer_stop(TMU1);
}



/* Enable timer interrupts (high priority); needs to move
   to irq.c sometime. */
void timer_enable_ints(int which) {
	volatile u_int16_t *ipra = (u_int16_t*)0xffd00004;
	*ipra |= (0x000f << (12 - 4*which));
}

/* Disable timer interrupts; needs to move to irq.c sometime. */
void timer_disable_ints(int which) {
	volatile u_int16_t *ipra = (u_int16_t*)0xffd00004;
	*ipra &= ~(0x000f << (12 - 4*which));
}

/* Check whether ints are enabled */
int timer_ints_enabled(int which) {
	volatile u_int16_t *ipra = (u_int16_t*)0xffd00004;
	return (*ipra & (0x000f << (12 - 4*which))) != 0;
}

/* Millisecond timer */
static u_int32_t timer_ms_counter = 0;
static u_int32_t timer_ms_countdown;
static void timer_ms_handler(u_int32_t code, REG_SET *regs) {
	timer_ms_counter++;
}

void timer_ms_enable() {
	intConnectHandler(INUM_TMU2_UNDERFLOW, timer_ms_handler);
	timer_prime(TMU2, 1, 1);
	timer_ms_countdown = timer_count(TMU2);
	timer_clear(TMU2);
	timer_start(TMU2);
}

void timer_ms_disable() {
	timer_stop(TMU2);
	timer_disable_ints(TMU2);
}

/* Return the number of ticks since KOS was booted */
void timer_ms_gettime(u_int32_t *secs, u_int32_t *msecs) {
	u_int32_t used;

	/* Seconds part comes from ms_counter */
	if (secs)
		*secs = timer_ms_counter;

	/* Milliseconds, we check how much of the timer has elapsed */
	if (msecs) {
		//assert( timer_ms_countdown > 0 );
		used = timer_ms_countdown - timer_count(TMU2);
        	/* SHOULD INVESTIGATE LATER */
		/* Causes undefined reference to __udivsi3_i4 */
		*msecs = used * 1000 / timer_ms_countdown;
	}
}

u_int64_t timer_ms_gettime64() {
	u_int32_t s, ms;
	u_int64_t msec;

	timer_ms_gettime(&s, &ms);
	msec = ((u_int64_t)s) * 1000 + ((u_int64_t)ms);

	return msec;
}

/* Primary kernel timer. What we'll do here is handle actual timer IRQs
   internally, and call the callback only after the appropriate number of
   millis has passed. For the DC you can't have timers spaced out more
   than about one second, so we emulate longer waits with a counter. */
static timer_primary_callback_t tp_callback;
static u_int32_t tp_ms_remaining;

/* IRQ handler for the primary timer interrupt. */
static void tp_handler(u_int32_t code, REG_SET *regs) {
	/* Are we at zero? */
	if (tp_ms_remaining == 0) {
		/* Disable any further timer events. The callback may
		   re-enable them of course. */
		timer_stop(TMU0);
		timer_disable_ints(TMU0);
		
		/* Call the callback, if any */
		if (tp_callback)
			tp_callback(regs);
	} /* Do we have less than a second remaining? */
	else if (tp_ms_remaining < 1000) {
		/* Schedule a "last leg" timer. */
		timer_stop(TMU0);
		timer_prime_wait(TMU0, tp_ms_remaining, 1);
		timer_clear(TMU0);
		timer_start(TMU0);
		tp_ms_remaining = 0;
	} /* Otherwise, we're just counting down. */
	else {
		tp_ms_remaining -= 1000;
	}
}

/* Enable / Disable primary kernel timer */
static void timer_primary_init() {
	/* Clear out our vars */
	tp_callback = NULL;
	
	/* Clear out TMU0 and get ready for wakeups */
	intConnectHandler(INUM_TMU0_UNDERFLOW, tp_handler);
	timer_clear(TMU0);
}

static void timer_primary_shutdown() {
	timer_stop(TMU0);
	timer_disable_ints(TMU0);
	intConnectHandler(INUM_TMU0_UNDERFLOW, NULL);
}

timer_primary_callback_t timer_primary_set_callback(timer_primary_callback_t cb) {
	timer_primary_callback_t cbold = tp_callback;
	tp_callback = cb;
	return cbold;
}

void timer_primary_wakeup(u_int32_t millis) {
	/* Don't allow zero */
	if (millis == 0) {
		//assert_msg( millis != 0, "Received invalid wakeup delay" );
		millis++;
	}

	/* Make sure we stop any previous wakeup */
	timer_stop(TMU0);

	/* If we have less than a second to wait, then just schedule the
	   timeout event directly. Otherwise schedule a periodic second
	   timer. We'll replace this on the last leg in the IRQ. */
	if (millis >= 1000) {
		timer_prime_wait(TMU0, 1000, 1);
		timer_clear(TMU0);
		timer_start(TMU0);
		tp_ms_remaining = millis - 1000;
	} else {
		timer_prime_wait(TMU0, millis, 1);
		timer_clear(TMU0);
		timer_start(TMU0);
		tp_ms_remaining = 0;
	}
}


/* Init */
int timer_init() {
	/* Disable all timers */
	TIMER8(TSTR) = 0;
	
	/* Set to internal clock source */
	TIMER8(TOCR) = 0;

	/* Setup the primary timer stuff */
	timer_primary_init();

	return 0;
}

/* Shutdown */
void timer_shutdown() {
	/* Shutdown primary timer stuff */
	timer_primary_shutdown();

	/* Disable all timers */
	TIMER8(TSTR) = 0;
	timer_disable_ints(TMU0);
	timer_disable_ints(TMU1);
	timer_disable_ints(TMU2);
}



