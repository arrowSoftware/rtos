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

/* ioLib.h - I/O library header */

#ifndef _ioLib_h
#define _ioLib_h

#include <tools/moduleNumber.h>

#define S_ioLib_UNKNOWN_REQUEST			(M_ioLib | 0x0001)
#define S_ioLib_DEVICE_ERROR			(M_ioLib | 0x0002)
#define S_ioLib_DISK_NOT_PRESENT		(M_ioLib | 0x0003)
#define S_ioLib_WRITE_PROTECTED			(M_ioLib | 0x0004)
#define S_ioLib_UNFORMATED			(M_ioLib | 0x0005)

/* ioctl function codes */
#define FIONREAD		1
#define FIOFLUSH		2
#define FIOOPTIONS		3
#define FIOBAUDRATE		4
#define FIODISKFORMAT		5
#define FIODISKINIT		6
#define FIOSEEK			7
#define FIOWHERE		8
#define FIODIRENTRY		9
#define FIORENAME		10
#define FIOREADYCHANGE		11
#define FIONWRITE		12
#define FIODISKCHANGE		13
#define FIOCANCEL		14
#define FIOSQUEEZE		15
#define FIONBIO			16
#define FIONMSGS		17
#define FIOGETNAME		18
#define FIOGETOPTIONS		19
#define FIOSETOPTIONS		FIOOPTIONS
#define FIOISATTY		20
#define FIOSYNC			21
#define FIOPROTOHOOK		22
#define FIOPROTOARG		23
#define FIORBUFSET		24
#define FIOWBUFSET		25
#define FIORFLUSH		26
#define FIOWFLUSH		27
#define FIOSTATGET              28
#define FIOREADLINK             29
#define FIOPATHCONF             30
#define FIOTRUNCATE             31
#define FIOREADDIR              32
#define FIOSELECT		33
#define FIOUNSELECT		34
#define FIOLABELGET		35
#define FIOLABELSET		36

#define OPT_ECHO		0x01
#define OPT_CRMOD		0x02
#define OPT_TANDEM		0x04
#define OPT_7_BIT		0x08
#define OPT_MON_TRAP	0x10
#define OPT_ABORT		0x20
#define OPT_LINE		0x40

#define OPT_TERMINAL (OPT_ECHO | OPT_CRMOD | OPT_TANDEM | \
                      OPT_MON_TRAP | OPT_7_BIT | OPT_ABORT | OPT_LINE)

#define DEFAULT_FILE_PERM	0000640
#define DEFAULT_DIR_PERM	0000750

#define FSTAT_DIR		0040000
#define FSTAT_CHR		0020000
#define FSTAT_BLK		0060000
#define FSTAT_REG		0100000
#define FSTAT_LNK		0120000
#define FSTAT_MON		0140000

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <types/rtosTypes.h>

#define STD_VALID(fd)	(((fd) >= 0) && ((fd) < 3))

extern void ioGlobalStdSet(int stdFd, int newFd);
extern int ioGlobalStdGet(int stdFd);
extern void ioTaskStdSet(int taskId, int stdFd, int newFd);
extern int ioTaskStdGet(int taskId, int stdFd);

extern int openInternal(const char *filename, int oflags, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ioLib_h */

