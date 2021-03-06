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

/* udg8pix.c - Universal graphics library general pixel operations */

#include <arch/sysArchLib.h>

#include <ugl/ugl.h>
#include <ugl/driver/graphics/generic/udgen.h>

/*******************************************************************************
 * uglGeneric8BitPixelSet - Set pixel
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglGeneric8BitPixelSet(UGL_DEVICE_ID devId,
			   	  UGL_POINT *p,
			   	  UGL_COLOR c)
{
  UGL_GENERIC_DRIVER *pDrv;
  UGL_GC_ID gc;
  UGL_RECT *pClipRect;
  UGL_GEN_DDB *pBmp;
  UGL_UINT8 *dest;

  /* Get driver which is first in the device structure */
  pDrv = (UGL_GENERIC_DRIVER *) devId;

  /* Get graphics context */
  gc = pDrv->gc;

  /* Move to position within viewport */
  UGL_POINT_MOVE(*p, gc->viewPort.left, gc->viewPort.top);

  /* Start clipping loop */
  UGL_CLIP_LOOP_START(gc, pClipRect);

  /* Check if point is within clipping reqion */
  if (UGL_POINT_IN_RECT(*p, *pClipRect) == UGL_TRUE) {

    /* Check if render to display or bitmap */
    if (gc->pDefaultBitmap == UGL_DISPLAY_ID) {

      /* Calculate destination address */
      dest = ((UGL_UINT8 *) pDrv->fbAddress) +
	p->x + (p->y * devId->pMode->Width);

    }
    else {

      /* Setup variables */
      pBmp = (UGL_GEN_DDB *) gc->pDefaultBitmap;
      dest = ((UGL_UINT8 *) pBmp->pData) +
	p->x + (p->y * pBmp->stride);
    }

    /* Select raster op and draw */
    switch(gc->rasterOp) {
      case UGL_RASTER_OP_COPY:	*dest = c;	break;
      case UGL_RASTER_OP_AND:	*dest &= c;	break;
      case UGL_RASTER_OP_OR:	*dest |= c;	break;
      case UGL_RASTER_OP_XOR:	*dest ^= c;	break;
    }

  } /* End if inside clipping region */

  /* End clipping loop */
  UGL_CLIP_LOOP_END;

  return UGL_STATUS_OK;
}

