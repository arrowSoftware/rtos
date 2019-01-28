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

/* strftime.c - Format time into string */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <private/timeP.h>
#include <rtos.h>

/* Locals */
LOCAL size_t strftime_r(char *buffer_arg, size_t buflen, const char *fmt_arg,
			const struct tm *tmp, TIMELOCALE *timeInfo);

/* Imports */
IMPORT TIMELOCALE *__loctime;

/* Functions */

/*******************************************************************************
 * strftime - Format time into string
 *
 * RETURNS: Number of chars
 ******************************************************************************/

size_t strftime(char *s, size_t n, const char *fmt, const struct tm *tmp)
{
  return strftime_r(s, n, fmt, tmp, __loctime);
}

/*******************************************************************************
 * strftime_r - Format time into string
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL size_t strftime_r(char *buffer_arg, size_t buflen, const char *fmt_arg,
			const struct tm *tmp, TIMELOCALE *timeInfo)
{
  const char *fmt;
  char *buffer;
  char buf[buflen];
  int n, len;

  /* Initialize locals */
  fmt = fmt_arg;
  buffer = buffer_arg;
  n = 0;
  len = 0;

  while (1) {

    /* Process leading non-procent chars */
    while ( (*fmt != '%') &&
	    (n <= buflen) &&
	    (*fmt != EOS) ) {

      n++;
      *buffer++ = *fmt++;

    }

    if (n >= buflen)
      break;

    if (*fmt++ != EOS) {

      __generateTime(buf, tmp, timeInfo, &len, fmt);

      /* Process time string */
      if (len >= 0) {

        if ( buflen > (n + len) ) {

          memcpy(buffer, buf, len);
          n += len;
          buffer += len;
          fmt++;

        }

        else {

          memcpy(buffer, buf, buflen - n);
          buffer += (buflen - n);
          n = buflen;
          break;

        }

      }

      /* Process format string */
      else {

        *(buf + abs(len)) = EOS;
        len = (int) strftime_r(buffer,
			       buflen - n,
			       buf,
			       tmp,
			       timeInfo);
	buffer += len;
	n += len;
	fmt++;

      }

    }

    else {

      break;

    }

  }

  *buffer = EOS;

  return n;
}

