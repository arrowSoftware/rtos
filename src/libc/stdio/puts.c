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

/* puts.c - Standard input/output */

#include <stdio.h>
#include <string.h>

int puts(const char *str)
{
  size_t len;
  struct __suio uio;
  struct __siov iov[2];

  /* Setup locals */
  len = strlen(str);

  iov[0].iov_base	=	(void *) str;
  iov[0].iov_len	=	len;

  iov[1].iov_base	=	"\n";
  iov[1].iov_len	=	1;

  uio.uio_resid		=	len + 1;
  uio.uio_iov		=	&iov[0];
  uio.uio_iovcnt	=	2;

  return ( (__sfvwrite(stdout, &uio)) ? (EOF) : ('\n') );
}

