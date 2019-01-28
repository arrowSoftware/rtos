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

/* fwrite.c - Write to stream */

#include <stdio.h>

/*******************************************************************************
 * fwrite - Write data to stream
 *
 * RETURNS: Number of objects written
 ******************************************************************************/

size_t fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
  size_t i, len;
  struct __suio uio;
  struct __siov iov;

  /* Check object */
  if (OBJ_VERIFY(fp, fpClassId) != OK)
    return 0;

  /* Setup locals */
  len = count * size;

  iov.iov_base = (void *) buf;
  iov.iov_len = len;

  uio.uio_resid = len;
  uio.uio_iov = &iov;
  uio.uio_iovcnt = 1;

  /* Write */
  if (__sfvwrite(fp, &uio) == 0)
    return count;

  return ((len - uio.uio_resid) / size);
}

