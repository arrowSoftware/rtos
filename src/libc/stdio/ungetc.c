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

/* ungetc.c - Unget character in stream */

#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * ungetc - Unget character in stream
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int ungetc(int c, FILE *fp)
{
  unsigned char uc;

  /* Check object */
  if (OBJ_VERIFY(fp, fpClassId) != OK)
    return EOF;

  if (c == EOF)
    return EOF;

  /* If not readable */
  if ( (fp->_flags & __SRD) == 0) {

    if ( (fp->_flags & __SRW) == 0)
      return EOF;

    /* If writeable */
    if (fp->_flags & __SWR) {

      if ( __sflush(fp) )
        return EOF;

       /* Update FILE struct */
       fp->_flags &= ~__SWR;
       fp->_w = 0;
       fp->_lbfsize = 0;

    } /* End if writable */

    /* Set as readable */
    fp->_flags |= __SRD;

  } /* End if not readable */

  /* Get character */
  uc = (unsigned char) c;

  /* If already ungetc exists */
  if ( HASUB(fp) ) {

    if ( (fp->_r >= fp->_ub._size) && __submore(fp) )
      return EOF;

    /* Ungetc */
    *--fp->_p = uc;
    fp->_r++;

    return c;

  } /* End if already ungetc exists */

  /* Ungetc buffer does not already exist */
  if ( (fp->_bf._base != NULL) &&
       (fp->_p > fp->_bf._base) &&
       (fp->_p[-1] == uc) ) {

    /* Update FILE struct */
    fp->_p--;
    fp->_r++;

    return c;

  }

  /* Create an ungetc buffer and undo read char */
  fp->_ur = fp->_r;
  fp->_up = fp->_p;
  fp->_ub._base = fp->_ubuf;
  fp->_ub._size = sizeof(fp->_ubuf);
  fp->_ubuf[sizeof(fp->_ubuf) - 1] = uc;
  fp->_p = &fp->_ubuf[sizeof(fp->_ubuf) - 1];
  fp->_r = 1;

  return c;
}

