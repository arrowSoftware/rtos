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

/* rand.c - Random number generator */

/* Includes */
#include <stdlib.h>

/* Globals */
unsigned long _randseed = 1;

/* Functions */

/*******************************************************************************
 * srand - Set random seed for random number generator
 *
 * RETURNS: NULL
 ******************************************************************************/

void* srand(unsigned int seed)
{
  _randseed = seed;

  return NULL;
}

/*******************************************************************************
 * rand - Generate random number
 *
 * RETURNS: Random number
 ******************************************************************************/

int rand(void)
{
  _randseed = _randseed * 1103515245 + 12345;

  return (unsigned int) (_randseed / 65536) % (RAND_MAX + 1);
}

