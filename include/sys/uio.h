/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2009 Surplus Users Ham Society
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

/* uio.h - header file for uio (used by scatter-gather I/O)  */

#ifndef __uio_h
#define __uio_h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include <sys/types.h>

struct iovec
    {
    void *  iov_base;    /* Base address. */
    size_t  iov_len;     /* # of bytes to copy to/from base address. */
    };

enum uio_rw
    {
    UIO_READ = 0,        /* Read from I/O vector(s) into buffer. */
    UIO_WRITE            /* Write from buffer into I/O vector(s). */
    };

/*
 * Incomplete!
 * Ultimately, the uio structure will require some fields for copying between
 * user-space and kernel-space.  At the current time, it is assumed that all
 * transactions will take place in the same memory-space.
 */

struct uio
    {
    struct iovec *  uio_iov;     /* ptr to one or more I/O vectors */
    int             uio_iovcnt;  /* # of remaining I/O vectors */
    off_t           uio_offset;  /* offset within src/dest buffer */
    int             uio_resid;   /* # of bytes left to copy */
    enum uio_rw     uio_rw;      /* direction of copy: UIO_READ or UIO_WRITE */
    };

extern void uioLibInit(void);
extern ssize_t  readv (int, const struct iovec *, int);
extern ssize_t  writev (int, const struct iovec *, int);

extern int  uiomove (void *, int, struct uio *);

#endif  /* !_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif  /* __uio_h */
