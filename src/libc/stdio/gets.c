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

/* gets.c - Get string from stdin */

#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * gets - Get string from stdin
 *
 * RETURNS: Pointer to string
 ******************************************************************************/

char* gets(char *buf)
{
  int c;
  char *str;

  /* Setup locals */
  str = buf;

  /* While not end-of-line */
  while ( (c = getchar()) != '\n') {

    /* If end-of-file */
    if (c == EOF) {

      if (str == buf)
        return NULL;
      else
        break;

    } /* End if end-of-file */

    /* Else not end-of-file */
    else
      *str++ = c;

  } /* End while not end-of-line */

  /* Put end-of-string at end */
  *str = EOS;

  return buf;
}

