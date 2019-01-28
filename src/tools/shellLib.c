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

/* shellLib.c - Shell library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <io/ioLib.h>
#include <os/excLib.h>
#include <tools/shellLib.h>

/* Defines */
#define MAX_PROMPT_LEN		80
#define MAX_SHELL_LINE		128

/* Imports */
IMPORT void yystart(char *line);
IMPORT int yyparse(void);

/* Globals */
int shellTaskId;
char *shellTaskName = "tShell";
int shellTaskPriority = 1;
int shellTaskOptions = TASK_OPTIONS_UNBREAKABLE;
int shellTaskStackSize = 20000;
int redirInFd = -1;
int redirOutFd = -1;

/* Locals */
LOCAL BOOL shellLibInstalled = FALSE;
LOCAL char promptString[MAX_PROMPT_LEN] = "-> ";
LOCAL BOOL shellAbort = FALSE;
LOCAL BOOL startRemoteSession = FALSE;
LOCAL FUNCPTR loginFunc = NULL;
LOCAL int loginValue = 0;
LOCAL FUNCPTR logoutFunc = NULL;
LOCAL int logoutValue = 0;
LOCAL BOOL shellLocked = FALSE;
LOCAL BOOL shellExecuting = FALSE;
LOCAL origFds[3];

LOCAL STATUS getRedir(char *line, int *pInFd, int *pOutFd);
LOCAL void stringTrimRight(char *str);
LOCAL STATUS getSingleRedir(char *line, int *pInFd, int *pOutFd);
LOCAL STATUS execShell(BOOL interactive);

/*******************************************************************************
 * shellLibInit - Initialize shell library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS shellLibInit(int stackSize, ARG arg)
{
  /* Check if already installed */
  if (shellLibInstalled)
    return OK;

  /* Not a remote shell */
  startRemoteSession = FALSE;

  /* Mark as installed */
  shellLibInstalled = TRUE;

  /* If shell spawn fails */
  if ( shellSpawn(stackSize, arg) != OK ) {

    shellLibInstalled = FALSE;
    return ERROR;

  } /* End if shell spawn fails */

  return OK;
}

/*******************************************************************************
 * shellSpawn - Respawn shell task
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS shellSpawn(int stackSize, ARG arg)
{
  /* If shell library already installed */
  if (shellLibInstalled) {

    /* Check if shell task already running */
    if ( taskNameToId(shellTaskName) != ERROR )
      return ERROR;

  } /* End if shell library already installed */

  /* Else shell library not installed */
  else {

    if ( shellLibInit(stackSize, arg) != OK )
      return ERROR;

    return OK;

  } /* End else shell library not installed */

  /* Check stack size */
  if (stackSize != 0)
    shellTaskStackSize = stackSize;

  /* Spawn shell task */
  shellTaskId = taskSpawn(shellTaskName, shellTaskPriority,
			  shellTaskOptions, shellTaskStackSize,
			  (FUNCPTR) shell,
			  arg,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0,
			  (ARG) 0);
  if (shellTaskId == (int) NULL)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * shellLoginInstall - Install shell login function
 *
 * RETURNS: N/A
 ******************************************************************************/

void shellLoginInstall(FUNCPTR func, int value)
{
  loginFunc = func;
  loginValue = value;
}

/*******************************************************************************
 * shellLogin - Login using user login function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS shellLogin(int fd)
{
  /* If login function set */
  if (loginFunc != NULL) {

    /* Set task filedescriptors */
    ioTaskStdSet(0, STDIN_FILENO, fd);
    ioTaskStdSet(0, STDOUT_FILENO, fd);

    /* Call user login function */
    if ( ( *loginFunc) (loginValue) == ERROR )
      return ERROR;

    printf("\n\n");
    return OK;

  } /* End if login function set */

  return OK;
}

/*******************************************************************************
 * shellLogoutInstall - Install shell logout function
 *
 * RETURNS: N/A
 ******************************************************************************/

void shellLogoutInstall(FUNCPTR func, int value)
{
  logoutFunc = func;
  logoutValue = value;
}

/*******************************************************************************
 * shellLogout - Logout using user logout function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS shellLogout(void)
{
  /* Unlock shell */
  shellLock(FALSE);

  /* If logout function set */
  if (logoutFunc != NULL)
    ( *logoutFunc) (logoutValue);
}

/*******************************************************************************
 * shellLock - Lock/unlock shell
 *
 * RETURNS: TRUE if mode changed else FALSE
 ******************************************************************************/

BOOL shellLock(BOOL lock)
{
  /* If already locked/unlocked */
  if (lock == shellLocked)
    return FALSE;

  shellLocked = lock;

  return TRUE;
}

/*******************************************************************************
 * shell - Start shell
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS shell(BOOL interactive)
{
  /* If shell is not executing */
  if (!shellExecuting) {

    /* If interactive shell */
    if (interactive) {

      /* Store original standard filedescriptors */
      origFds[STDIN_FILENO] = ioGlobalStdGet(STDIN_FILENO);
      origFds[STDOUT_FILENO] = ioGlobalStdGet(STDOUT_FILENO);
      origFds[STDERR_FILENO] = ioGlobalStdGet(STDERR_FILENO);

      /* Unlock shell */
      shellLock(FALSE);

    } /* End if interactive shell */

    /* Else non-interacrive shell */
    else {

      /* Lock for possible remote logins */
      shellLock(TRUE);

    } /* End else non-interacrive shell */

    /* Mark shell as executing */
    shellExecuting = TRUE;

  } /* End if shell not is executing */

  /* Else executing must be a restart */
  else {

    /* If interactive shell */
    if (interactive) {

      /* Store original standard filedescriptors */
      origFds[STDIN_FILENO] = ioGlobalStdGet(STDIN_FILENO);
      origFds[STDOUT_FILENO] = ioGlobalStdGet(STDOUT_FILENO);
      origFds[STDERR_FILENO] = ioGlobalStdGet(STDERR_FILENO);

      printf("\n");

    } /* End if interactive shell */

    /* Else non-interactive shell */
    else {

      return;

    } /* End else non-interactive shell */

  } /* End else executing must be a restart */

  /* Make sure input is setup as a terminal */
  ioctl(STDIN_FILENO, FIOOPTIONS, OPT_TERMINAL);

  /* Execute shell */
  execShell(interactive);

  /* When we arrive here shell has ended */
  shellExecuting = FALSE;
}

/*******************************************************************************
 * execute - Execute shell line
 *
 * RETURNS: N/A
 ******************************************************************************/

STATUS execute(char *line)
{
  int oldInFd, oldOutFd;
  int newInFd, newOutFd;
  STATUS status;

  /* Initialize locals */
  oldInFd = ioGlobalStdGet(STDIN_FILENO);
  oldOutFd = ioGlobalStdGet(STDOUT_FILENO);

  /* Get redirection */
  if (getRedir (line, &newInFd, &newOutFd) != OK)
    return ERROR;

  /* If empty line */
  if (*line == EOS) {

    status = execShell(FALSE);

    /* Restore old filedescriptors */
    ioGlobalStdSet(STDIN_FILENO, oldInFd);
    ioGlobalStdSet(STDOUT_FILENO, oldOutFd);

  } /* End if empty line */

  /* Else non empty line */
  else {

    /* Store redirect file descriptos */
    redirInFd = newInFd;
    redirOutFd = newOutFd;

    /* Parse line */
    yystart(line);
    if ( yyparse() == 0 )
      status = OK;
    else
      status = ERROR;

  } /* Else non empty line */

  /* If in redirected */
  if (newInFd >= 0)
    close(newInFd);

  /* If out redericted */
  if (newOutFd >= 0)
    if (close(newOutFd) == ERROR)
      fprintf(stderr, "can't close output.\n");

  return status;
}

/*******************************************************************************
 * getRedir - Get I/O redirection
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS getRedir(char *line, int *pInFd, int *pOutFd)
{
  /* Reset file descriptor pointers */
  *pInFd = -1;
  *pOutFd = -1;

  /* If in or out redirection */
  if ( (getSingleRedir (line, pInFd, pOutFd) != OK) ||
       (getSingleRedir (line, pInFd, pOutFd) != OK) ) {

    if (*pInFd >= 0)
      close(*pInFd);

    if (*pOutFd >= 0)
      close(*pOutFd);

    return ERROR;

  } /* End if in or out redirection */

  return OK;
}

/*******************************************************************************
 * getSingleRedir - Get a single redirection character
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS getSingleRedir(char *line, int *pInFd, int *pOutFd)
{
  int len;
  char *pLine;
  char *name;

  /* Get line lendth */
  len = strlen(line);

  /* If zero line length */
  if (len == 0)
    return OK;

  /* Goto end of line */
  pLine = line + len - 1;

  /* Get last word */
  while ( (pLine > line) && (*pLine == ' ') )
    pLine--;

  /* Terminate line here */
  *(pLine + 1) = EOS;

  /* Goto start of word */
  while ( (pLine > line) &&
	  (*pLine != ' ') &&
	  (*pLine != '<') && (*pLine != '>') &&
	  (*pLine != ')') && (*pLine != ';') &&
	  (*pLine != '"') )
    pLine--;

  /* Name is here */
  name = pLine + 1;

  /* Goto previous non blank */
  while ( (pLine > line) && (*pLine == ' ') )
    pLine--;

  /* If this is an output redirection */
  if ( (*pLine == '>') && (*(pLine - 1) != '>') ) {

    /* If output already open */
    if (*pOutFd >= 0) {

      fprintf(stderr, "ambiguous output redirect.\n");
      return ERROR;

    } /* End if output already open */

    /* First try to open file */
    *pOutFd = open(name, O_RDONLY, 0);
    if (*pOutFd < 0) {

      /* Try to creat file instead */
      *pOutFd = creat(name, O_WRONLY);
      if (*pOutFd < 0) {

        fprintf(stderr, "can't create output '%s'\n", name);
        return ERROR;

      }

    }

    /* Remove redirection */
    *pLine = EOS;

  } /* End if this is an output redirection */

  /* Else if this is an input redirection */
  else if ( (*pLine == '<') && ( (pLine == line) || ((*pLine - 1) != '<') ) ) {

    /* If input already open */
    if (*pInFd >= 0) {

      fprintf(stderr, "ambigious input redirect.\n");
      return ERROR;

    } /* End if input already open */

    *pInFd = open(name, O_RDONLY, 0);
    if (*pInFd < 0) {

      fprintf(stderr, "can't open input '%s'\n", name);
      return ERROR;

    }

    /* Remove redirection */
    *pLine = EOS;

  } /* End else if this is an input redirection */

  return OK;
}

/*******************************************************************************
 * stringTrimRight - Remove trailing space from a string
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void stringTrimRight(char *str)
{
  char *strPos;

  /* Initialize locals */
  strPos = str + strlen(str) - 1;

  /* While space left to trim */
  while (strPos > str) {

    if ( __isspace(*strPos) )
      strPos--;
    else
      break;

  } /* End while space left to trim */

  /* If position is at start of string */
  if (strPos == str) {

    /* If space */
    if ( __isspace(*strPos) ) {

      *strPos = EOS;
      return;

    } /* End if space */

  } /* End if position is at start of string */

  *(strPos + 1) = EOS;

}

/*******************************************************************************
 * execShell - Execute shell commands
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS execShell(BOOL interactive)
{
  char line[MAX_SHELL_LINE + 1];
  int i, nChars;
  STATUS status;

  /* If remote shell */
  if (startRemoteSession) {

    /* Will be started */
    startRemoteSession = FALSE;

  } /* End if start remote session */

  /* Reset abort flag */
  shellAbort = FALSE;

  /* While loop */
  while (1) {

    /* If interactive shell */
    if (interactive) {

      /* If unable to get stdin */
      if ( ioGlobalStdGet(STDIN_FILENO) == ERROR ) {

        taskDelay(1);
        continue;

      } /* End if unable to get stdin */

      /* Print prompt */
      printf("%s", promptString);

      /* Read input string */
      fgets(line, MAX_SHELL_LINE, stdin);
      nChars = strlen(line);

      /* If nothing read */
      if (nChars <= 0) {

	/* Restart shell */
        excJobAdd((VOIDFUNCPTR) taskRestart, (ARG) shellTaskId,
		  (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);

      } /* End if nothing read */

    } /* End if interactive shell */

    /* Terminate input string */
    line[MAX_SHELL_LINE] = EOS;

    /* Eat blanks */
    for (i = 0; __isspace(line[i]); i++);

    /* If line is not a comment */
    if (line[i] != '#' && line[i] != EOS) {

      /* Eat blanks */
      stringTrimRight(&line[i]);
      if (line[i] == EOS)
        continue;

      /* If builtin command */
      if ( strcmp(&line[i], "exit") == 0 ) {

        sprintf(line, "logout");
        i = 0;

      } /* End if builtin command */

      /* Execute shell command */
      status = execute(&line[i]);
      if (status != OK && !interactive) {

        status = ERROR;
        break;

      }

      /* If shell abort requested */
      if (shellAbort) {

        /* If not interactive shell */
        if (!interactive) {

          status = ERROR;
          break;

        } /* End if not interactive shell */

        /* Else interactive shell */
        else {

          shellAbort = FALSE;

        } /* End else interactive shell */

      } /* End if shell abort requested */

    } /* End if line is not a comment */

  } /* End while loop */

  return status;
}

