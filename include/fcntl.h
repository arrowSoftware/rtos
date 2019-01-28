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

/* fcntl.h - I/O definitions */

#ifndef _fcntl_h
#define _fcntl_h

#include <sys/fcntlcom.h>

/* File access modes */
#define O_RDONLY	0		/* open in read-only mode */
#define O_WRONLY	1		/* open in write-only mode */
#define O_RDWR		2		/* open in read-write mode */
#define O_ACCMODE       3		/* mask for file access modes */

/* File creation flags */
#define O_APPEND        _FAPPEND
#define O_TRUNC         _FTRUNC
#define O_EXCL          _FEXCL
#define O_CREAT		_FCREAT
#define O_NONBLOCK      _FNONBLOCK
#define O_NOCTTY        _FNOCTTY
#define O_SYNC          _FSYNC

#define O_NOFOLLOW      0x00010000
#define O_DIRECTORY     0x00020000
#define O_CLOEXEC       0x00040000

/* Default file open mode */
#define DEFFILEMODE	0666

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

int  creat (const char *filename, mode_t mode);
int  fcntl (int fd, int cmd, ...);
int  open (const char *filename, int oflags, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _fcntl_h */

