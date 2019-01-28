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

/* sysI386Lib.h - System library */

#ifndef _sysI386Lib_h
#define _sysI386Lib_h

#define GDT_ENTRIES	5
#define	IDT_ENTRIES	256

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* System initialization */
void sysHwInit(void);

/* I/O functions */
char sysInByte(int address);
short sysInWord(int address);
long sysInLong(int address);
void sysOutByte(int address, char data);
void sysOutWord(int address, short data);
void sysOutLong(int address, long data);
void sysInWordString(int port, short *address, int count);
void sysInLongString(int port, long *address, int count);
void sysOutWordString(int port, short *address, int count);
void sysOutLongString(int port, long *address, int count);
void sysWait(void);
void sysReboot(void);

/* System controll functions */
void sysReboot(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _sysI386Lib_h */

