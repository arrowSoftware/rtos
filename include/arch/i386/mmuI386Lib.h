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

/* mmuI386Lib.h - Memory mapping unit for Interl 386+ */

#ifndef _mmuI386Lib_h
#define _mmuI386Lib_h

/* MMU definitions */
#define PAGE_SIZE_4KB		0x1000
#define PAGE_SIZE_4MB		0x400000

#define PD_SIZE			0x1000
#define PT_SIZE			0x1000
#define PAGE_BLOCK_SIZE		0x400000

#define DIR_BITS		0xffc00000
#define TABLE_BITS		0x003ff000
#define DIR_INDEX		22
#define TABLE_INDEX		12
#define PTE_TO_ADDR_4KB		0xfffff000
#define PTE_TO_ADDR_4MB		0xffc00000
#define ADDR_TO_PAGE		12
#define ADDR_TO_PAGEBASE	0xffc00000
#define OFFSET_BITS_4KB		0x00000fff
#define OFFSET_BITS_4MB		0x003fffff

/* State definitions */
#define MMU_STATE_MASK_VALID		0x001
#define MMU_STATE_MASK_WRITABLE		0x002
#define MMU_STATE_MASK_WBACK		0x008
#define MMU_STATE_MASK_CACHEABLE	0x010
#define MMU_STATE_MASK_GLOBAL		0x100

#define MMU_STATE_VALID			0x001
#define MMU_STATE_NOT_VALID		0x000
#define MMU_STATE_WRITABLE		0x002
#define MMU_STATE_NOT_WRITABLE		0x000
#define MMU_STATE_WBACK			0x000
#define MMU_STATE_NOT_WBACK		0x008
#define MMU_STATE_CACHEABLE		0x000
#define MMU_STATE_NOT_CACHEABLE		0x010
#define MMU_STATE_GLOBAL		0x100
#define MMU_STATE_NOT_GLOBAL		0x000
#define MMU_STATE_CACHEABLE_WT		0x008

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pteField {
  unsigned 	present	:1;
  unsigned	rw	:1;
  unsigned	us	:1;
  unsigned	pwt	:1;
  unsigned	pcd	:1;
  unsigned	access	:1;
  unsigned	dirty	:1;
  unsigned	pagesize:1;
  unsigned	global	:1;
  unsigned	avail	:3;
  unsigned	page	:20;
} PTE_ENTRY;

typedef union pte
{
  PTE_ENTRY	entry;
  unsigned int	bits;
} PTE;

typedef struct mmuTransTable
{
  PTE 		*pDirTable;
} MMU_TRANS_TABLE;

typedef MMU_TRANS_TABLE *MMU_TRANS_TABLE_ID;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _mmuI386Lib_h */

