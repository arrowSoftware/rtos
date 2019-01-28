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

/* ugltypes.h - Universal graphics library types header */

#ifndef _ugltypes_h
#define _ugltypes_h

/* UGL_STATUS */
#define UGL_STATUS_FINISHED	1
#define UGL_STATUS_OK		0
#define UGL_STATUS_ERROR	-1

/* UGL_RASTER_OP */
#define UGL_RASTER_OP_COPY	0x02020101
#define UGL_RASTER_OP_AND	0x00020002
#define UGL_RASTER_OP_OR	0x00020003
#define UGL_RASTER_OP_XOR	0x00020004

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/types.h>

/* Default bitmaps */
#define UGL_DISPLAY_ID		((UGL_DDB_ID)(-1))
#define UGL_DEFAULT_ID		((UGL_DDB_ID)(UGL_NULL))

/* Integer types */
typedef int8_t			UGL_INT8;
typedef u_int8_t		UGL_UINT8;
typedef int16_t			UGL_INT16;
typedef u_int16_t		UGL_UINT16;
typedef int32_t			UGL_INT32;
typedef u_int32_t		UGL_UINT32;
typedef int			UGL_BOOL;

/* Other types */
typedef int			UGL_STATUS;
typedef char			UGL_CHAR;
typedef UGL_UINT16		UGL_WCHAR;
typedef UGL_UINT32		UGL_RASTER_OP;
typedef UGL_UINT32		UGL_COLOR;
typedef UGL_UINT32		UGL_TIMESTAMP;
typedef UGL_UINT32		UGL_TIMEOUT;
typedef void*			UGL_ID;
typedef int			UGL_SIZE;
typedef int			UGL_ORD;
typedef int			UGL_POS;

/* Misc */
#define UGL_NULL		((void *)0)
#define UGL_LOCAL		static
#define UGL_TRUE		((UGL_BOOL)1)
#define UGL_FALSE		((UGL_BOOL)0)
#define UGL_VOID		void

/* Complex types */
typedef struct ugl_range {
  UGL_ORD min, max;
} UGL_RANGE;

typedef struct ugl_point {
  UGL_POS x, y;
} UGL_POINT, UGL_VECTOR, UGL_DIMENSIONS;

typedef struct ugl_rect {
  UGL_POS left, top, right, bottom;
} UGL_RECT;

/* Macros */
/*******************************************************************************
 * UGL_LO_BYTE - Get low byte
 *
 * RETURNS: Low byte
 ******************************************************************************/

#define UGL_LO_BYTE(i)		((UGL_UINT8) (i) & 0xff)

/*******************************************************************************
 * UGL_HI_BYTE - Get high byte
 *
 * RETURNS: High byte
 ******************************************************************************/

#define UGL_HI_BYTE(i)		((UGL_UINT8) ((i) >> 8) & 0xff)

/*******************************************************************************
 * UGL_RECT_WIDTH - Get rectangle width
 *
 * RETURNS: Rectangle width
 ******************************************************************************/

#define UGL_RECT_WIDTH(r)	((r).right - (r).left + 1)

/*******************************************************************************
 * UGL_RECT_HEIGHT - Get rectangle height
 *
 * RETURNS: Rectangle height
 ******************************************************************************/

#define UGL_RECT_HEIGHT(r)	((r).bottom - (r).top + 1)

/*******************************************************************************
 * UGL_POINT_MOVE - Move a point by delta (dx, dy)
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_POINT_MOVE(p, dx, dy)					       \
{									       \
  (p).x += dx;								       \
  (p).y += dy;								       \
}

/*******************************************************************************
 * UGL_RECT_MOVE - Move a rectangle by delta (dx, dy)
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_RECT_MOVE(r, dx, dy)					       \
{									       \
  (r).left += dx;							       \
  (r).right += dx;							       \
  (r).top += dy;							       \
  (r).bottom += dy;							       \
}

/*******************************************************************************
 * UGL_RECT_MOVE_TO - Move a rectangle
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_RECT_MOVE_TO(r, x, y)					       \
{									       \
  (r).right += (x) - (r).left;						       \
  (r).bottom += (y) - (r).top;						       \
  (r).left = (x);							       \
  (r).top = (y);							       \
}

/*******************************************************************************
 * UGL_RECT_MOVE_TO_POINT - Move a rectangle to point
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_RECT_MOVE_TO_POINT(r, p)					       \
{									       \
  (r).right += (p).x - (r).left;					       \
  (r).bottom += (p).y - (r).top;					       \
  (r).left = (p).x;							       \
  (r).top = (p).y;							       \
}

/*******************************************************************************
 * UGL_RECT_SIZE_TO - Resize rectangle
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_RECT_SIZE_TO(r, w, h)					       \
{									       \
  (r).right = (r).left + (w) - 1;					       \
  (r).bottom = (r).top + (h) - 1;					       \
}

/*******************************************************************************
 * UGL_RECT_INTERSECT - Get intersection between two rectangles
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_RECT_INTERSECT(r1, r2, r)					       \
{									       \
  (r).top = max((r1).top, (r2).top);					       \
  (r).bottom = min((r1).bottom, (r2).bottom);				       \
  (r).left = max((r1).left, (r2).left);					       \
  (r).right = min((r1).right, (r2).right);				       \
}

/*******************************************************************************
 * UGL_POINT_IN_RECT - Determine if a point is inside a rectangle
 *
 * RETURNS: UGL_TRUE if inside, otherwise UGL_FALSE
 ******************************************************************************/

#define UGL_POINT_IN_RECT(p, r)						       \
(									       \
  (									       \
    (p).x >= (r).left && (p).x <= (r).right &&				       \
    (p).y >= (r).top && (p).y <= (r).bottom				       \
  )									       \
  ? UGL_TRUE								       \
  : UGL_FALSE								       \
)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ugltypes_h */

