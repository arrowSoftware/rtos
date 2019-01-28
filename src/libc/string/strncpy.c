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

/* strncpy.c - Copy string up to a maximum size */

#include <sys/types.h>

/*******************************************************************************
 * strncpy - Copy string up to maximum side
 *
 * RETURNS: Pointer to string
 ******************************************************************************/

char* strncpy(char *s1, const char *s2, size_t size)
{
  char *ptr = s1;

  /* While size left not reached zero */
  while (size--) {

    if (*s2)
      *s1++ = *s2++;

    else
      *s1++ = '\0';

  } /* End while size left not reached zero */

  return ptr;
}

