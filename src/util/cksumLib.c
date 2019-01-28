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

/* cksumLib.c - Checksum library */

/* Includes */
#include <stdlib.h>
#include <rtos.h>
#include <util/cksumLib.h>

/* Defines */

/* Imports */

/* Locals */

/* Globals */

/* Functions */

/*******************************************************************************
 * checksum - Calculate checksum
 *
 * RETURNS: Checksum
 ******************************************************************************/

unsigned short checksum(unsigned short *addr, int len)
{
  int nLeft, sum;
  unsigned short result;
  unsigned short *w;

  /* Initialize locals */
  nLeft = len;
  sum = 0;
  w = addr;

  while (nLeft > 1) {

    sum += *w++;
    nLeft -= 2;

  }

  if (nLeft == 1) {

#if _BYTE_ORDER == _BIG_ENDIAN

    sum += 0 | ((*(unsigned char *) w) << 8);

#else /* _BYTE_ORDER == _LITTLE_ENDIAN */

    sum += *(unsigned char *) w;

#endif /* _BYTE_ORDER */

  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  result = sum;

  return (~result & 0xffff);
}

