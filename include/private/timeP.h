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

/* timeP.h - Private time header */

#ifndef _timeP_h
#define _timeP_h

#include <time.h>
#include <rtos.h>

#define SECSPERMIN		60		/* Seconds per minute */
#define MINSPERHOUR		60		/* Minutes per hour */
#define HOURSPERDAY		24		/* Hours per day */
#define DAYSPERWEEK		7		/* Days per week */
#define MONSPERYEAR		12		/* Moths per year */
#define DAYSPERYEAR		365		/* Days per year */
#define SECSPERHOUR		(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY		((long) SECSPERHOUR * HOURSPERDAY)

#define CENTURY			100		/* Years per century */

#define TM_THURSDAY		4		/* Fourth day of week */
#define TM_SUNDAY		0		/* Zeroeth days of week */
#define TM_MONDAY		1		/* First day of week */

#define EPOCH_WDAY		TM_THURSDAY
#define EPOCH_YEAR		1970
#define TM_YEAR_BASE		1900

#define NAME			0		/* Zone name offset */
#define NAME2			1		/* Zone name offset */
#define TIMEOFFSET		2		/* Time difference offset */

#define DSTON			0		/* Daylight savings on */
#define DSTOFF			1		/* Daylight savings off */

#define ABBR			1		/* Abbreation only */
#define FULL			0		/* Full name */

/* Locale item offsets */
#define DATETIME		0		/* Date time offset */
#define DATE			1		/* Date offset */
#define TIMEO			2		/* Time offset */
#define AMPM			3		/* Am/pm offset */
#define ZONE			4		/* Zone offset */
#define DST			5		/* Daylight offset */

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct __timeLocale {
  char *_Days[14];
  char *_Months[24];
  char *_Format[3];
  char *_Ampm[2];
  char *_Zone[3];
  char *_Isdst[2];
} TIMELOCALE;

#define ZONEBUFFER		"UTC:UTC:000:ddmmhh:ddmmhh\0"

/* Macros */
#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

/* Functions */
extern void timeLibInit(void);
extern int __julday(int yr, int mon, int day);
extern int __daysSinceEpoch(int year, int yday);
extern void __tmNormalize(int *tens, int *units, int base);
extern void __tmValidate(struct tm *pTime);
extern int __getDstInfo(struct tm *timeNow, TIMELOCALE *timeInfo);
extern void __getZoneInfo(char *buffer, int option, TIMELOCALE *timeInfo);
extern int __getTime(const time_t, struct tm *tmp);
extern void __generateTime(char *buffer, const struct tm *tmp,
			   TIMELOCALE *timeInfo, int *pos, const char *fmt);
extern void __getLocale(char *buffer, int desc, const struct tm *tmp,
			TIMELOCALE *timeInfo, int *size);
extern void __getMonth(char *buffer, int index, int abbr,
		       TIMELOCALE *timeInfo, int *size);
extern void __getDay(char *buffer, int index, int abbr,
		     TIMELOCALE *timeInfo, int *size);
extern int __weekOfYear(int start, int wday, int yday);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _timeP_h */

