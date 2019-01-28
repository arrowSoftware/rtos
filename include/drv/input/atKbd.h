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

/* atKbd.h - Keyboard driver */

#ifndef _atKbd_h
#define _atKbd_h

#define	KEYBOARD_INTERRUPT	0x09
#define	KEY_BUFFER		0x60
#define	KEY_CONTROL		0x61
#define	PIC_PORT		0x20

#define KEY_ESC			1
#define KEY_1			2
#define KEY_2			3
#define KEY_3			4
#define KEY_4			5
#define KEY_5			6
#define KEY_6			7
#define KEY_7			8
#define KEY_8			9
#define KEY_9			10
#define KEY_0			11
#define KEY_A			30

#ifdef __cplusplus
extern "C" {
#endif

extern void atKbdInit(void);

extern int keys;
extern int kbdon[];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _atKbd_h */

