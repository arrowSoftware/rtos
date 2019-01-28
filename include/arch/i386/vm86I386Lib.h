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

/* vm86I386.h - x86 architecture bios interrupts */

/*
 * $XFree86: xc/programs/Xserver/hw/kdrive/vesa/vm86.h,v 1.1 2000/10/20 00:19:51 keithp Exp $
 *
 * Copyright � 2000 Keith Packard, member of The XFree86 Project, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* 
Copyright (c) 2000 by Juliusz Chroboczek
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions: 
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _vm86I386_h_
#define _vm86I386_h_

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
/*
#include <sys/mman.h>
#include <sys/vm86.h>
#include <sys/io.h>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
static void ErrorF(char*, ...);
#define xalloc(a) malloc(a)
#define xcalloc(a,b) mallocc(a * b)
#define xfree(a) free(a)

/* BEGIN: Types taken from Linux vm86.h */

#define IF_MASK         0x00000200
#define IOPL_MASK	0x00003000

#define VM86_SIGNAL	0
#define VM86_UNKNOWN	1
#define VM86_INTx	2
#define VM86_STI	3

#define CPU_586		5

#define VM86_TYPE(retval)	((retval) & 0xff)
#define VM86_ARG(retval)	((retval) >> 8)

struct vm86_regs {
/*
 * normal regs, with special meaning for the segment descriptors..
 */
	long ebx;
	long ecx;
	long edx;
	long esi;
	long edi;
	long ebp;
	long eax;
	long __null_ds;
	long __null_es;
	long __null_fs;
	long __null_gs;
	long orig_eax;
	long eip;
	unsigned short cs, __csh;
	long eflags;
	long esp;
	unsigned short ss, __ssh;
/*
 * these are specific to v86 mode:
 */
	unsigned short es, __esh;
	unsigned short ds, __dsh;
	unsigned short fs, __fsh;
	unsigned short gs, __gsh;
};

struct revectored_struct {
	unsigned long __map[8];			/* 256 bits */
};

struct vm86_struct {
	struct vm86_regs regs;
	unsigned long flags;
	unsigned long screen_bitmap;
	unsigned long cpu_type;
	struct revectored_struct int_revectored;
	struct revectored_struct int21_revectored;
};
/* END: Types taken from Linux vm86.h */

typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;

/* The whole addressable memory */
#define SYSMEM_BASE 0x00000
#define SYSMEM_SIZE 0x100000

/* Interrupt vectors and BIOS data area */
/* This is allocated privately from /dev/mem */
#define MAGICMEM_BASE 0x00000
#define MAGICMEM_SIZE 0x01000

/* The low memory, allocated privately from /dev/zero */
/* 64KB should be enough for anyone, as they used to say */
#define LOMEM_BASE 0x10000
#define LOMEM_SIZE 0x10000

/* The video memory and BIOS ROM, allocated shared from /dev/mem */
#define HIMEM_BASE 0xA0000
#define HIMEM_SIZE (SYSMEM_BASE + SYSMEM_SIZE - HIMEM_BASE)

/* The BIOS ROM */
#define ROM_BASE 0xC0000
#define ROM_SIZE 0x30000

#define STACK_SIZE 0x1000

#define POINTER_SEGMENT(ptr) (((unsigned int)ptr)>>4)
#define POINTER_OFFSET(ptr) (((unsigned int)ptr)&0x000F)
#define MAKE_POINTER(seg, off) (((((unsigned int)(seg))<<4) + (unsigned int)(off)))
#define MAKE_POINTER_1(lw) MAKE_POINTER(((lw)&0xFFFF0000)/0x10000, (lw)&0xFFFF)
#define ALLOC_FAIL ((U32)-1)

typedef struct _Vm86InfoRec {
    void		*magicMem, *loMem, *hiMem;
    U32			brk;
    struct vm86_struct	vms;
    U32			ret_code, stack_base;
} Vm86InfoRec, *Vm86InfoPtr;

#define LM(vi,i) (((char*)vi->loMem)[i-LOMEM_BASE])
#define LMW(vi,i) (*(U16*)(&LM(vi,i)))
#define LML(vi,i) (*(U32*)(&LM(vi,i)))
#define MM(vi,i) (((char*)vi->magicMem)[i-MAGICMEM_BASE])
#define MMW(vi,i) (*(U16*)(&MM(vi,i)))
#define MML(vi,i) (*(U32*)(&MM(vi,i)))
#define HM(vi,i) (((char*)vi->hiMem)[i-HIMEM_BASE])
#define HMW(vi,i) (*(U16*)(&MM(vi,i)))
#define HML(vi,i) (*(U32*)(&MM(vi,i)))

Vm86InfoPtr
Vm86Setup(void);
    
void
Vm86Cleanup(Vm86InfoPtr vi);

int
Vm86DoInterrupt(Vm86InfoPtr vi, int num);

int
Vm86IsMemory(Vm86InfoPtr vi, U32 i);

U8
Vm86Memory(Vm86InfoPtr, U32);

U16
Vm86MemoryW(Vm86InfoPtr, U32);

U32
Vm86MemoryL(Vm86InfoPtr, U32);

void
Vm86WriteMemory(Vm86InfoPtr, U32, U8);

void
Vm86WriteMemoryW(Vm86InfoPtr, U32, U16);

void
Vm86WriteMemoryL(Vm86InfoPtr, U32, U32);

int
Vm86AllocateMemory(Vm86InfoPtr, int);

int
Vm86MarkMemory (Vm86InfoPtr vi);

void
Vm86ReleaseMemory (Vm86InfoPtr vi, int mark);

void
Vm86Debug(Vm86InfoPtr vi);

#endif /* _vm86I386_h_ */

