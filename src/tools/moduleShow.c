/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2010 Surplus Users Ham Society
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

/* moduleShow.c - Module show routines */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <util/classLib.h>
#include <tools/moduleLib.h>
#include <tools/loadLib.h>
#include <tools/moduleShow.h>

/* Defines */

/* Imports */

/* Locals */
LOCAL char *moduleShowHeader[] = {
"\n"
"MODULE NAME     MODULE ID  GROUP #    TEXT START DATA START  BSS START\n"
"--------------- ---------- ---------- ---------- ----------  ---------\n"
};
LOCAL char moduleShowFormat[] = "%15s %#10x %10d %#10x %#10x %#10x\n";

LOCAL STATUS moduleDisplay(MODULE_ID modId, int options);

/* Globals */

/* Functions */

/*******************************************************************************
 * moduleShowInit - Inititalize module show
 *
 * RETURNS OK or ERROR
 ******************************************************************************/

STATUS moduleShowInit(void)
{
  return classShowConnect(moduleClassId, (FUNCPTR) moduleShow);
}

/*******************************************************************************
 * moduleShow - Show module info
 *
 * RETURNS OK or ERROR
 ******************************************************************************/

STATUS moduleShow(void *modNameOrId, int options)
{
  MODULE_ID modId;

  /* If id or name given */
  if (modNameOrId != NULL) {

    modId = moduleIdFigure(modNameOrId); 
    if (modId == NULL) {

      fprintf(stderr, "Module not found\n");
      return ERROR;

    }

    /* Print header */
    printf("%s", moduleShowHeader[0]);

    /* Default options */
    if (options == 0)
      options = MODDISPLAY_ALL;

    /* Show module info */
    moduleDisplay(modId, options);

  } /* End if id or name given */

  /* Else id or name not given */
  else {

    /* Print header */
    printf("%s", moduleShowHeader[0]);

    moduleEach((FUNCPTR) moduleDisplay, (ARG) options);

  } /* End else id or name not given */

  return OK;
}

/*******************************************************************************
 * moduleDisplay - Display module information
 *
 * RETURNS OK or ERROR
 ******************************************************************************/

LOCAL STATUS moduleDisplay(MODULE_ID modId, int options)
{
  MODULE_SEG_INFO segInfo;

  /* If hidden module */
  if (modId->flags & HIDDEN_MODULE)
    return TRUE;

  /* Get module info */
  if (moduleSegInfoGet(modId, &segInfo) == ERROR) {

    fprintf(stderr, "Can't get information about module %#x\n", modId);
    return FALSE;

  }

  /* Print */
  printf(moduleShowFormat,
	 modId->name,
	 (int) modId,
	 modId->group,
	 segInfo.textAddr,
	 segInfo.dataAddr,
	 segInfo.bssAddr);

  /* If codesize display */
  if (options & MODDISPLAY_CODESIZE) {

    printf("\n");
    printf("Size of text segment: %8d\n", segInfo.textSize);
    printf("Size of data segment: %8d\n", segInfo.dataSize);
    printf("Size of bss  segment: %8d\n", segInfo.bssSize);
    printf("Total size          : %8d\n\n",
	   segInfo.textSize + segInfo.dataSize + segInfo.bssSize);

  } /* End if codesize display */

  return TRUE;
}

