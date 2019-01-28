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

/* udcclr.h - Universal graphics library color support */

#ifndef _udcclr_h
#define _udcclr_h

#include <ugl/ugl.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ugl_clut_entry {
  UGL_ARGB		color;			/* Color argb value */
  UGL_INT32		useCount;		/* Number of users */
  UGL_INT32		nextIndex;		/* Next index */
  UGL_INT32		prevIndex;		/* Previous index */
} UGL_CLUT_ENTRY;

typedef struct ugl_clut {
  UGL_INT32		numColors;		/* Number of colors */
  UGL_INT32		firstFreeIndex;		/* First free slot */
  UGL_INT32		firstUsedIndex;		/* First used slot */
  UGL_CLUT_ENTRY	*clut;			/* Clut entry */
} UGL_CLUT;

extern UGL_CLUT* uglCommonClutCreate(UGL_SIZE numColors);
extern UGL_STATUS uglCommonClutSet(UGL_CLUT *pClut,
			    	   UGL_ORD offset,
			    	   UGL_ARGB *pColors,
			    	   UGL_SIZE numColors);
extern UGL_STATUS uglCommonClutGet(UGL_CLUT *pClut,
			    	   UGL_ORD offset,
			    	   UGL_ARGB *pColors,
			    	   UGL_SIZE numColors);
extern UGL_STATUS uglCommonClutMapNearest(UGL_CLUT *pClut,
				   	  UGL_ARGB *pMapColors,
				   	  UGL_ARGB *pActualColors,
				   	  UGL_COLOR *pUglColors,
				   	  UGL_SIZE numColors);
extern UGL_BOOL uglCommonClutAlloc(UGL_CLUT *pClut,
			    	   UGL_ORD *pIndex,
			    	   UGL_ARGB reqColor,
			    	   UGL_ARGB *pActualColor);
extern UGL_STATUS uglCommonClutFree(UGL_CLUT *pClut,
			     	    UGL_COLOR *pColors,
			     	    UGL_SIZE numColors,
			     	    UGL_COLOR mask);
extern UGL_STATUS uglCommonClutDestroy(UGL_CLUT *pClut);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _udcclr_h */

