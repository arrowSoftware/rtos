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

/* ioFsLib.c - File System I/O support library */

#include <rtos.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <io/ioLib.h>
#include <io/private/ioLibP.h>
#include <rtos/errnoLib.h>

/***************************************************************************
 *
 * mkdir - create a directory
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS  mkdir (
    const char *  path,    /* path to directory to create */
    mode_t        mode     /* permissions for directory */
    ) {
    int  fd;

    if (mode & ~0777) {    /* only allow permission bits to be set */
        errnoSet (EINVAL);
        return (ERROR);
    }

    fd = ioConnect (creatConnect, path, S_IFDIR | mode, NULL);
    if (fd < 0) {
        return (ERROR);
    }

    close (fd);
    return (OK);
}

/***************************************************************************
 *
 * rmdir - remove a directory
 *
 * RETURNS: OK on success, ERROR otherwise
 */

int rmdir (
    const char *  path    /* path to directory to remove */
    ) {
    STATUS  status;

    status = ioConnect (removeConnect, path, S_IFDIR);

    return (status);
}

/***************************************************************************
 *
 * symlink - create a symlink
 *
 * RETURNS: OK on success, ERROR otherwise
 */

int symlink (
    const char *  path,   /* path to symlink to create */
    const char *  target, /* path against which to link */
    mode_t        mode    /* file permission bits */
    ) {
    int  fd;

    if (mode & ~0777) {   /* only allow permission bits */
        errnoSet (EINVAL);
        return (ERROR);
    }

    fd = ioConnect (creatConnect, path, S_IFLNK | mode, target);
    if (fd < 0) {
        return (ERROR);
    }

    close (fd);
    return (OK);
}

/***************************************************************************
 *
 * readlink - read a symlink
 *
 * RETURNS: # of bytes in buffer on success, or ERROR otherwise
 */

ssize_t readlink (
    const char *  path,
    char *        buf,
    size_t        bufsize
    ) {
    int      fd;
    ssize_t  size;
    struct iovec  arg;

    fd = ioConnect (openConnect, path, O_RDONLY, S_IFLNK);
    if (fd < 0) {
        return (ERROR);
    }

    arg.iov_base = buf;
    arg.iov_len  = bufsize;

    size = ioctl (fd, FIOREADLINK, (int) &arg);

    close (fd);
    return (size);
}

/***************************************************************************
 *
 * fstat - retrieve file status information
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS fstat (
    int           fd,
    struct stat * buf
    ) {
    STATUS  status;

    status = ioctl (fd, FIOSTATGET, (int) buf);

    return (status);
}

/***************************************************************************
 *
 * stat - retrieve file status information
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS stat (
    const char *  path,
    struct stat * buf
    ) {
    int  fd;
    STATUS  status;

    fd = ioConnect (openConnect, path, O_RDONLY, 0);
    if (fd < 0) {
        return (ERROR);
    } 

    status = ioctl (fd, FIOSTATGET, (int) buf);

    close (fd);
    return (status);
}

/***************************************************************************
 *
 * lstat - retrieve file status information
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS lstat (
    const char *  path,
    struct stat * buf
    ) {
    int  fd;
    STATUS  status;

    /*
     * TODO: Check O_NOFOLLOW usage.  Documentation indicates that when
     * 'open()' uses NO_FOLLOW, if target is a symlink, then it is supposed
     * to fail.  This, routine however uses openConnect(), an internal
     * routine to rtos.  We could add some more checks to 'open()' for this
     * and allow the internal routines to open a symlink with this oflag set.
     * Anyways, food for thought.
     */

    fd = ioConnect (openConnect, path, O_NOFOLLOW | O_RDONLY, 0);
    if (fd < 0) {
        return (ERROR);
    } 

    status = ioctl (fd, FIOSTATGET, (int) buf);

    close (fd);
    return (status);
}

/***************************************************************************
 *
 * fpathconf - get configurable path variables
 *
 * RETURNS: current configurable value or limit on success, ERROR otherwise
 */

long fpathconf (
    int  fd,
    int  name
    ) {
    long  rv;

    rv = ioctl (fd, FIOPATHCONF, name);

    return (rv);
}

/***************************************************************************
 *
 * pathconf - get configurable path variables
 *
 * RETURNS: current configurable value or limit on success, ERROR otherwise
 */

long pathconf (
    const char *  path,
    int           name
    ) {
    int   fd;
    long  rv;

    fd = ioConnect (openConnect, path, O_RDONLY, 0);
    if (fd < 0) {
        return (ERROR);
    }

    rv = fpathconf (fd, name);
    close (fd);

    return (rv);
}

/***************************************************************************
 *
 * ftruncate - truncate a file
 *
 * RETURNS: OK on success, ERROR otherwise
 */

int  ftruncate (
    int    fd,
    off_t  length
    ) {
    STATUS  status;

    status = ioctl (fd, FIOTRUNCATE, (int) &length);

    return (status);
}

/***************************************************************************
 *
 * lseek - seek to within a position in the file
 *
 * RETURNS: new position on success, ERROR otherwise
 */

off_t  lseek (
    int    fd,
    off_t  offset,
    int    whence
    ) {
    struct stat  buf;
    STATUS  status;
    off_t   value;

    if (whence == SEEK_CUR) {
        /* Add <offset> to current position */
        if (ioctl (fd, FIOWHERE, (int) &value) != OK) {
            return (ERROR);
        }
        offset += value;
    } else if (whence == SEEK_END) {
        /* Add <offset> to size of the file */
        if (fstat (fd, &buf) != OK) {
            return (ERROR);
        }
        offset += buf.st_size;
    } else if (whence != SEEK_SET) {
        errnoSet (EINVAL);
        return (ERROR);
    }

    if (offset < 0) {
        errnoSet (EINVAL);
        return (ERROR);
    }

    status = ioctl (fd, FIOSEEK, offset);

    return (status);
}
