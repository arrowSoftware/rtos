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

/* ivSh.h - Interrupt vector */

#ifndef _ivSh_h
#define _ivSh_h

/*******************************************************************************
* Exception/Interrupt 			       Code:   No:Vector:      Entry:
*******************************************************************************/

#define INUM_EXC_LOW			(0x000 >> 4) /* Exception lower limit */
/*#define INUM_RESET_PWRON		(0x000 >> 4) 	0   0x000 (0xa0000000)*/
/*#define INUM_RESET_MANUAL		(0x020 >> 4)	1   0x004 (0xa0000000)*/
#define INUM_TLB_READ_MISS		(0x040 >> 4) /* 2   0x008 (vbr+0x400) */
#define INUM_TBL_WRITE_MISS		(0x060 >> 4) /*	3   0x00c (vbr+0x400) */
#define INUM_TBL_WRITE_INITIAL_PAGE	(0x080 >> 4) /*	4   0x010 (vbr+0x100) */
#define INUM_TBL_READ_PROTECTED		(0x0a0 >> 4) /*	5   0x014 (vbr+0x100) */
#define INUM_TBL_WRITE_PROTECTED	(0x0c0 >> 4) /*	6   0x018 (vbr+0x100) */
#define INUM_TBL_READ_ADDRESS_ERROR	(0x0e0 >> 4) /*	7   0x01c (vbr+0x100) */
#define INUM_TBL_WRITE_ADDRESS_ERROR	(0x100 >> 4) /*	8   0x020 (vbr+0x100) */
#define INUM_FPU_EXCEPTION		(0x120 >> 4) /*	9   0x024 (vbr+0x100) */
#define INUM_TLB_MULTIPLE_HIT		(0x140 >> 4) /*	10  0x028 (0xa0000000)*/
#define INUM_TRAPA_INSTRUCTION		(0x160 >> 4) /*	11  0x02c (vbr+0x100) */
#define INUM_ILLEGAL_INST_GENERAL	(0x180 >> 4) /*	12  0x030 (vbr+0x100) */
#define INUM_ILLEGAL_INST_SLOT		(0x1a0 >> 4) /*	13  0x034 (vbr+0x100) */
#define INUM_NMI			(0x1c0 >> 4) /*	14  0x038 (vbr+0x100) */
#define INUM_USER_BREAK_TRAP		(0x1e0 >> 4) /*	15  0x03c (vbr+0x100) */
#define INUM_EXC_HIGH			(0x1e0 >> 4) /* Exception upper limit */
#define INUM_INTR_LOW			(0x200 >> 4) /* Interrupt lower limit */
#define INUM_IRL15			(0x200 >> 4) /* 16  0x040 (vbr+0x600) */
#define INUM_IRL14			(0x220 >> 4) /* 17  0x044 (vbr+0x600) */
#define INUM_IRL13			(0x240 >> 4) /* 18  0x048 (vbr+0x600) */
#define INUM_IRL12			(0x260 >> 4) /* 19  0x04c (vbr+0x600) */
#define INUM_IRL11			(0x280 >> 4) /* 20  0x050 (vbr+0x600) */
#define INUM_IRL10			(0x2a0 >> 4) /* 21  0x054 (vbr+0x600) */
#define INUM_IRL9			(0x2c0 >> 4) /* 22  0x058 (vbr+0x600) */
#define INUM_IRL8			(0x2e0 >> 4) /* 23  0x05c (vbr+0x600) */
#define INUM_IRL7			(0x300 >> 4) /* 24  0x060 (vbr+0x600) */
#define INUM_IRL6			(0x320 >> 4) /* 25  0x064 (vbr+0x600) */
#define INUM_IRL5			(0x340 >> 4) /* 26  0x068 (vbr+0x600) */
#define INUM_IRL4			(0x360 >> 4) /* 27  0x06c (vbr+0x600) */
#define INUM_IRL3			(0x380 >> 4) /* 28  0x070 (vbr+0x600) */
#define INUM_IRL2			(0x3a0 >> 4) /* 29  0x074 (vbr+0x600) */
#define INUM_IRL1			(0x3c0 >> 4) /* 30  0x078 (vbr+0x600) */
/*#define INUM_RESERVED_0		(0x3e0 >> 4)    31  0x07c Reserved    */
#define INUM_TMU0_UNDERFLOW		(0x400 >> 4) /* 32  0x080 (vbr+0x600) */
#define INUM_TMU1_UNDERFLOW		(0x420 >> 4) /* 33  0x084 (vbr+0x600) */
#define INUM_TMU2_UNDERFLOW		(0x440 >> 4) /* 34  0x088 (vbr+0x600) */
#define INUM_TMU2_INPUT_CAPTURE		(0x460 >> 4) /* 35  0x08c (vbr+0x600) */
#define INUM_RTC_ALARM			(0x480 >> 4) /* 36  0x090 (vbr+0x600) */
#define INUM_RTC_PERIODIC		(0x4a0 >> 4) /* 37  0x094 (vbr+0x600) */
#define INUM_RTC_CARRY			(0x4c0 >> 4) /* 38  0x098 (vbr+0x600) */
#define INUM_SCI_RX_ERROR		(0x4e0 >> 4) /* 39  0x09c (vbr+0x600) */
#define INUM_SCI_RX			(0x500 >> 4) /* 40  0x0a0 (vbr+0x600) */
#define INUM_SCI_TX			(0x520 >> 4) /* 41  0x0a4 (vbr+0x600) */
#define INUM_SCI_TX_END			(0x540 >> 4) /* 42  0x0a8 (vbr+0x600) */
#define INUM_WDT_INTERVAL_TIMER		(0x560 >> 4) /* 43  0x0ac (vbr+0x600) */
#define INUM_BSC_REFRESH_CMI		(0x580 >> 4) /* 44  0x0b0 (vbr+0x600) */
#define INUM_BSC_REFRESH_OVF		(0x5a0 >> 4) /* 45  0x0b4 (vbr+0x600) */
/*#define INUM_RESERVED_1		(0x5c0 >> 4)    46  0x0b8 Reserved    */
/*#define INUM_RESERVED_2		(0x5e0 >> 4)    47  0x0bc Reserved    */
#define INUM_JTAG			(0x600 >> 4) /* 48  0x0c0 (vbr+0x600) */
#define INUM_GPIO			(0x620 >> 4) /* 49  0x0c4 (vbr+0x600) */
#define INUM_GPIO			(0x620 >> 4) /* 49  0x0c4 (vbr+0x600) */
#define INUM_DMAC_DMTE0			(0x640 >> 4) /* 50  0x0c8 (vbr+0x600) */
#define INUM_DMAC_DMTE1			(0x660 >> 4) /* 51  0x0cc (vbr+0x600) */
#define INUM_DMAC_DMTE2			(0x680 >> 4) /* 52  0x0d0 (vbr+0x600) */
#define INUM_DMAC_DMTE3			(0x6a0 >> 4) /* 53  0x0d4 (vbr+0x600) */
#define INUM_DMAC_DMTER			(0x6c0 >> 4) /* 54  0x0d8 (vbr+0x600) */
/*#define INUM_RESERVED_3		(0x6e0 >> 4)    55  0x0dc Reserved    */
#define INUM_SCIF2_ERROR		(0x700 >> 4) /* 56  0x0e0 (vbr+0x600) */
#define INUM_SCIF2_RX			(0x720 >> 4) /* 57  0x0e4 (vbr+0x600) */
#define INUM_SCIF2_BREAK		(0x740 >> 4) /* 58  0x0e8 (vbr+0x600) */
#define INUM_SCIF2_TX			(0x760 >> 4) /* 59  0x0ec (vbr+0x600) */
#define INUM_SH_BUS			(0x780 >> 4) /* 60  0x0f0 (vbr+0x600) */
/*#define INUM_RESERVED_4		(0x7a0 >> 4)    61  0x0f4 Reserved    */
/*#define INUM_RESERVED_5		(0x7c0 >> 4)    62  0x0f8 Reserved    */
/*#define INUM_RESERVED_6		(0x7e0 >> 4)    63  0x0fc Reserved    */
#define INUM_FPU_DISABLE		(0x800 >> 4) /* 64  0x100 (vbr+0x600) */
#define INUM_SLOT_FPU_DISABLE		(0x820 >> 4) /* 65  0x104 (vbr+0x600) */
/*#define INUM_RESERVED_7_20		(0x840 >> 4)-(0xbe0 >> 4) Reserved    */
#define INUM_PCI_SERR			(0xa00 >> 4) /* 80  0x140 (vbr+0x600) */
#define INUM_PCI_DMA3			(0xa20 >> 4) /* 81  0x144 (vbr+0x600) */
#define INUM_PCI_DMA2			(0xa40 >> 4) /* 82  0x148 (vbr+0x600) */
#define INUM_PCI_DMA1			(0xa60 >> 4) /* 83  0x14c (vbr+0x600) */
#define INUM_PCI_DMA0			(0xa80 >> 4) /* 84  0x150 (vbr+0x600) */
#define INUM_PCI_PWONREQ		(0xaa0 >> 4) /* 85  0x154 (vbr+0x600) */
#define INUM_PCI_PWDWREQ		(0xac0 >> 4) /* 86  0x158 (vbr+0x600) */
#define INUM_PCI_ERR			(0xae0 >> 4) /* 87  0x15c (vbr+0x600) */
#define INUM_TMU3_UNDERFLOW		(0xb00 >> 4) /* 88  0x160 (vbr+0x600) */
/*#define INUM_RESERVED_21		(0xb20 >> 4)    89  0x164 Reserved    */
/*#define INUM_RESERVED_22		(0xb40 >> 4)    90  0x168 Reserved    */
/*#define INUM_RESERVED_23		(0xb60 >> 4)    91  0x16c Reserved    */
#define INUM_TMU4_UNDERFLOW		(0xb80 >> 4) /* 92  0x170 (vbr+0x600) */

#define INUM_INTR_HIGH			(0xfe0 >> 4) /* Interrupt upper limit */
#define INUM_TRAP_0			0            /* Trap lower limit      */
#define INUM_TRAP_1			1            /* Trap divide by zero   */
#define INUM_TRAP_128			128          /* Trap middle           */
#define INUM_TRAP_255			255          /* Trap upper limit      */

/* This is a software-generated one (double-fault) for when an exception
 *    happens inside an ISR. */
#define INUM_DOUBLE_FAULT        	(0x0ff0 >> 4)

/* Software-generated for unhandled exception */
#define INUM_UNHANDLED_EXC       	(0x0fe0 >> 4)

/* Macros */
#define	INUM_TO_IEVT(inum)	((inum) << 4)
#define	IEVT_TO_INUM(ievt)	((ievt) >> 4)

/* Fixed address offsets from VBR */
#define NULL_EVT_CNT_OFFSET		0x000
#define INT_EVT_ADRS_OFFSET		0x004
#define NESTED_OFFSET			0x008
#define INT_STACK_BASE_OFFSET		0x00c
#define DISPATCH_STUB_OFFSET		0x020
#define INT_EXIT_STUB_OFFSET		0x060
#define EXC_BUSERR_STUB_OFFSET		0x0a0
#define MEMPROBE_INT_STUB_OFFSET	0x0e0
#define EXC_STUB_OFFSET			0x100
#define TLB_STUB_OFFSET			0x400
#define INT_STUB_OFFSET			0x600
#define VEC_TABLE_OFFSET		0x800
#define VECTOP				0x8
#define INT_PRIO_TABLE_OFFSET		0xc00
#define PRIOTOP				0xc

/* Must be able to get this on up to 0x1000 bytes at least */
#define VMA_TABLE_SIZE			0xc00

#endif /* _ivSh_h */

