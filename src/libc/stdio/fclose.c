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

/* fclose.c - Close stream */

#include <errno.h>
#include <rtos/errnoLib.h>
#include <stdlib.h>
#include <stdio.h>

/*******************************************************************************
* fclose - Close stream
*
* RETURNS: 0 or EOF
*******************************************************************************/

int fclose(FILE *fp)
{
  int ret;

  /* Verify object class */
  if (OBJ_VERIFY(fp, fpClassId) != OK)
    return EOF;

  /* Check flags */
  if (fp->_flags == 0) {

    errnoSet(EBADF);
    return EOF;

  }

  /* Reset return value */
  ret = 0;

  /* Flush if writable */
  if (fp->_flags & __SWR)
    ret = __sflush(fp);

  /* Close file assoicated with stream */
  if ( __sclose(fp) < 0 )
    ret = EOF;

  /* Free buffer if it was allocated internally */
  if (fp->_flags & __SMBF)
    free(fp->_bf._base);

  /* Free ungetc buffer if it was allocated */
  if ( HASUB(fp) )
    FREEUB(fp);

  /* Free line buffer if it was allocated */
  if ( HASLB(fp) )
    FREELB(fp);

  /* Zero flags */
  fp->_flags = 0;

  /* Destroy object */
  stdioFpDestroy(fp);

  return ret;
}

