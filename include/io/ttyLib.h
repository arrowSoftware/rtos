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

/* ttyLib.h - tty support library header */

#ifndef _ttyLib_h
#define _ttyLib_h

#define XON		0x11
#define XOFF		0x13

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <rtos/semLib.h>
#include <util/ringLib.h>
#include <io/iosLib.h>
#include <os/selectLib.h>

typedef struct {
  DEV_HEADER devHeader;

  RING_ID readBuffer;
  SEMAPHORE readSync;
  SEMAPHORE mutex;
  struct {
    unsigned char xoff;
    unsigned char pending;
    unsigned char canceled;
    unsigned char flushingReadBuffer;
  } readState;

  RING_ID writeBuffer;
  SEMAPHORE writeSync;
  struct {
    unsigned char busy;
    unsigned char xoff;
    unsigned char cr;
    unsigned char canceled;
    unsigned char flushingWriteBuffer;
    unsigned char writeBufferBusy;
  } writeState;

  u_int8_t lnNBytes;
  u_int8_t lnBytesLeft;
  u_int16_t options;

  FUNCPTR txStartup;
  FUNCPTR protoHook;
  ARG protoArg;
  SEL_WAKEUP_LIST selWakeupList;
  int numOpen;

} TTY_DEV;

typedef TTY_DEV *TTY_DEV_ID;

extern STATUS ttyDevInit(TTY_DEV_ID ttyId,
		         int readBufferSize,
		         int writeBufferSize,
		         FUNCPTR txStartup);
extern STATUS ttyDevRemove(TTY_DEV_ID ttyId);
extern void ttyAbortFuncSet(FUNCPTR func);
extern void ttyAbortSet(char c);
extern void ttyBackSpaceSet(char c);
extern void ttyDeleteLineSet(char c);
extern void ttyEOFSet(char c);
extern void ttyMonitorTrapSet(char c);
extern int ttyIoctl(TTY_DEV_ID ttyId, int req, int arg);
extern int ttyWrite(TTY_DEV_ID ttyId,
	            char *buffer,
	            int nBytes);
extern int ttyRead(TTY_DEV_ID ttyId,
	           char *buffer,
	           int nBytes);
extern STATUS ttyIntTx(TTY_DEV_ID ttyId, char *pc);
extern STATUS ttyIntRd(TTY_DEV_ID ttyId, char c);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ttyLib_h */

