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

/* ioLib.c - I/O library */

#include <rtos.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <rtos/taskLib.h>
#include <rtos/errnoLib.h>
#include <io/ioLib.h>
#include <io/iosLib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* LOCALS */
LOCAL int ioStdFd[3];

/*******************************************************************************
* ioGlobalStdSet - Set global standard file descriptor
*
* RETURNS: N/A 
*******************************************************************************/

void ioGlobalStdSet(int stdFd, int newFd)
{
  if (STD_VALID(stdFd)) ioStdFd[stdFd] = newFd;
}

/*******************************************************************************
* ioGlobalStdGet - Get global standard file descriptor
*
* RETURNS: File descriptor or ERROR
*******************************************************************************/

int ioGlobalStdGet(int stdFd)
{
  if (STD_VALID(stdFd)) return (ioStdFd[stdFd]);

  return (ERROR);
}

/*******************************************************************************
* ioTaskStdSet - Set local standard file descriptor
*
* RETURNS: N/A 
*******************************************************************************/

void ioTaskStdSet(int taskId, int stdFd, int newFd)
{
  TCB_ID pTcb;

  /* Get task or default task if zero */
  pTcb = taskTcb(taskId);

  if (STD_VALID(stdFd) && pTcb != NULL)
    pTcb->taskStd[stdFd] = newFd;
}

/*******************************************************************************
* ioTaskStdGet - Get local standard file descriptors
*
* RETURNS: File descriptor or ERROR
*******************************************************************************/

int ioTaskStdGet(int taskId, int stdFd)
{
  TCB_ID pTcb;
  int fd;

  /* Get task or default task if zero */
  pTcb = taskTcb(taskId);

  if (pTcb != NULL) {
    fd = pTcb->taskStd[stdFd];

    if (STD_VALID(fd))
      return(ioStdFd[fd]);
    else
      return(fd);
  }

  return(ERROR);
}

/*******************************************************************************
* read - Read from a file or device
*
* RETURNS: Bytes read or ERROR
*******************************************************************************/

int read(int fd, void *buffer, size_t nBytes)
{
  int ret;

  ret =  iosRead(fd, buffer, (int) nBytes);

  return (ret);
}

/*******************************************************************************
* write - Write a file or device
*
* RETURNS: Bytes written or ERROR
*******************************************************************************/

int write(int fd, void *buffer, size_t nBytes)
{
  int ret;

  ret =  iosWrite(fd, buffer, (int) nBytes);

  return (ret);
}

/*******************************************************************************
* ioctl - I/O control function
*
* RETURNS: Driver specific
*******************************************************************************/

int ioctl(int fd, int func, int arg)
{
  return (iosIoctl(fd, func, (ARG) arg));
}

/*******************************************************************************
* isatty - Check if device is a terminal
*
* RETURNS: TRUE or FALSE
*******************************************************************************/

int isatty(int fd)
{
  return ( ioctl(fd, FIOISATTY, 0) == TRUE);
}

/***************************************************************************
 *
 * openConnect -
 *
 */

int openConnect (
    DEV_HEADER *  pDevHeader,
    char *        filename,     
    va_list       args
    ) {
    int     oflags;
    mode_t  mode;

    oflags = va_arg (args, int);
    mode = va_arg (args, mode_t);

    return (iosOpen (pDevHeader, filename, oflags, mode));
}

/***************************************************************************
 *
 * creatConnect -
 *
 */

int creatConnect (
    DEV_HEADER *  pDevHeader,
    char *        filename,
    va_list       args
    ) {
    mode_t  mode;
    char *  symlink;

    mode = va_arg (args, mode_t);
    symlink = va_arg (args, char *);

    return (iosCreate (pDevHeader, filename, mode, symlink));
}

/***************************************************************************
 *
 * removeConnect -
 *
 */

int removeConnect (
    DEV_HEADER *  pDevHeader,
    char *        filename,
    va_list       args
    ) {
    mode_t   mode;   /* type of file to remove */

    mode = va_arg (args, mode_t);

    return (iosDelete (pDevHeader, filename, mode));
}

/***************************************************************************
 *
 * ioConnect -
 *
 * RETURNS: ERROR on failure, other on success
 */

int ioConnect (
    FUNCPTR       funcInternal,
    const char *  filename,
    ...
    ) {
    int     rv;
    int     len;
    int     fd;
    int     cwdLen;
    char    path[PATH_MAX + 1];
    char *  tail;
    DEV_HEADER *  pDevHeader;
    va_list  args;

    if ((filename == NULL) || (*filename == '\0')) {
        errnoSet (ENOENT);
        return (ERROR);
    }

    /*
     * Prepend current working directory.
     * Also ensure that total path length does not exceed PATH_MAX.
     */

    /* Check length */
    cwdLen = pathCwdLen();
    len = strlen (filename);
    if (cwdLen + len > PATH_MAX) {
        errnoSet (ENAMETOOLONG);
        return (ERROR);
    }

    /* Prepend default path name */
    if (pathPrependCwd((char *) filename, path) != OK) {
      errnoSet (EINVAL);
      return (ERROR);
    }

    /* Allocate a file descriptor */
    fd = iosFdNew (NULL, NULL, NULL);

    args = va_start (args, filename);

    while (1) {
        /*
         * Strip ...
         *   1.  Any superfluous '/' characters.
         *   2.  All "." path components.
         *   3.  Any ".." path components at the start of <path>.
         */
        pathCondense (path);

        pDevHeader = iosDevFind (path, &tail);    /* Identify the device */
        if (pDevHeader == NULL) {
            errnoSet (ENODEV);
            rv = ERROR;
            break;
        }

        rv = funcInternal (pDevHeader, tail, args);
        if ((rv != FOLLOW_LINK) && (rv != FOLLOW_DOTDOT)) {
            break;
        }
    }

    va_end (args);

    /*
     * If the driver's open/create routine failed, or if the allocated
     * file descriptor could not be updated with the correct information,
     * then free the file descriptor and return ERROR.
     */

    if ((rv == ERROR) || (iosFdSet (fd, pDevHeader, path, (ARG) rv) != OK)) {
        iosFdFree (fd);
        return (ERROR);
    }

    return (fd);
}

/***************************************************************************
 *
 * open - open a file bypassing mode check
 *
 */

int openInternal(
		 const char *filename,
		 int oflags,
		 ...)
{
    int      mode;
    va_list  args;

    va_start (args, oflags);
    mode = va_arg (args, int);
    va_end (args);

    return ioConnect (openConnect, filename, oflags, mode);
}

/***************************************************************************
 *
 * open - open a file
 *
 */

int open (
    const char *  filename,
    int           oflags,
    ...
    ) {
    int      fd;
    int      mode;
    va_list  args;

    mode = 0;
    if (oflags & O_CREAT) {
        va_start (args, oflags);
        mode = va_arg (args, int);
        va_end (args);

        if ((mode & ~0777) != 0) {    /* Only permission bits may be set */
            errnoSet (EINVAL);
            return (ERROR);
        }

        mode |= S_IFREG;    /* May only create regular files this way */
    }

    fd = ioConnect (openConnect, filename, oflags, mode);

    return (fd);
}

/***************************************************************************
 *
 * creat - create a file
 *
 */

int creat (
    const char *  filename,
    mode_t        mode
    ) {
    int  fd;

    if ((mode & ~0777) != 0) {    /* Only permission bits may be set */
        return (EINVAL);
    }

    fd = ioConnect (creatConnect, filename, S_IFREG | mode, NULL);

    return (fd);
}

/***************************************************************************
 *
 * close - 
 *
 */

int close (
    int  fd
    ) {
    return (iosClose (fd));
}

/***************************************************************************
 *
 * remove - remove a file or a symlink (but not a directory)
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS remove (
    const char *  path
    ) {
    STATUS  status;

    status = ioConnect (removeConnect, path, 0);

    return (status);
}


