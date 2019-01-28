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

/* esfI386.h - Stack frame */

#ifndef _esfI386_h
#define _esfI386_h

#define	ESF0_EIP	0x00
#define ESF0_CS		0x04
#define ESF0_EFLAGS	0x08

#define ESF1_ERROR	0x00
#define	ESF1_EIP	0x04
#define ESF1_CS		0x08
#define ESF1_EFLAGS	0x0c

#define ESF0_NBYTES	0x0c
#define ESF1_NBYTES	0x10

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <rtos.h>

/* No error code, no privilege */
typedef struct
{
  INSTR *pc;
  u_int16_t cs;
  u_int16_t pad0;
  u_int32_t eflags;
} __attribute__((packed)) ESF0;

/* Error code, no privilege */
typedef struct
{
  u_int32_t errCode;
  INSTR *pc;
  u_int16_t cs;
  u_int16_t pad0;
  u_int32_t eflags;
} __attribute__((packed)) ESF1;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#endif /* _esfI386_h */

