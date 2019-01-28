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

/* configAll.h - Congifuration for all architectures */

#ifndef _configAll_h
#define _configAll_h

/* Kernel configuration */
#define INT_LOCK_LEVEL			0x0	/* Interrupt lockout level */
#define ROOT_STACK_SIZE			10000	/* Root task stack size */
#define SHELL_STACK_SIZE		20000	/* Shell stack size */
#define ISR_STACK_SIZE			2000	/* Interrupt stack size */
#define VEC_BASE_ADRS			((char *) LOCAL_MEM_LOCAL_ADRS)

#define SYS_CLOCK_RATE			60	/* Ticks per second */
#define NUM_DRIVERS			20	/* Maximum drivers */
#define NUM_FILES			50	/* Maximum open files */
#define MAX_LOG_MSGS			50	/* Maximum of log messages */
#define NUM_SIGNAL_QUEUES		16	/* Numboer of signal queues */
#define SYM_TABLE_HASH_SIZE_LOG2	8	/* Symbol table log 2 size  */

#define USER_I_CACHE_MODE		CACHE_WRITETROUGH
#define USER_D_CACHE_MODE		CACHE_WRITETROUGH
#define USER_I_CACHE_ENABLE
#define USER_D_CACHE_ENABLE

/* Low memory layout */
#define GDT_BASE_OFFSET			0x1000

/* Components to include */
#define INCLUDE_WDOG
#define INCLUDE_TIME
#define INCLUDE_ENV_VARS
#define ENV_VAR_USE_HOOKS		TRUE
#define INCLUDE_ASSERT
#define INCLUDE_LOADER
#define INCLUDE_ELF
#define INCLUDE_FILESYSTEMS
#define INCLUDE_NET

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _configAll_h */

