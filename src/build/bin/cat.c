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

/* cat.c - Concatenate from file list */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

/* Defines */

/* Imports */

/* Locals */

/* Globals */
char catErrStr[] = "Error - Unable to open file\n";

/* Functions */

/*******************************************************************************
 * cat - Concatenate contents of files
 *
 * RETURNS: Zero or error
 ******************************************************************************/

int cat(char *fn, ...)
{
  va_list args;
  int fd;
  char buf;
  char *fileName;

  args = va_start(args, fn);
  fileName = fn;

  do {

    fd = open(fileName, O_RDONLY, 0777);
    if (fd == -1) {

      write(STDOUT_FILENO, catErrStr, strlen(catErrStr));
      return 1;

    }

    do {

      if (read(fd, &buf, 1) != 1)
        break;

      write(STDOUT_FILENO, &buf, 1);

    } while (buf != EOF);

    close(fd);

    fileName = va_arg(args, char *);

  } while (fileName != NULL);

  va_end(args);

  return 0;
}

