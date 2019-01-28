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

/* uioLib.c - Uio library */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/uio.h>
#include <rtos.h>

/* Defines */

/* Imports */

/* Locals */

/* Globals */

/* Functions */

/***************************************************************************
*
* uiomove - move data between buffer and I/O vectors
*
* This routine moves data between a buffer and one or more I/O vectors.  This
* is particularly useful when dealing with scatter-gather I/O.  Data is written
* from the I/O vector(s) to the buffer.  Data is read from the buffer to the
* I/O vector(s).
*
* RETURNS: OK, or EINVAL
*/

int  uiomove
    (
    void *        buffer,   /* source or destination buffer */
    int           nBytes,   /* number of bytes to move */
    struct uio *  pUio      /* ptr to uio structure */
    )
    {
    struct iovec *  pIov;
    void *          pBaseAddr;
    int             count;

    if ((pUio->uio_rw != UIO_READ) && (pUio->uio_rw != UIO_WRITE))
        return (EINVAL);

    if (nBytes > pUio->uio_resid)    /* Limit <nBytes> to the residual count */
        nBytes = pUio->uio_resid;    /* to avoid exceeding the bounds. */
 
    while ((nBytes > 0) && (pUio->uio_iovcnt > 0))
        {
        pIov  = pUio->uio_iov;
        count = pIov->iov_len;

        if (count == 0)          /* If this I/O vector is empty */
            {                    /* (zero-length), advance to   */
            pUio->uio_iov++;     /* the next I/O vector.        */
            pUio->uio_iovcnt--;
            continue;
            }

        if (count > nBytes)           /* Limit <count> so we do */
            count = nBytes;           /* not exceed the limits. */

        if (count > pUio->uio_resid)
            count = pUio->uio_resid;
        
        if (pUio->uio_rw == UIO_READ)
            {
            memmove (pIov->iov_base, (const char *) buffer, count);
            }
        else    /* Must be UIO_WRITE case */
            {
            memmove (buffer, pIov->iov_base, count);
            }

        pIov->iov_base += count;    /* Update the I/O vector. */
        pIov->iov_len  -= count;

        pUio->uio_offset += count;      /* Update the uio structure */
        pUio->uio_resid  -= count;

        buffer = (void *) ((char *) buffer + count);
        nBytes -= count;
        }

    return (OK);
    }

