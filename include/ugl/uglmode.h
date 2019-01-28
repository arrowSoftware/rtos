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

/* uglmode.h - Universal graphics library display mode */

#ifndef _uglmode_h
#define _uglmode_h

#ifndef _ASMLANGUAGE

/* Color modes */
#define UGL_MODE_DIRECT_COLOR		0x00000001
#define UGL_MODE_TRUE_COLOR		0x00000001
#define UGL_MODE_INDEXED_COLOR		0x00000002
#define UGL_MODE_PSEUDO_COLOR		0x00000002
#define UGL_MODE_MONO			0x00000004
#define UGL_MODE_GRAY_SCALE		0x00000008
#define UGL_MODE_INTERLEACE_SIMULATE	0x00000010

/* Monitor types */
#define UGL_MODE_CRT			1
#define UGL_MODE_FLAT_PANEL		2
#define UGL_MODE_DUAL_OUTPUT		3

/* Video modes */
#define UGL_NTSC			0
#define UGL_PAL				1
#define UGL_SECAM			2

/* Display types */
#define	UGL_COLOR_CRT			0
#define UGL_MONO_CRT			1
#define UGL_S_VIDEO			2
#define UGL_COMPOSITE			3
#define UGL_S_VIDEO_COMPOSITE		4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ugl_mode {
  UGL_CHAR*		Name;		/* Mode name */
  UGL_UINT16		Width;		/* Display width */
  UGL_UINT16 		Height;		/* Display height */
  UGL_UINT8		Depth;		/* BitsPerPixel */
  UGL_ORD		RefreshRate;	/* Monitor refresh rate */
  UGL_UINT16		MonitorType;	/* Monitor type */
  UGL_UINT32		Flags;		/* Misc Flags */
  UGL_UINT8		Clock;		/* PixelClock used */
  UGL_UINT32		HDisplay;
  UGL_UINT32		HSyncStart;
  UGL_UINT32		HSyncEnd;
  UGL_UINT32		HTotal;
  UGL_UINT32		HSkew;
  UGL_UINT32		VDisplay;
  UGL_UINT32		VSyncStart;
  UGL_UINT32		VSyncEnd;
  UGL_UINT32		VTotal;
} UGL_MODE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglmode_h */

