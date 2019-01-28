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

/* uglugi.c - Universal graphics library common device */

#include <ugl/ugl.h>

/*******************************************************************************
 * uglUgiDevInit - Initialize graphics device
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS
 ******************************************************************************/

UGL_STATUS uglUgiDevInit(UGL_DEVICE_ID devId)
{
  devId->lockId = uglOsLockCreate();
  if (devId->lockId == UGL_NULL)
    return UGL_STATUS_ERROR;

  return UGL_STATUS_OK;
}

/*******************************************************************************
 * uglUgiDevDeinit - Deinitialize graphics device
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS
 ******************************************************************************/

UGL_STATUS uglUgiDevDeinit(UGL_DEVICE_ID devId)
{
  if (uglOsLockDestroy(devId->lockId) != UGL_STATUS_OK)
    return UGL_STATUS_ERROR;

  return UGL_STATUS_OK;
}

/*******************************************************************************
 * uglGraphicsDevDestroy - Free graphics device
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS
 ******************************************************************************/

UGL_STATUS uglGraphicsDevDestroy(UGL_DEVICE_ID devId)
{
  if (devId == UGL_NULL)
    return UGL_STATUS_ERROR;

  /* Destroy default graphics context */
  if (devId->defaultGc != UGL_NULL)
    uglGcDestroy(devId->defaultGc);

  /* Call specific device destroy method */
  return (*devId->destroy)(devId);
}

