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

/* iosLib.h - Serial io driver library header */

#ifndef _iosLib_h
#define _iosLib_h

#include <tools/moduleNumber.h>

#define S_iosLib_DRIVER_GLUT			(M_iosLib | 0x0001)
#define S_iosLib_DUPLICATE_DEVICE_NAME		(M_iosLib | 0x0002)
#define S_iosLib_DEVICE_NOT_FOUND		(M_iosLib | 0x0003)
#define S_iosLib_INVALID_FILE_DESCRIPTOR	(M_iosLib | 0x0004)
#define S_iosLib_TO_MANY_OPEN_FILES		(M_iosLib | 0x0005)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <util/listLib.h>

typedef struct {
    LIST_NODE  node;        /* node in linked list */
    u_int16_t  drvNumber;   /* installed driver number index */
    char *     name;        /* ptr to device name */
} DEV_HEADER;

#include <io/private/iosLibP.h>

extern STATUS iosLibInit(int max_dev,
			 int max_fd,
			 char *nullDevName);
extern int iosDrvInstall(FUNCPTR createMethod,
			 FUNCPTR deleteMethod,
			 FUNCPTR openMethod,
			 FUNCPTR closeMethod,
			 FUNCPTR readMethod,
			 FUNCPTR writeMethod,
			 FUNCPTR ioctlMethod);
extern STATUS iosDrvRemove(int drvNumber, BOOL forceClose);

extern void iosDefaultPathSet(char *name);
extern STATUS iosDevAdd(DEV_HEADER *pDevHeader,
			char *name,
			int drvNumber);
extern void iosDevDelete(DEV_HEADER *pDevHeader);
extern DEV_HEADER *iosDevFind(char *name, char **pNameTail);
extern DEV_HEADER *iosNextDevGet(DEV_HEADER *pDevHeader);

extern ARG iosFdValue(int fd);
extern DEV_HEADER *iosFdDevFind(int fd);
extern void iosFdFree(int fd);
extern STATUS iosFdSet(int fd,
		       DEV_HEADER *pDevHeader,
		       char *name,
		       ARG value);
extern int iosFdNew(DEV_HEADER *pDevHeader,
	            char *name,
	            ARG value);

extern int iosCreate(DEV_HEADER *pDevHeader,
	             char *filename,
	             int mode,
                     const char *symlink);
extern int iosDelete(DEV_HEADER *pDevHeader,
	             char *filename,
                     mode_t  mode);
extern int iosOpen(DEV_HEADER *pDevHeader,
	           char *filename,
	           int oflags,
	           int mode);
extern STATUS iosClose(int fd);
extern int iosRead(int fd,
	           void *buffer,
	           int maxBytes);
extern int iosWrite(int fd,
	            void *buffer,
	            int maxBytes);
extern STATUS iosIoctl(int fd,
	               int function,
	               ARG arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _iosLib_h */

