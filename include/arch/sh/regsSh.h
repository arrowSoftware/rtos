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

/* regsSh.h - Processor registers */

#ifndef _regsSh_h
#define _regsSh_h

#define REGS_VBR		0x00
#define REGS_GBR		0x04
#define REGS_PR			0x08
#define REGS_R0			0x0c
#define REGS_R1			0x10
#define REGS_R2			0x14
#define REGS_R3			0x18
#define REGS_R4			0x1c
#define REGS_R5			0x20
#define REGS_R6			0x24
#define REGS_R7			0x28
#define REGS_MACH		0x2c
#define REGS_MACL		0x30
#define REGS_R8			0x34
#define REGS_R9			0x38
#define REGS_R10		0x3c
#define REGS_R11		0x40
#define REGS_R12		0x44
#define REGS_R13		0x48
#define REGS_R14		0x4c
#define REGS_R15		0x50
#define REGS_PC			0x54
#define REGS_SR			0x58

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

typedef u_int16_t INSTR;

typedef struct {
  u_int32_t vbr;		/* Vector base register */
  u_int32_t gbr;		/* Global base register */
  INSTR *pr;			/* Procedure register */
  u_int32_t voreg[8];		/* Volatile registers */
  u_int32_t mac[2];		/* Multiply and accumlate registers */
  u_int32_t nvreg[8];		/* Non-volatile registers */
  INSTR *pc;			/* Program counter */
  u_int32_t sr;			/* Status register */
} __attribute__((packed)) REG_SET;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _regsSh_h */

