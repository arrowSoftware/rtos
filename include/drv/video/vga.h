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

/* vga.h - VGA screen output driver */

#ifndef _vga_h
#define _vga_h

#ifdef __cplusplus
extern "C" {
#endif

extern void vgaInit(void);
extern void vgaClear2(void);
extern void vgaMoveCsr(void);
extern void vgaScroll2(void);
extern void vgaPutch(char c);
extern void vgaPuts(const char *str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _vga_h */

