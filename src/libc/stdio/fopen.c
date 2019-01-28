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

/* fopen.c - Open stream */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/*******************************************************************************
* fopen - Open stream
*
* RETURNS: FILE*  or NULL
*******************************************************************************/

FILE* fopen(const char *file, const char *mode)
{
  FILE *fp;
  int fd, flags, oflags;

  /* Get flags for given mode */
  flags = __sflags(mode, &oflags);
  if (flags == 0)
    return NULL;

  /* Create stream object */
  fp = stdioFpCreate();

  /* Open file */
  fd = open(file, oflags, DEFFILEMODE);
  if (fd < 0) {

    fp->_flags = 0;
    stdioFpDestroy(fp);
    return NULL;

  }

  /* Setup FILE struct */
  fp->_file = fd;
  fp->_flags = flags;

  /* If append, goto end of file */
  if (oflags & O_APPEND)
    __sseek(fp, (fpos_t) 0, SEEK_END);

  return fp;
}

