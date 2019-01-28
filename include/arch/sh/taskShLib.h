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

/* taskShLib.h - Architecture dependent task context switch */

#ifndef _taskShLib_h
#define _taskShLib_h

#define TIMER_INTERRUPT_NUM	0x20

#define TASK_INITIAL_FLAGS	0x202
#define TASK_TCB_ENTRY		0x38
#define TASK_TCB_ERRNO		0x3c
#define TASK_TCB_ID		0x40
#define TASK_TCB_STATUS		0x44
#define TASK_TCB_LOCK_COUNT	0x48
#define TASK_TCB_SWAP_IN	0x4c
#define TASK_TCB_SWAP_OUT	0x4e
#define TASK_TCB_REGS		0x50

#define TASK_TCB_VBR		TASK_TCB_REGS + REGS_VBR
#define TASK_TCB_GBR		TASK_TCB_REGS + REGS_GBR
#define TASK_TCB_PR		TASK_TCB_REGS + REGS_PR
#define TASK_TCB_R0		TASK_TCB_REGS + REGS_R0
#define TASK_TCB_R1		TASK_TCB_REGS + REGS_R1
#define TASK_TCB_R2		TASK_TCB_REGS + REGS_R2
#define TASK_TCB_R3		TASK_TCB_REGS + REGS_R3
#define TASK_TCB_R4		TASK_TCB_REGS + REGS_R4
#define TASK_TCB_R5		TASK_TCB_REGS + REGS_R5
#define TASK_TCB_R6		TASK_TCB_REGS + REGS_R6
#define TASK_TCB_R7		TASK_TCB_REGS + REGS_R7
#define TASK_TCB_MACH		TASK_TCB_REGS + REGS_MACH
#define TASK_TCB_MACL		TASK_TCB_REGS + REGS_MACL
#define TASK_TCB_R8		TASK_TCB_REGS + REGS_R8
#define TASK_TCB_R9		TASK_TCB_REGS + REGS_R9
#define TASK_TCB_R10		TASK_TCB_REGS + REGS_R10
#define TASK_TCB_R11		TASK_TCB_REGS + REGS_R11
#define TASK_TCB_R12		TASK_TCB_REGS + REGS_R12
#define TASK_TCB_R13		TASK_TCB_REGS + REGS_R13
#define TASK_TCB_R14		TASK_TCB_REGS + REGS_R14
#define TASK_TCB_R15		TASK_TCB_REGS + REGS_R15
#define TASK_TCB_PC		TASK_TCB_REGS + REGS_PC
#define TASK_TCB_SR		TASK_TCB_REGS + REGS_SR

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <arch/regs.h>

//extern void taskRegsInit(TCB *pTcb, char *pStackBase);
extern void taskRegsInitData(REG_SET *regs, u_int32_t stkpntr,
                      	     INSTR *routine, u_int32_t *args, int usermode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _taskShLib_h */

