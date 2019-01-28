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

/* ldiv.c - Division functions for long integers */

/* Includes */
#include <stdlib.h>

/* Functions */

/*******************************************************************************
 * ldiv - Calculate quotient and reminder
 *
 * RETURNS: Result in a ldiv_t structure
 ******************************************************************************/

ldiv_t ldiv(long numer, long denom)
{
  static ldiv_t result;

  ldiv_r(numer, denom, &result);

  return result;
}

/*******************************************************************************
 * ldiv_r - Calculate quotient and reminder
 *
 * RETURNS: N/A
 ******************************************************************************/

void ldiv_r(long numer, long denom, ldiv_t *result)
{
  result->quot = numer / denom;
  result->rem = numer - (denom * result->quot);

  if ( (numer < 0) &&
       (result->rem > 0) ) {

    result->quot++;
    result->rem -= denom;

  }

}

