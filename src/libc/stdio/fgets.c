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

/* fgets.c - Get line from stream */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * fgets - Get line from stream
 *
 * RETURNS: Pointer to buffer of EOF
 ******************************************************************************/

char* fgets(char *buf, size_t num, FILE *fp)
{
  size_t size;
  char *sp;
  unsigned char *p, *t;

  /* Verify object */
  if (OBJ_VERIFY(fp, fpClassId) != OK)
    return NULL;

  /* If to few characters to read */
  if (num < 2)
    return NULL;

  /* Setup pointer to begining of buffer */
  sp = buf;

  /* Decrease number of characters by one */
  num--;

  /* While number of characters not zero */
  do {

    /* Get buffer length */
    size = fp->_r;
    if (size <= 0) {

      /* Fill buffer if needed */
      if ( __srefill(fp) ) {

        /* The same as the target buffer */
        if (sp == buf)
          return NULL;

        /* Break loop if buffer was filled */
        break;

      }

      /* Get new read position */
      size = fp->_r;

    }

    /* Get position from buffer */
    p = fp->_p;

    /* If length is greater than number of chars to read */
    if (size > num)
      size = num;

    /* Search for end-of-line in buffer */
    t = memchr( (void *) p, '\n', size );
    if (t != NULL) {

      /* Got end-of-line return */
      size = ++t - p;
      fp->_r -= size;
      fp->_p = t;
      memcpy(sp, p, size);
      sp[size] = EOS;

      return buf;

    }

    /* Update file struct */
    fp->_r -= size;
    fp->_p += size;

    /* Copy data */
    memcpy(sp, p, size);

    /* Advance locals */
    sp += size;
    num -= size;

  } while (num != 0);

  /* Terminate string */
  *sp = EOS;

  return buf;
}

