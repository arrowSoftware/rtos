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

/* usrLib.c - User functions used by shell */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <a.out.h>
#include <rtos.h>
#include <arch/regs.h>
#include <util/classLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskInfo.h>
#include <rtos/taskShow.h>
#include <rtos/kernLib.h>
#include <rtos/errnoLib.h>
#include <io/iosLib.h>
#include <io/iosShow.h>
#include <os/symbol.h>
#include <os/symLib.h>
#include <tools/moduleLib.h>
#include <tools/loadLib.h>
#include <usr/usrLib.h>

/* Defines */
#define MAX_BYTES_PER_LINE		16
#define MAXLINE				80
#define MAX_DISP_TASKS			500

/* Imports */
IMPORT TCB_ID taskIdCurrent;
IMPORT SYMTAB_ID sysSymTable;
IMPORT char *pKernExcStkBase;
IMPORT char *pKernExcStkEnd;
IMPORT FUNCPTR _func_symFindSymbol;
IMPORT FUNCPTR _func_symNameGet;
IMPORT FUNCPTR _func_symValueGet;

IMPORT int sysClockRateGet(void);

/* Gloabals */
int spTaskPriority = 100;
int spTaskOptions = 0;
int spTaskStackSize = 20000;
BOOL ldCommonMatchAll = TRUE;

/* Locals */
LOCAL BOOL usrLibInstalled = FALSE;

LOCAL void printStackSummary(TASK_DESC *pTd);

/*******************************************************************************
 * usrLibInit - Initialize user library
 *
 * RETURNS: OK er ERROR
 ******************************************************************************/

STATUS usrLibInit(void)
{
  int i;
  /* Check if already installed */
  if (usrLibInstalled)
    return OK;

  /* Mark as installed */
  usrLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * taskIdFigure - Translate task name or id to task id
 *
 * RETURNS: Task id
 ******************************************************************************/

int taskIdFigure(int taskNameOrId)
{
  char name[10];
  int taskId;

  /* If it is zero */
  if (taskNameOrId == 0)
    return taskIdDefault(0);

  /* Else if it is a valid task id */
  else if ( taskIdVerify(taskNameOrId) == OK )
    return taskNameOrId;

  /* Copy task name string */
  strcpy(name, (char *) taskNameOrId);

  taskId = taskNameToId(name);
  if (taskId == ERROR)
    return ERROR;

  return taskId;
}

/*******************************************************************************
 * show - Call object show method
 *
 * RETURNS: N/A
 ******************************************************************************/

void show(int objId, int level)
{

  /* Zero will default to current task */
  if (objId == 0)
    objId = (int) taskIdCurrent;

  /* If object show fails */
  if ( objShow(taskClassId, (OBJ_ID) objId, level) != OK ) {

    if ( errnoGet() == S_classLib_OBJ_NO_METHOD )
      fprintf(stderr, "Show routine of this object not configured into system.\n");
    else
      fprintf(stderr, "Object not found.\n");

  } /* End if object show fails */

}

/*******************************************************************************
 * help - Show help
 *
 * RETURNS: N/A
 ******************************************************************************/

void help(void)
{
  static char *helpMessages[] = {
    "help                           Print this list",
    "ioHelp                         Print I/O utilities help info",
    "i         [task]               Summary of tasks' TCBs",
    "ti        task                 Complete info on TCB for task",
    "sp        adr,args...          Spawn a task, pri=100, opt=0, stk=20000",
    "taskSpawn name,pri,opt,stk,adr,args... Spawn a task",
    "td        task                 Delete a task",
    "ts        task                 Suspend a task",
    "tr        task                 Resume a task",
    "d         [adr,[nunits[,width]]] Display memory",
    "m         adr[,width]          Modify memory",
    "pc        [task]               Return task's program counter",
    "devs                           List devices",
    "ld        [syms[,noAbort][,\"name\"]] Load stdin, or file into memory",
    "lkup      [\"substr\"]         List symbols in system symbol table",
    "checkStack  [task]             List task stack sizes and usage",
    "period    secs,adr,args... Spawn task to call function periodically",
    "repeat    n,adr,args...    Spawn task to call function n times (0=forever)",
    "version                        Print Real rtos version info",
    ""
  };
  int i;

  printf("\n");
  for (i = 0; i < NELEMENTS(helpMessages); i++) {

    printf(helpMessages[i]);
    printf("\n");

  }

  printf("\n");
}

/*******************************************************************************
 * t - Show task summary
 *
 * RETURNS: N/A
 ******************************************************************************/

void i(int taskNameOrId)
{
  int taskId;

  /* If task id is zero show summary */
  if (taskNameOrId == 0) {

    show(taskIdSelf(), 2);
    return;

  } /* End if task id is zero */

  taskId = taskIdFigure(taskNameOrId);
  if (taskId == ERROR) {

    fprintf(stderr, "Task not found.\n");
    return;

  }

  show(taskId, 0);
}

/*******************************************************************************
 * ti - Show task info
 *
 * RETURNS: N/A
 ******************************************************************************/

void ti(int taskNameOrId)
{
  int taskId;

  /* Get task id */
  taskId = taskIdFigure(taskNameOrId);
  if ( (taskId == ERROR) || (taskIdVerify(taskId) != OK) ) {

    fprintf(stderr, "Task not found.\n");
    return;

  }

  /* Set as default task id */
  taskId = taskIdDefault(taskId);

  show(taskId, 1);
}

/*******************************************************************************
 * sp - Spawn a task
 *
 * RETURNS: Task id or error
 ******************************************************************************/

int sp(FUNCPTR func,
       ARG arg1,
       ARG arg2,
       ARG arg3,
       ARG arg4,
       ARG arg5,
       ARG arg6,
       ARG arg7,
       ARG arg8,
       ARG arg9)
{
  int taskId;

  /* If task id is zero task id */
  if (func == (FUNCPTR) 0) {

    fprintf(stderr, "sorry, won't spawn task at PC = 0.\n");
    return ERROR;

  } /* End if task id is zero */

  /* Spawn task */
  taskId = taskSpawn(NULL,
		     spTaskPriority,
		     spTaskOptions,
		     spTaskStackSize,
		     (FUNCPTR) func,
		     arg1,
		     arg2,
		     arg3,
		     arg4,
		     arg5,
		     arg6,
		     arg7,
		     arg8,
		     arg9,
		     (ARG) 0);
  if (taskId == ERROR) {

    fprintf(stderr, "not able to spawn task.\n");
    return ERROR;

  }

  printf("task spawned: id = %#x, name = %s\n", taskId, taskName(taskId));

  return taskId;
}

/*******************************************************************************
 * ts - Suspend task
 *
 * RETURNS: N/A
 ******************************************************************************/

void ts(int taskNameOrId)
{
  int taskId;

  /* Get task id */
  taskId = taskIdFigure(taskNameOrId);
  if (taskId == ERROR) {

    fprintf(stderr, "Task not found.\n");
    return;

  }

  /* If task id self */
  if (taskId == 0 || taskId == taskIdSelf()) {

    fprintf(stderr, "sorry, the shell can't suspend itself.\n");
    return;

  } /* End if task id self */

  /* If action fails */
  if (taskSuspend(taskId) != OK)
    fprintf(stderr, "unable to suspend task.\n");
}

/*******************************************************************************
 * tr - Resume task
 *
 * RETURNS: N/A
 ******************************************************************************/

void tr(int taskNameOrId)
{
  int taskId;

  /* Get task id */
  taskId = taskIdFigure(taskNameOrId);
  if (taskId == ERROR) {

    fprintf(stderr, "Task not found.\n");
    return;

  }

  /* If action fails */
  if (taskResume(taskId) != OK)
    fprintf(stderr, "unable to resume task.\n");
}

/*******************************************************************************
 * td - Delete task
 *
 * RETURNS: N/A
 ******************************************************************************/

void td(int taskNameOrId)
{
  int taskId;

  /* Get task id */
  taskId = taskIdFigure(taskNameOrId);
  if (taskId == ERROR) {

    fprintf(stderr, "Task not found.\n");
    return;

  }

  /* If task id self */
  if (taskId == 0 || taskId == taskIdSelf()) {

    fprintf(stderr, "sorry, the shell can't delete itself.\n");
    return;

  } /* End if task id self */

  /* If action fails */
  if (taskDelete(taskId) != OK)
    fprintf(stderr, "unable to delete task.\n");
}

/*******************************************************************************
 * d - Display memory
 *
 * RETURNS: N/A
 ******************************************************************************/

void d(void *addr, int n, int size)
{
  static void *lastAddr = (void *) 0;
  static int lastSize = 2;
  static int lastN = 0x80;

  int i, count, itm;
  char line[MAX_BYTES_PER_LINE + 1];
  unsigned char *pByte;

  /* Terminate line */
  line[MAX_BYTES_PER_LINE] = EOS;

  /* If address non-zero */
  if (addr != (void *) 0)
    lastAddr = addr;

  /* if non-zero size */
  if (size != 0) {

    if (size != 1 &&
        size != 2 &&
        size != 4

#ifdef INCLUDE_LONG_LONG

	&&
        size != 8

#endif /* INCLUDE_LONG_LONG */

    )
      size = 1;

    /* Set new size */
    lastSize = size;

  } /* End if non-zero size */

  /* If non-zero number of units */
  if (n != 0)
    lastN = n;

  /* Set start address /*
  lastAddr = (void *) ( (int) lastAddr & ~(lastSize - 1) );

  /* Insert spaces at start of line */
  memset(line, '.', MAX_BYTES_PER_LINE);

  /* Print address */
  printf("%08x:  ", (int) lastAddr & ~0xf);

  /* For all items */
  for (itm = 0; itm < ( (int) lastAddr & 0xf) / lastSize; itm++) {

    printf("%*s ", 2 * lastSize, " ");
    memset(&line[itm * lastSize], ' ', 2 * lastSize);

  } /* End for all items */

  /* Store numner of items to print */
  count = lastN;

  /* While units left to print */
  while( (count--) > 0) {

    /* If end of line */
    if (itm == MAX_BYTES_PER_LINE / lastSize) {

      printf("  *%16s*\n%08x:  ", line, (int) lastAddr);
      memset(line, '.', MAX_BYTES_PER_LINE);
      itm = 0;

    } /* End if end of line */

    /* Select size */
    switch(lastSize) {

      case 1:
        printf("%02x", *(unsigned char *) lastAddr);
      break;

      case 2:
        printf("%04x", *(unsigned short *) lastAddr);
      break;

      case 4:
        printf("%08lx", *(unsigned long *) lastAddr);
      break;

#ifdef INCLUDE_LONG_LONG

      case 8:
        printf("%016lx", *(unsigned long long *) lastAddr);
      break;

#endif /* INCLUDE_LONG_LONG */

      default:
        printf("%02x", *(unsigned char *) lastAddr);
      break;

    } /* End select size */

    /* Add space between units */
    printf(" ");

    /* Set line buffer */
    pByte = (unsigned char *) lastAddr;

    /* For byte size */
    for (i = 0; i < lastSize; i++) {

      /* If character is printable */
      if ( *pByte == ' ' || ( __isascii(*pByte) && __isprint(*pByte) ) )
        line[itm * lastSize + i] = *pByte;

      /* Advance */
      pByte++;

    } /* End for byte size */

    /* Advance */
    lastAddr = (void *) ( (int) lastAddr + lastSize );
    itm++;

  } /* End while units left to print */

  /* Print end of last line */
  for (; itm < MAX_BYTES_PER_LINE / lastSize; itm++)
    printf("%*s ", 2 * lastSize, " ");

  printf("  *%16s*\n", line);
}

/*******************************************************************************
 * m - Modify memory
 *
 * RETURNS: N/A
 ******************************************************************************/

void m(void *addr, int size)
{
  static void *lastAddr = (void *) 0;
  static int lastSize = 2;
  char line[MAXLINE + 1];
  unsigned long value;

#ifdef INCLUDE_LONG_LONG

  long long lValue;

#endif /* INCLUDE_LONG_LONG */

  /* If address non-zero */
  if (addr != (void *) 0)
    lastAddr = addr;

  /* if non-zero size */
  if (size != 0) {

    if (size != 1 &&
        size != 2 &&
        size != 4

#ifdef INCLUDE_LONG_LONG

	&&
        size != 8

#endif /* INCLUDE_LONG_LONG */

    )
      size = 1;

    /* Set new size */
    lastSize = size;

  } /* End if non-zero size */

  /* Set start address /*
  lastAddr = (void *) ( (int) lastAddr & ~(lastSize - 1) );

  /* While loop */
  while (1) {

    /* Select size */
    switch(lastSize) {

      case 1:
        printf("%08x:  %02x-",
	       (int) lastAddr,
	       *(unsigned char *) lastAddr);
      break;

      case 2:
        printf("%08x:  %04x-",
	       (int) lastAddr,
	       *(unsigned short *) lastAddr);
      break;

      case 4:
        printf("%08x:  %08lx-",
	       (int) lastAddr,
	       *(unsigned long *) lastAddr);
      break;

#ifdef INCLUDE_LONG_LONG

      case 4:
        printf("%016x:  %08lx-",
	       (int) lastAddr,
	       *(unsigned long long *) lastAddr);
      break;

#endif /* INCLUDE_LONG_LONG */

      default:
        printf("%08x:  %02x-",
	       (int) lastAddr,
	       *(unsigned char *) lastAddr);
      break;

    } /* End select size */

    /* Get input */
    fgets(line, MAXLINE, stdin);
    if ( strlen(line) <= 1)
      break;

    /* Select size */
    switch(lastSize) {

      case 1:
        sscanf(line, "%x", &value);
        *(unsigned char *) lastAddr = (unsigned char) value;
      break;

      case 2:
        sscanf(line, "%x", &value);
        *(unsigned short *) lastAddr = (unsigned short) value;
      break;

      case 4:
        sscanf(line, "%lx", &value);
        *(unsigned long *) lastAddr = (unsigned long) value;
      break;

#ifdef INCLUDE_LONG_LONG

      case 8:
        sscanf(line, "%lx", &lValue);
        *(unsigned long long *) lastAddr = (unsigned long long) lValue;
      break;

#endif /* INCLUDE_LONG_LONG */

      default:
        sscanf(line, "%x", &value);
        *(unsigned char *) lastAddr = (unsigned char) value;
      break;

    } /* End select size */

    /* Advance */
    lastAddr = (void *) ( (int) lastAddr + lastSize );

  } /* End while loop */
}

/*******************************************************************************
 * pc - Get task program counter
 *
 * RETURNS: Task program counter or ERROR
 ******************************************************************************/

int pc(int taskNameOrId) 
{
  REG_SET regSet;
  int taskId;

  /* Get task id */
  taskId = taskIdFigure(taskNameOrId);
  if (taskId == ERROR)
    return ERROR;

  /* Set as default task */
  taskId = taskIdDefault(taskId);

  /* Get registers */
  if (taskRegsGet(taskId, &regSet) != OK)
    return ERROR;

  return ( (int) regSet.pc );
}

/*******************************************************************************
 * devs - Show a list of known devices
 *
 * RETURNS: N/A
 ******************************************************************************/

void devs(void)
{
  iosDevShow();
}

/*******************************************************************************
 * ld - Load object module into memory
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID ld(int symFlags, BOOL noAbort, char *name)
{
  MODULE_ID modId;
  int fd;
  int loadFlag;

  /* If filename given */
  if (name != NULL) {

    fd = open(name, O_RDONLY, 0);
    if (fd == ERROR) {

      fprintf(stderr, "ld error: unable to open \"%s\"\n", name);
      return NULL;

    }

  } /* End if filename given */

  /* Else used stdin */
  else {

    fd = STDIN_FILENO;

  } /* End else used stdin */

  /* Select load flags */
  switch (symFlags) {

    case NO_SYMBOLS:
      loadFlag = LOAD_NO_SYMBOLS;
    break;

    case GLOBAL_SYMBOLS:
      loadFlag = LOAD_GLOBAL_SYMBOLS;
    break;

    case ALL_SYMBOLS:
      loadFlag = LOAD_ALL_SYMBOLS;
    break;

    default:
      loadFlag = LOAD_ALL_SYMBOLS;
    break;

  } /* End select load flags */

  /* If match all symbols */
  if (ldCommonMatchAll)
    loadFlag = (loadFlag | LOAD_COMMON_MATCH_ALL);

  /* Load module */
  modId = loadModule(fd, loadFlag);

  /* Close file if opened */
  if (name != NULL)
    close(fd);

  /* If error */
  if (modId == NULL) {

    /* Select error */
    switch (errnoGet()) {

      case S_loadLib_TOO_MANY_SYMBOLS:
        fprintf(stderr, "ld error: to many symbols.\n");
      break;

      case S_symLib_SYMBOL_NOT_FOUND:
        fprintf(stderr, "ld error: Module contains undefined symbol(s) "
			"and may be unusable.\n");
      break;

      default:
        fprintf(stderr, "ld error: error loading file (errno = %#x).\n",
		errnoGet());
      break;

    } /* End select error */

  } /* End if error */

  return modId;
}

/*******************************************************************************
 * lkup - List symbols in system symbol table
 *
 * RETURNS: N/A
 ******************************************************************************/

void lkup(char *str)
{
  show((int) sysSymTable, (int) str);
}

/*******************************************************************************
 * periodicRun - Call a function periodically
 *
 * RETURNS: N/A
 ******************************************************************************/

void periodRun(int secs,
	       FUNCPTR func,
	       ARG arg1,
	       ARG arg2,
	       ARG arg3,
	       ARG arg4,
	       ARG arg5,
	       ARG arg6,
	       ARG arg7,
	       ARG arg8)
{

  /* While loop */
  while (1) {

    /* Call function */
    ( *func) (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

    /* Sleep */
    taskDelay(secs * sysClockRateGet());

  } /* End while loop */
}

/*******************************************************************************
 * checkStack - Print stack info
 *
 * RETURNS: N/A
 ******************************************************************************/

void checkStack(int taskNameOrId)
{
  static char checkStackHeader [] =
    "  NAME        ENTRY       TID    SIZE   CUR  HIGH  MARGIN\n"
    "---------- ------------ -------- ----- ----- ----- ------\n";
  int i, numTasks;
  char *pIntStackHigh;
  int taskId;
  TASK_DESC td;
  int idList[MAX_DISP_TASKS];
  char s[20];

  /* If task id is zero */
  if (taskNameOrId == 0) {

    /* Print stack info */
    printf(checkStackHeader);

    /* Get all tasks */
    numTasks = taskIdListGet(idList, NELEMENTS(idList));
    taskIdListSort(idList, numTasks);

    /* For all tasks */
    for (i = 0; i < numTasks; i++) {

      if ( taskInfoGet(idList[i], &td) == OK )
        printStackSummary(&td);

    } /* End for all tasks */

    /* Calculate interrupt stack usage */

#if (_STACK_DIR == _STACK_GROWS_DOWN)

    for (pIntStackHigh = pKernExcStkEnd;
	 *(unsigned char *) pIntStackHigh  == 0xee;
	 pIntStackHigh++);

#else /* _STACK_GROWS_UP */

    for (pIntStackHigh = pKernExcStkEnd - 1;
	 *(unsigned char *) pIntStackHigh == 0xee;
	 pIntStackHigh--);

#endif /* _STACK_DIR */

    /* Check for overflow */
    if ( (pIntStackHigh == pKernExcStkEnd) &&
	 (pKernExcStkEnd != pKernExcStkBase != 0) )
      strcpy(s, "OVERFLOW");
    else
      strcpy(s, "");

    /* Print interrupt stack usage */
    printf("%-11.11s%-12.12s", "INTERRUPT", "");
    printf(" %8s %5d %5d %5d %6d %s\n",
	   "",
	   (int) ( (pKernExcStkEnd - pKernExcStkBase) * _STACK_DIR ),
	   0,
	   (int) ( (pIntStackHigh - pKernExcStkBase) * _STACK_DIR ),
	   (int) ( (pKernExcStkEnd - pIntStackHigh) * _STACK_DIR ),
	   s);

  } /* End if task id is zero */

  /* Else task id non-zero */
  else {

    /* Get task id */
    taskId = taskIdFigure(taskNameOrId);
    if (taskId == ERROR) {

      fprintf(stderr, "Task not found.\n");
      return;

    }

    /* Get task info */
    if ( taskInfoGet(taskId, &td) != OK ) {

      fprintf(stderr, "Unable to get task info.\n");
      return;

    }

    /* Print stack info */
    printf(checkStackHeader);
    printStackSummary(&td);


  } /* End else task id non-zero */
}

/*******************************************************************************
 * period - Spawn a task that calls a function periodically
 *
 * RETURNS: Task id
 ******************************************************************************/

int period(int secs,
	   FUNCPTR func,
	   ARG arg1,
	   ARG arg2,
	   ARG arg3,
	   ARG arg4,
	   ARG arg5,
	   ARG arg6,
	   ARG arg7,
	   ARG arg8)
{

  return sp ( (FUNCPTR) periodRun, (ARG) secs, (ARG) func,
	      arg1,
	      arg2,
	      arg3,
	      arg4,
	      arg5,
	      arg6,
	      arg7);
}

/*******************************************************************************
 * repeatRun - Call a function a specified number of times
 *
 * RETURNS: N/A
 ******************************************************************************/

void repeatRun(int n,
	       FUNCPTR func,
	       ARG arg1,
	       ARG arg2,
	       ARG arg3,
	       ARG arg4,
	       ARG arg5,
	       ARG arg6,
	       ARG arg7,
	       ARG arg8)
{
  BOOL endless;

  if (n == 0)
    endless = TRUE;
  else
    endless = FALSE;

  /* While endless n greater than zero */
  while ( endless || (--n >= 0) )
    ( *func) (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

/*******************************************************************************
 * repeat - Spawn a task that calls a function a specified number of times
 *
 * RETURNS: Task id
 ******************************************************************************/

int repeat(int n,
	   FUNCPTR func,
	   ARG arg1,
	   ARG arg2,
	   ARG arg3,
	   ARG arg4,
	   ARG arg5,
	   ARG arg6,
	   ARG arg7,
	   ARG arg8)
{

  return sp ( (FUNCPTR) repeatRun, (ARG) n, (ARG) func,
	      arg1,
	      arg2,
	      arg3,
	      arg4,
	      arg5,
	      arg6,
	      arg7);

}

/*******************************************************************************
 * version - Printf kernel version
 *
 * RETURNS: N/A
 ******************************************************************************/

void version(void)
{
  printf("Kernel version: %s.\n", kernelVersion());
}

/*******************************************************************************
 * printLogo - Print the Real rtos logo
 *
 * RETURNS: N/A
 ******************************************************************************/

void printLogo(void)
{
  int i;
  static char *logo[] = {
"+-++-++-++-+   _           _  _           _  _           _ ",
"|R||e||a||l|  (_)         (_)(_) _     _ (_)(_)_       _(_)",
"+-++-++-++-+  (_)         (_)(_)(_)   (_)(_)  (_)_   _(_)  ",
"              (_)_       _(_)(_) (_)_(_) (_)    (_)_(_)    ",
"                (_)     (_)  (_)   (_)   (_)     _(_)_     ",
"                 (_)   (_)   (_)         (_)   _(_) (_)_   ",
"                  (_)_(_)    (_)         (_) _(_)     (_)_ ",
"                    (_)      (_)         (_)(_)         (_)"
};
                                       

  for (i = 0; i < NELEMENTS(logo); i++)
    printf("%s\n", logo[i]);

  printf("KERNEL: %s.\n", kernelVersion());
  printf("This system is released under GNU lesser general public license.\n");

  printf("\n");
}

/*******************************************************************************
 * printStackSummary - Print stack summary from task info
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void printStackSummary(TASK_DESC *pTd)
{
  char s[18];
  char *name;
  FUNCPTR func;
  void *value;
  SYMBOL_ID symId;

  /* Get task entry */
  func = pTd->td_entry;

  /* If symbol find function exists */
  if (_func_symFindSymbol != NULL) {

    /* If symbol found */
    if ( ( *_func_symFindSymbol) (sysSymTable, NULL, func,
		N_TEXT | N_EXT, N_TEXT | N_EXT,
		&symId) == OK) {

      ( *_func_symNameGet) (symId, &name);
      ( *_func_symValueGet) (symId, &value);

    } /* End if symbol found */

  } /* End if symbol find function exists */

  /* Print task name */
  printf("%-11.11s", pTd->td_name);

  /* Print symbol or function address */
  if (func == (FUNCPTR) value)
    printf("%-12.12s", name);
  else
    printf("%-12x", (int) func);

  /* Check for overflow */
  if (pTd->td_stackMargin <= 0)
    strcpy(s, "OVERFLOW");
  else
    strcpy(s, "");

  /* Print stack sizes */
  printf(" %8x %5d %5d ", pTd->td_id, pTd->td_stackSize, pTd->td_stackCurrent);

  /* Calculate stack usage */
  if (pTd->td_options & TASK_OPTIONS_NO_STACK_FILL)
    printf("%5s %6s NO_STACK_FILL\n", "???", "???");
  else
    printf("%5d %6d %s\n", pTd->td_stackHigh, pTd->td_stackMargin, s);
}

