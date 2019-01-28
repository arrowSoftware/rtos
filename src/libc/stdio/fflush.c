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

/* fflush.c - Flush file */

#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <rtos/errnoLib.h>

/*******************************************************************************
* fflush - Flush stdio buffers
*
* RETURNS: 0 or EOF
*******************************************************************************/

int fflush(FILE *fp)
{

  /* Check fp */
  if (fp == NULL) {
    errnoSet(EBADF);
    return EOF;
  }

  /* Check class */
  if (OBJ_VERIFY(fp, fpClassId) != OK) {
    errnoSet(EBADF);
    return EOF;
  }

  /* Check flags */
  if ( (fp->_flags & __SWR) == 0) {
    errnoSet(EBADF);
    return EOF;
  }

  /* Flush */
  return __sflush(fp);
}

