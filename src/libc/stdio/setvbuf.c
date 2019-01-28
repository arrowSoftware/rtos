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

/* setvbuf.c - Set stdio buffer */

#include <stdio.h>

/*******************************************************************************
 * setvbuf - Set stdio buffer
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int setvbuf(FILE *fp, char *buf, int mode, size_t size)
{
  /* Check mode and size */
  if ( ((mode != _IOFBF) && (mode != _IOLBF) && (mode != _IONBF)) ||
       ((int) size < 0))
    return EOF;

  /* Flush buffer */
  __sflush(fp);

  /* Reset read space left and lbfsize */
  fp->_r = 0;
  fp->_lbfsize = 0;

  /* Set flags */
  fp->_flags &= ~(__SLBF | __SNBF | __SMBF);

  /* Select mode */
  switch(mode) {
    case _IONBF:
	fp->_flags |= __SNBF;
	fp->_bf._base = fp->_p = fp->_nbuf;
	fp->_bf._size = 1;
	break;

    case _IOLBF:
	fp->_flags |= __SLBF;
	fp->_lbfsize = -size;
	/* FALL TROUGH */

    case _IOFBF:
	fp->_bf._base = fp->_p = (unsigned char *) buf;
	fp->_bf._size = size;
	break;

    default:
	break;
  }

  /* Fix byte count if nessesary */
  if (fp->_flags & __SWR)
    if ( fp->_flags & (__SLBF | __SNBF) )
      fp->_w = 0;
    else
      fp->_w = size;

  return 0;
}

