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

/* eventShow.c - Event show library */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/taskInfo.h>
#include <rtos/eventLib.h>
#include <rtos/eventShow.h>

/* Defines */

/* Imports */

/* Locals */

/* Globals */

/* Functions */

/*******************************************************************************
 * eventTaskShow - Show events for a task
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS eventTaskShow(EVENTS *pEvt)
{
  printf("\n"
         "Real rtos Events\n"
         "---------------\n");

  /* If no events pending */
  if ( (pEvt->sysflags & EVENTS_SYSFLAGS_WAITING) == 0 ) {

    printf("Events Pended on    : Not Pended\n"
           "Received Events     : 0x%x\n"
           "Options             : N/A\n",
	   pEvt->received);

  } /* End if no events pedning */

  /* Else events pending */
  else {

    printf("Events Pended on    : 0x%x\n"
           "Received Events     : 0x%x\n"
           "Options             : 0x%x\n",
	   pEvt->wanted,
	   pEvt->received,
           pEvt->options);

  } /* End events pending */

  /* Print options string */
  if ( (pEvt->options & EVENTS_WAIT_MASK) == EVENTS_WAIT_ALL )
    printf("EVENTS_WAIT_ALL\n");
  else
    printf("EVENTS_WAIT_ANY\n");

  if ( (pEvt->options & EVENTS_RETURN_ALL) != 0 )
    printf("\t\t\t\tEVENTS_RETURN_ALL\n");

  if ( (pEvt->options & EVENTS_KEEP_UNWANTED) != 0 )
    printf("\t\t\t\tEVENTS_KEEP_UNWANTED\n");

  return OK;
}

/*******************************************************************************
 * eventResourceShow - Show resource event info
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS eventResourceShow(EVENTS_RESOURCE *pEvRes)
{
  char *name;
  char tName[12];

  printf("\n"
         "Real rtos Events\n"
         "---------------\n");

  /* If null task */
  if (pEvRes->taskId == (int) NULL) {

    printf("Registered Task     : NONE\n"
	   "Event(s) to Send    : N/A\n"
	   "Options             : N/A\n");

  } /* End if null task */

  /* Else non null task */
  else {

    /* Get task name */
    name = taskName(pEvRes->taskId);
    if (name == NULL)
      strcpy(tName, "Deleted!");
    else
      strcpy(tName, name);

    printf("Registered Task     : 0x%x\t(%s)\n"
	   "Event(s) to Send    : 0x%x\n"
	   "Options             : 0x%x\t\n",
	   pEvRes->taskId,
	   tName,
	   pEvRes->registered,
	   pEvRes->options);

    /* If no event options */
    if (pEvRes->options & EVENTS_OPTIONS_NONE) {

      printf("EVENTS_OPTIONS_NONE\n");

    } /* End if no event options */

    /* Else event options */
    else {

      if ( (pEvRes->options & EVENTS_SEND_ONCE) != 0 )
        printf("EVENTS_SEND_ONCE\n\t\t\t\t");

      if ( (pEvRes->options & EVENTS_ALLOW_OVERWRITE) != 0 )
        printf("EVENTS_ALLOW_OVERWRITE\n\t\t\t\t");

      if ( (pEvRes->options & EVENTS_SEND_IF_FREE) != 0 )
        printf("EVENTS_SEND_IF_FREE\n\t\t\t\t");

    } /* End else event options */

  } /* End else non null task */

  return OK;
}

