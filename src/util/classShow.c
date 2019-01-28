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

/* classShow.c - Class show utilities */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>
#include <rtos.h>
#include <os/symbol.h>
#include <os/symLib.h>
#include <util/classLib.h>
#include <util/classShow.h>

/* Defines */

/* Imports */
IMPORT SYMTAB_ID sysSymTable;

IMPORT FUNCPTR _func_symFindSymbol;
IMPORT FUNCPTR _func_symNameGet;
IMPORT FUNCPTR _func_symValueGet;

/* Locals */
LOCAL void classShowSymbol(void *value);

/* Globals */

/* Functions */

/*******************************************************************************
 * classShowInit - Initialize class show utilities
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS classShowInit(void)
{
  return classShowConnect(rootClassId, (FUNCPTR) classShow);
}

/*******************************************************************************
 * classShowSymbol - Show class symbolic value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void classShowSymbol(void *value)
{
  char *name;
  void *symValue;
  SYMBOL_ID symId;

  /* Initialize locals */
  name = NULL;
  symValue = NULL;

  /* If no value */
  if (value == NULL) {

    fprintf(stderr, "No routine attached.\n");
    return;

  } /* End if no value */

  /* If symbol find function set */
  if (_func_symFindSymbol != NULL) {

    /* If symbol found */
    if ( ( *_func_symFindSymbol) (sysSymTable, NULL, value,
				  SYM_MASK_NONE, SYM_MASK_NONE,
				 &symId) == OK ) {

      /* Get name and value */
      ( *_func_symNameGet) (symId, &name);
      ( *_func_symValueGet) (symId, &symValue);

    } /* End if symbol found */

  } /* End if symbol find function set */

  if (symValue == value)
    printf(" (%s)", name);

  printf("\n");
}

/*******************************************************************************
 * classShow - Show class status
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS classShow(CLASS_ID classId, int mode)
{
  /* Verify object class */
  if ( OBJ_VERIFY(classId, rootClassId) != OK )
    return ERROR;

  printf("\n");
  printf("Memory partition id : %#-10x", classId->objPartId);
  classShowSymbol(classId->objPartId);

  printf("Object Size         : %-10d\n", classId->objSize);
  printf("Objects Allocated   : %-10d\n", classId->objAllocCount);
  printf("Objects Deallocated : %-10d\n", classId->objFreeCount);
  printf("Objects Initialized : %-10d\n", classId->objInitCount);
  printf("Objects Terminated  : %-10d\n", classId->objTerminateCount);

  printf("Create Routine      : %-#10x", classId->createMethod);
  classShowSymbol(classId->createMethod);

  printf("Init Routine        : %-#10x", classId->initMethod);
  classShowSymbol(classId->initMethod);

  printf("Destroy Routine     : %-#10x", classId->destroyMethod);
  classShowSymbol(classId->destroyMethod);

  printf("Show Routine        : %-#10x", classId->showMethod);
  classShowSymbol(classId->showMethod);

  return OK;
}

