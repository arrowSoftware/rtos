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

/* blkIo.h - Block device I/O */

#ifndef _blkIo_h
#define _blkIo_h

#include <rtos.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  FUNCPTR	bd_blkRd;
  FUNCPTR	bd_blkWrt;
  FUNCPTR	bd_ioctl;
  FUNCPTR	bd_reset;
  FUNCPTR	bd_statusChk;
  BOOL		bd_removable;
  unsigned long	bd_bytesPerBlk;
  unsigned long	bd_nBlocks;
  unsigned long	bd_blksPerTrack;
  unsigned long	bd_nHeads;
  int		bd_retry;
  int		bd_mode;
  BOOL		bd_readyChanged;
} BLK_DEV;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _blkIo_h */

