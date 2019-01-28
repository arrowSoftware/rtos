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

/* uglpixel.c - Universal graphics library pixel support functions */

#include <ugl/ugl.h>

/*******************************************************************************
 * uglPixelSet - Set pixel
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglPixelSet(UGL_GC_ID gc, UGL_POS x, UGL_POS y, UGL_COLOR color)
{
  UGL_DEVICE_ID devId;
  UGL_POINT p;

  if (gc == UGL_NULL)
    return UGL_STATUS_ERROR;

  /* Get device */
  devId = gc->pDriver;

  /* Set coodrinates */
  p.x = x;
  p.y = y;

  /* Call specific method */
  return (*devId->pixelSet)(devId, &p, color);
}

