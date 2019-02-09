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

/* envLib.c - Environment variable library */

/* Includes */
#include <stdlib.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <os/logLib.h>
#include <os/envLib.h>
#include <string.h>

/* Defines */
#define ENV_MAX_ENTRIES			50

/* Imports */

/* Locals */
LOCAL char** envFind(char *name, int namelen);
LOCAL void envCreateHook(TCB_ID tcbId);
LOCAL void envDeleteHook(TCB_ID tcbId);
LOCAL STATUS envDuplicate(char **srcEnv,
			  int srcEnvTableSize,
			  int srcNEnvVarEntries,
			  TCB_ID tcbId);
LOCAL void envDestroy(char **ppEnv, int nEnvVarEntries);

/* Globals */
char **ppGlobalEnviron;
int globalEnvTableSize;
int globalEnvNEntries;

/* Functions */

/*******************************************************************************
 * envLibInit - Initialize environment variable library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS envLibInit(BOOL installHooks)
{
  /* Allocate storage */
  ppGlobalEnviron = (char **) malloc(ENV_MAX_ENTRIES * sizeof(char *));
  if (ppGlobalEnviron == NULL)
    return ERROR;

  globalEnvTableSize = ENV_MAX_ENTRIES;
  globalEnvNEntries = 0;

  /* Optional hooks */
  if (installHooks) {

    if (taskCreateHookAdd((FUNCPTR) envCreateHook) != OK)
      return ERROR;

    if (taskDeleteHookAdd((FUNCPTR) envDeleteHook) != OK)
      return ERROR;

  }

}

/*******************************************************************************
 * envPrivateCreate - Create a private environment for task
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS envPrivateCreate(int taskId, int envSource)
{
  TCB_ID tcbId;
  char **ppEnv;

  /* Select source */
  switch(envSource) {

    /* Create new environment for task */
    case -1:
      ppEnv = (char **) malloc(ENV_MAX_ENTRIES * sizeof(char *));
      if (ppEnv == NULL)
        return ERROR;

      tcbId = taskTcb(taskId);
      if (tcbId == NULL)
        return ERROR;

      tcbId->envTableSize = ENV_MAX_ENTRIES;
      tcbId->nEnvVarEntries = 0;
      tcbId->ppEnviron = ppEnv;

    break;

    /* Duplicate global environment */
    case 0:
      if (envDuplicate(ppGlobalEnviron, globalEnvTableSize,
		       globalEnvNEntries, (TCB_ID) taskIdSelf()) == ERROR)
        return ERROR;

    break;

    /* Duplicate given current tasks environment */
    default:
      tcbId = taskTcb(taskId);
      if (tcbId == NULL)
        return ERROR;

      if (envDuplicate(tcbId->ppEnviron, tcbId->envTableSize,
		       tcbId->nEnvVarEntries, (TCB_ID) taskTcb(0)) == ERROR)
        return ERROR;

    break;

  }

  taskOptionsSet(taskId, TASK_OPTIONS_PRIVATE_ENV, TASK_OPTIONS_PRIVATE_ENV);

  return OK;
}

/*******************************************************************************
 * envPrivateDestroy - Free a tasks environment
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS envPrivateDestroy(int taskId)
{
  TCB_ID tcbId;

  tcbId = taskTcb(taskId);
  if (tcbId == NULL)
    return ERROR;

  envDestroy(tcbId->ppEnviron, tcbId->nEnvVarEntries);
  tcbId->ppEnviron = NULL;

  taskOptionsSet(taskId, TASK_OPTIONS_PRIVATE_ENV, 0);

  return OK;
}

/*******************************************************************************
 * getenv - Get environment
 *
 * RETURNS: Pointer to value string or NULL
 ******************************************************************************/

char* getenv(const char *name)
{
  char **pEnvLine;
  char *pValue;

  /* Find entry */
  pEnvLine = envFind((char *) name, strlen(name));
  if (pEnvLine == NULL)
    return NULL;

  /* Go beyound name */
  pValue = *pEnvLine + strlen(name);

  while ( (*pValue == ' ') ||
	  (*pValue == '\t') ||
	  (*pValue == '=') )
    pValue++;

  return pValue;
}

/*******************************************************************************
 * putenv - Put environment variable
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

int putenv(char *string)
{
  TCB_ID tcbId;
  int currEnvTableSize, currEnvNEntries;
  char **ppCurrEnv, **buf;
  char **ppEnvLine;
  char *pChar;

  /* Initialize locals */
  tcbId = taskTcb(0);
  pChar = string;

  while ( (*pChar != ' ') &&
	  (*pChar != '\t') &&
	  (*pChar != '=') &&
	  (*pChar != NULL) )
    pChar++;

  /* Find entry */
  ppEnvLine = envFind(string, pChar - string);
  if (ppEnvLine == NULL) {

    /* If global environment */
    if (tcbId->ppEnviron == NULL) {

      ppCurrEnv = ppGlobalEnviron;
      currEnvTableSize = globalEnvTableSize;
      currEnvNEntries = globalEnvNEntries;
      ppEnvLine = &ppGlobalEnviron[globalEnvNEntries++];

    }

    /* Else private environment */
    else {

      ppCurrEnv = tcbId->ppEnviron;
      currEnvTableSize = tcbId->envTableSize;
      currEnvNEntries = tcbId->nEnvVarEntries;
      ppEnvLine = &tcbId->ppEnviron[tcbId->nEnvVarEntries++];

    }

    /* Allocate more if needed */
    if (currEnvTableSize == currEnvNEntries) {

      buf = (char **) realloc((char *) ppCurrEnv,
	(unsigned) (currEnvTableSize + ENV_MAX_ENTRIES) * sizeof(char *));
      if (buf == NULL)
        return ERROR;

      /* Clear */
      memcpy(&buf, 0, ENV_MAX_ENTRIES * sizeof(char *));

      if (tcbId->ppEnviron == NULL) {

        ppGlobalEnviron = buf;
        globalEnvTableSize += ENV_MAX_ENTRIES;

      }

      else {

        tcbId->ppEnviron = buf;
        tcbId->envTableSize += ENV_MAX_ENTRIES;

      }

      ppEnvLine = &buf[currEnvNEntries];

    }

  }

  else {

    free(*ppEnvLine);

  }

  /* Allocate memory from string */
  *ppEnvLine = (char *) malloc(strlen(string) + 1);
  strcpy(*ppEnvLine, string);

  return OK;
}

/*******************************************************************************
 * envFind - Find environment variable
 *
 * RETURNS: Pointer to environment variable or NULL
 ******************************************************************************/

LOCAL char** envFind(char *name, int namelen)
{
  TCB_ID tcbId;
  int i, nEnvEntries;
  char **envVar;
  char endChar;

  /* Initialize locals */
  tcbId = taskTcb(0);

  /* If no local enironment */
  if (tcbId->ppEnviron == NULL) {

    envVar = ppGlobalEnviron;
    nEnvEntries = globalEnvNEntries;

  }

  /* Else local envirionment */
  else {

    envVar = tcbId->ppEnviron;
    nEnvEntries = tcbId->nEnvVarEntries;

  }

  /* For all entries */
  for (i = 0; i < nEnvEntries; i++, envVar++) {

    if (strncmp(name, *envVar, namelen) == 0) {

      endChar = (*envVar)[namelen];
      if ( (endChar == ' ') ||
	   (endChar == '\t') ||
	   (endChar == '=') ||
	   (endChar == NULL) )
        return envVar;

    }

  }

  return NULL;
}

/*******************************************************************************
 * envCreateHook - Environment variable create hook
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL void envCreateHook(TCB_ID tcbId)
{
  TCB_ID currTask;
  char **ppSrcEnviron;
  int srcEnvTableSize, srcNEnvVarEntries;

  if (tcbId->options & TASK_OPTIONS_PRIVATE_ENV) {

    currTask = taskTcb(0);

    /* If tasks environment */
    ppSrcEnviron = currTask->ppEnviron;
    if (ppSrcEnviron == NULL) {

      /* Get global environment */
      ppSrcEnviron = ppGlobalEnviron;
      srcEnvTableSize = globalEnvTableSize;
      srcNEnvVarEntries = globalEnvNEntries;

    }

    else {

      /* Get tasks environment */
      srcEnvTableSize = currTask->envTableSize;
      srcNEnvVarEntries = currTask->nEnvVarEntries;

    }

    /* Duplicate environment */
    if (envDuplicate(ppSrcEnviron, srcEnvTableSize,
		     srcNEnvVarEntries, tcbId) == ERROR) {

      logMsg("evnCreateHook: couldn't create private environment!\n",
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0,
	     (ARG) 0);
      taskOptionsSet((int) tcbId, TASK_OPTIONS_PRIVATE_ENV, 0);

    }

  }

  else {

    tcbId->ppEnviron = NULL;

  }

}

/*******************************************************************************
 * envDeleteHook - Environment variable delete hook
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL void envDeleteHook(TCB_ID tcbId)
{
  if (tcbId->options & TASK_OPTIONS_PRIVATE_ENV)
    if (tcbId->ppEnviron != NULL)
      envDestroy(tcbId->ppEnviron, tcbId->nEnvVarEntries);
}

/*******************************************************************************
 * envDuplicate - Duplicate a given environment
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS envDuplicate(char **srcEnv,
			  int srcEnvTableSize,
			  int srcNEnvVarEntries,
			  TCB_ID tcbId)
{
  int i;
  char *src, *dest;
  char **ppEnv;

  /* Allocate */
  ppEnv = (char **) malloc(srcEnvTableSize * sizeof(char *));
  if (ppEnv == NULL)
    return ERROR;

  /* Setup struct */
  tcbId->envTableSize = srcEnvTableSize;
  tcbId->nEnvVarEntries = 0;
  tcbId->ppEnviron = ppEnv;

  /* Copy environment */
  for (i = 0; i < srcNEnvVarEntries; i++) {

    src = srcEnv[i];
    dest = (char *) malloc(strlen(src) + 1);
    if (dest == NULL) {

     envDestroy(ppEnv, tcbId->nEnvVarEntries);
     return ERROR;

    }

    strcpy(dest, src);
    ppEnv[i] = dest;
    tcbId->nEnvVarEntries++;

  }

  return OK;
}

/*******************************************************************************
 * envDestroy - Free environment entry
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void envDestroy(char **ppEnv, int nEnvVarEntries)
{
  int i;

  for (i = 0; i < nEnvVarEntries; i++)
      free(ppEnv[i]);

  free(ppEnv);
}

