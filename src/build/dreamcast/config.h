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

/* config.h - arch configuration settings */

#ifndef _config_h_
#define _config_h_

/* CPU type dependent macros */
#define	INTEVT_ADRS	0xff000028

/* Temporary puts */
#define puts		dcScrTextPuts

/* Specific arch for dreamcast */
#define _BYTE_ORDER		LITTLE_ENDIAN
#define L_div_table
#define L_udivsi3_i4
#define NO_TASKLIB
#define NO_ROOT_TASK
#define NO_IDLE_TASK
#define NO_KERN_START
#define schedLib_PORTABLE

/* Onchip interrupt levels */
#define INT_LVL_AUXCLK			15
#define INT_LVL_SYSCLK			12
#define INT_LVL_SCIF			8
#define INT_LVL_SCI			7
#define INT_LVL_TSTAMP			1

#define INCLUDE_LIBC
#define INCLUDE_VIDEO

#endif

