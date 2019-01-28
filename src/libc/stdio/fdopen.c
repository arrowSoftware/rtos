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

/* fdopen.c - Create stream assisiated with file descriptor */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io/iosLib.h>

/*******************************************************************************
 * Create stream for file descritptor
 *
 * RETURNS: FILE* or NULL
 ******************************************************************************/

FILE* fdopen(int fd, const char *mode)
{
  FILE *fp;
  int flags, oflags;

  /* Check file descriptor */
  if ( iosFdValue(fd) == (ARG) 0 )
    return NULL;

  /* Get flags */
  flags = __sflags(mode, &oflags);
  if (flags == 0)
    return NULL;

  /* Create stream object */
  fp = stdioFpCreate();
  if (fp == NULL)
    return NULL;

  /* Setup FILE struct */
  fp->_flags = flags;

  if (oflags & O_APPEND)
    fp->_flags |= __SAPP;

  fp->_file = fd;

  return fp;
}

