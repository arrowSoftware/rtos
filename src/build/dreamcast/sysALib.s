/* KallistiOS 1.2.0
 *
 * startup.s
 * (c)2000-2001 Dan Potter
 *
 * This file must appear FIRST in your linking order, or your program won't
 * work correctly as a raw binary.
 *
 * This is very loosely based on Marcus' crt0.s/startup.s
 *
 */

/* Edited by Alexander von Knorring (SUHS):
 * To be included in the Real rtos distribution.
 * Original KallistiOS is BSD License.
 * Real rtos is GNU public license.
 */

#define _ASMLANGUAGE
#include <arch/asm.h>
#include <stdlib.h>

#include "config.h"

	/* Globals */
	.global	sysInit
	.global	_sysInit
	.global	GTEXT(sysExit)
	.global	GTEXT(sysOldSR)
	.global	GTEXT(sysOldVBR)
	.global	GTEXT(sysOldSP)
	.global	GTEXT(sysOldFPSCR)
	.global	GTEXT(intPrioTable)
	.global	GTEXT(intPrioTableSize)

/*******************************************************************************
* sysInit - Os entry point
*
* RETURNS: N/A
*******************************************************************************/

_sysInit:
sysInit:
	// Disable interrupts (if they're enabled)
	mov.l	old_sr_addr,r0
	stc	sr,r1
	mov.l	r1,@r0
	mov.l	init_sr,r0
	ldc	r0,sr

	// Run in the P2 area
	mov.l	setup_cache_addr,r0
	mov.l	p2_mask,r1
	or	r1,r0
	jmp	@r0
	nop

setup_cache:
	// Now that we are in P2, it's safe to enable the cache
	// Check to see if we should enable OCRAM.
	//mov.l	kos_init_flags_addr, r0
	//mov.l	@r0, r0
	//tst	#0x10, r0
	//bf	.L_setup_cache_L0
	mov.w	ccr_data,r1
	//bra	.L_setup_cache_L1
	//nop
//.L_setup_cache_L0:
	//mov.w	ccr_data_ocram,r1
//.L_setup_cache_L1:
	mov.l	ccr_addr,r0
	mov.l	r1,@r0

	// After changing CCR, eight instructions must be executed before
	// it's safe to enter a cached area such as P1
	nop			// 1
	nop			// 2
	nop			// 3
	nop			// 4
	nop			// 5 (d-cache now safe)
	nop			// 6
	mov.l	init_addr,r0	// 7
	mov	#0,r1		// 8
	jmp	@r0		// go
	mov	r1,r0
	nop

init:
	// Save old PR on old stack so we can get to it later
	sts.l	pr,@-r15

	// Save the current stack, and set a new stack (higher up in RAM)
	mov.l	old_stack_addr,r0
	mov.l	r15,@r0
	mov.l	new_stack,r15

	// Save VBR
	mov.l	old_vbr_addr,r0
	stc	vbr,r1
	mov.l	r1,@r0

	// Save FPSCR
	mov.l	old_fpscr_addr,r0
	sts	fpscr,r1
	mov.l	r1,@r0

	// Reset FPSCR
	//mov	#4,r4		// Use 00040000 (DN=1)
	//mov.l	fpscr_addr,r0
	//jsr	@r0
	//shll16	r4

	// Setup a sentinel value for frame pointer in case we're using
	// FRAME_POINTERS for stack tracing.
	mov	#-1,r14

	// Jump to the kernel main
	mov.l	main_addr,r0
	jsr	@r0
	nop

	// Program can return here (not likely) or jump here directly
	// from anywhere in it to go straight back to the monitor

/*******************************************************************************
* sysExit - Exit from OS
*
* RETURNS: N/A
*******************************************************************************/

FUNC_LABEL(sysExit)
	// Reset SR
	mov.l	old_sr,r0
	ldc	r0,sr

	// Disable MMU, invalidate TLB
	mov	#4,r0
	mov.l	mmu_addr,r1
	mov.l	r0,@r1

	// Wait (just in case)
	nop				// 1
	nop				// 2
	nop				// 3
	nop				// 4
	nop				// 5
	nop				// 6
	nop				// 7
	nop				// 8
	
	// Restore VBR
	mov.l	old_vbr,r0
	ldc	r0,vbr

	// If we're working under dcload, call its EXIT syscall
	mov.l	dcload_magic_addr,r0
	mov.l	@r0,r0
	mov.l	dcload_magic_value,r1
	cmp/eq	r0,r1
	bf	normal_exit

	mov.l	dcload_syscall,r0
	mov.l	@r0,r0
	jsr	@r0
	mov	#15,r4

	// Set back the stack and return (presumably to a serial debug)
normal_exit:
	mov.l	old_stack,r15
	lds.l	@r15+,pr
	rts
	nop

// Misc variables
	.align	2
dcload_magic_addr:
	.long	0x8c004004
dcload_magic_value:
	.long	0xdeadbeef
dcload_syscall:
	.long	0x8c004008
FUNC_LABEL(sysOldSR)
old_sr:
	.long	0
FUNC_LABEL(sysOldVBR)
old_vbr:
	.long	0
FUNC_LABEL(sysOldFPSCR)
old_fpscr:
	.long	0
init_sr:
	.long	0x500000f0
old_sr_addr:
	.long	old_sr
old_vbr_addr:
	.long	old_vbr
old_fpscr_addr:
	.long	old_fpscr
old_stack_addr:
	.long	old_stack
FUNC_LABEL(sysOldSP)
old_stack:
	.long	0
new_stack:
	.long	0x8d000000
p2_mask:
	.long	0xa0000000
setup_cache_addr:
	.long	setup_cache
init_addr:
	.long	init
main_addr:
	.long	FUNC(usrInit)
mmu_addr:
	.long	0xff000010
fpscr_addr:
	.long	0x0//___set_fpscr	// in libgcc
kos_init_flags_addr:
	.long	0x0//___kos_init_flags
ccr_addr:
	.long	0xff00001c
ccr_data:
	.word	0x090d
ccr_data_ocram:
	.word	0x092d

	.align	4

#ifndef INT_LVL_SYSCLK
#error  INT_LVL_SYSCLK not defined
#endif

#ifndef INT_LVL_AUXCLK
#error  INT_LVL_AUXCLK not defined
#endif

#ifndef INT_LVL_TSTAMP
#error  INT_LVL_TSTAMP not defined
#endif

#ifndef INT_LVL_SCI
#error  INT_LVL_SCI not defined
#endif

#ifndef INT_LVL_SCIF
#error  INT_LVL_SCIF not defined
#endif

/*******************************************************************************
* intPrioTable - Interrupts priority table
*******************************************************************************/

	.align	ALIGN_DATA
	.type	FUNC(intPrioTable),@object

FUNC_LABEL(intPrioTable)			/* EXPEVT INTEVT */

	.long	NULL				/* 0x000: NOT USED */
	.long	NULL				/* 0x020: NOT USED */
	.long	0x400000f0			/* 0x040: TLB instr/data(r) */
	.long	0x400000f0			/* 0x060: TBL instr/data(w) */
	.long	0x400000f0			/* 0x080: Init page write */
	.long	0x400000f0			/* 0x0a0: TBL data(r) prot */
	.long	0x400000f0			/* 0x0c0: TLB data(w) prot */
	.long	0x400000f0			/* 0x0e0: Err addr(r) */
	.long	0x400000f0			/* 0x100: Err addr(w) */
	.long	0x400000f0			/* 0x120: FPU exc */
	.long	0x400000f0			/* 0x140: TBL instr/data(m) */
	.long	0x400000f0			/* 0x160: Uncond trap */
	.long	0x400000f0			/* 0x180: Illegal instr */
	.long	0x400000f0			/* 0x1a0: Illegal slot instr */
	.long	NULL				/* 0x1c0: NMI */
	.long	0x400000f0			/* 0x1e0: Usr break trap */
	.long	0x400000f0			/* 0x200: IRL15 */
	.long	0x400000e0			/* 0x220: IRL14 */
	.long	0x400000d0			/* 0x240: IRL13 */
	.long	0x400000c0			/* 0x260: IRL12 */
	.long	0x400000b0			/* 0x280: IRL11 */
	.long	0x400000a0			/* 0x2a0: IRL10 */
	.long	0x40000090			/* 0x2c0: IRL9 */
	.long	0x40000080			/* 0x2e0: IRL8 */
	.long	0x40000070			/* 0x300: IRL7 */
	.long	0x40000060			/* 0x320: IRL6 */
	.long	0x40000050			/* 0x340: IRL5 */
	.long	0x40000040			/* 0x360: IRL4 */
	.long	0x40000030			/* 0x380: IRL3 */
	.long	0x40000020			/* 0x3a0: IRL2 */
	.long	0x40000010			/* 0x3c0: IRL1 */
	.long	0x400000f0			/* 0x3e0: Reserved */
	.long	0x40000000|(INT_LVL_SYSCLK<<4)	/* 0x400: TMU0_UNDEFLOW */
	.long	0x40000000|(INT_LVL_AUXCLK<<4)	/* 0x420: TMU1_UNDEFLOW */
	.long	0x40000000|(INT_LVL_TSTAMP<<4)	/* 0x440: TMU2_UNDEFLOW */
	.long	0x400000f0			/* 0x460: TMU2_INPUT_CAPTURE */
	.long	0x400000f0			/* 0x480: RTC_ALARM */
	.long	0x400000f0			/* 0x4a0: RTC_PERIODIC */
	.long	0x400000f0			/* 0x4c0: RTC_CARRY */
	.long	0x40000000|(INT_LVL_SCI<<4)	/* 0x4e0: SCI_RX_ERROR */
	.long	0x40000000|(INT_LVL_SCI<<4)	/* 0x500: SCI_RX */
	.long	0x40000000|(INT_LVL_SCI<<4)	/* 0x520: SCI_TX */
	.long	0x40000000|(INT_LVL_SCI<<4)	/* 0x540: SCI_TX_ERROR */
	.long	0x400000f0			/* 0x560: WDT_INTERVAL_TIMER */
	.long	0x400000f0			/* 0x580: BSC_REFRESH_CMI */
	.long	0x400000f0			/* 0x5a0: BSC_REFRESH_OVF */
	.long	0x400000f0			/* 0x5c0: Reserved */
	.long	0x400000f0			/* 0x5e0: Reserved */
	.long	0x400000f0			/* 0x600: Hitachi-UDI */
	.long	0x400000f0			/* 0x620: Reserved */
	.long	0x400000f0			/* 0x640: DMAC DMTE0 */
	.long	0x400000f0			/* 0x660: DMAC DMTE1 */
	.long	0x400000f0			/* 0x680: DMAC DMTE2 */
	.long	0x400000f0			/* 0x6a0: DMAC DMTE3 */
	.long	0x400000f0			/* 0x6c0: DMAC DMAE */
	.long	0x400000f0			/* 0x6e0: Reserved */
	.long	0x40000000|(INT_LVL_SCIF<<4)	/* 0x700: SCIF_RX_ERROR */
	.long	0x40000000|(INT_LVL_SCIF<<4)	/* 0x720: SCIF_RX */
	.long	0x40000000|(INT_LVL_SCIF<<4)	/* 0x740: SCIF_TX */
	.long	0x40000000|(INT_LVL_SCIF<<4)	/* 0x760: SCIF_TX_ERROR */
	.long	0x400000f0			/* 0x780: Reserved */
	.long	0x400000f0			/* 0x7a0: Reserved */
	.long	0x400000f0			/* 0x7c0: Reserved */
	.long	0x400000f0			/* 0x7e0: Reserved */
	.long	0x400000f0			/* 0x800: FPU disable */
	.long	0x400000f0			/* 0x820: FPU slot disable */

intPrioTableEnd:

	.align	ALIGN_DATA
	.type	FUNC(intPrioTableSize),@object
	.size	FUNC(intPrioTableSize),4

FUNC_LABEL(intPrioTableSize)
	.long	intPrioTableEnd - FUNC(intPrioTable)

/*******************************************************************************
* Include rest of assembler files
*******************************************************************************/

/* Basic architecure dependent stuff */
#include <arch/sh/excALib.s>
#include <arch/sh/intALib.s>
#include <arch/sh/kernALib.s>
#include <arch/sh/sysALib.s>
#include <arch/sh/libkern/lib1funcs.s>

