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

/* timerLibP.h - Timers and clocks header */

#ifndef _timerLibP_h
#define _timerLibP_h

#include <time.h>
#include <sys/types.h>
#include <rtos.h>

#define BILLION		1000000000

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */

#define TV_ADD(a, b)							       \
  (a).tv_sec += (b).tv_sec;						       \
  (a).tv_nsec += (b).tv_nsec;

#define TV_CONVERT_TO_SEC(a, b)						       \
  {									       \
    u_int32_t hz = sysClockRateGet();					       \
    (a).tv_sec = (time_t) ((b) / hz);					       \
    (a).tv_nsec = (long) (((b) % hz) * (BILLION / hz));			       \
  }

typedef struct clock {
  u_int64_t tickBase;
  struct timespec timeBase;
} CLOCK;

IMPORT STATUS clockLibInit(void);
IMPORT int sysClockRateGet(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _timerLibP_h */

