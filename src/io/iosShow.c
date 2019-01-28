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

/* iosShow.h - Show I/O drivers and devices */

#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <util/listLib.h>
#include <io/iosLib.h>
#include <io/iosShow.h>

/* Imports */
IMPORT LIST iosDevList;

/*******************************************************************************
 * iosShowInit - Initialize I/O show
 *
 * RETURNS: N/A
 ******************************************************************************/

void iosShowInit(void)
{
}

/*******************************************************************************
 * iosDevShow - Show I/O devices
 *
 * RETURNS: N/A
 ******************************************************************************/

void iosDevShow(void)
{
  DEV_HEADER *pDevHeader;

  /* Print header */
  printf("%3s %-20s\n", "drv", "name");

  for (pDevHeader = (DEV_HEADER *) LIST_HEAD(&iosDevList);
       pDevHeader != NULL;
       pDevHeader = (DEV_HEADER *) LIST_NEXT(&pDevHeader->node))
    printf("%3d %-20s\n", pDevHeader->drvNumber, pDevHeader->name);
}

