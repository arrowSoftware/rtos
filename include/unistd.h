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

/* unistd.h - Unix I/O */

#ifndef _unistd_h
#define _unistd_h

#include <io/ioLib.h>
#include <io/pathLib.h>

/* File positions */
#define SEEK_CUR  	0	/* set file offset to offset */
#define SEEK_END  	1	/* set file offset to current + offset */
#define SEEK_SET  	2	/* set file offset to EOF + offset */

/* Standard file descriptors */
#define STDIN_FILENO	0	/* Standard input */
#define STDOUT_FILENO	1	/* Standard output */
#define STDERR_FILENO	2	/* Standard error output */

#ifndef _ASMLANGUAGE

extern off_t lseek(int fd, off_t offset, int whence);
extern int read(int fd, void *buffer, size_t nBytes);
extern int write(int fd, void *buffer, size_t nBytes);
extern char* getcwd(char *buf, size_t size);
extern int chdir(char *path);
extern int rmdir(const char *path);
extern int isatty(int fd);
extern int close(int fd);
extern long fpathconf(int fd, int name);
extern long pathconf(const char *path, int name);
extern int ftruncate(int fd, off_t length);
extern int symlink(const char *path, const char *target, mode_t mode);
extern ssize_t readlink(const char *path, char *buf, size_t bufSize);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _unistd_h */

