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

/* iosLib.c - I/O Serial library */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <rtos/errnoLib.h>
#include <io/ioLib.h>
#include <io/iosLib.h>

/* GLOBALS */
char iosDefaultPath[255];
int iosMaxDrv;
int iosMaxFd;
DRV_ENTRY *iosDrvTable;
FD_ENTRY *iosFdTable;
LIST iosDevList;
VOIDFUNCPTR iosFdNewHook = NULL;
VOIDFUNCPTR iosFdFreeHook = NULL;

/* LOCALS */
LOCAL BOOL iosLibInstalled = FALSE;
LOCAL SEMAPHORE iosSem;
LOCAL DEV_HEADER iosNullDevHeader;
LOCAL void iosLock(void);
LOCAL void iosUnlock(void);
LOCAL DEV_HEADER *iosDevMatch(char *name);
LOCAL STATUS iosNullWrite(int dummy, void *buf, int n);

/*******************************************************************************
* iosLibInit - Initialize library
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS iosLibInit(int max_drv,
		  int max_fd,
		  char *nullDevName)
{
  int i, size;

  /* Setup max variables */
  iosMaxDrv = max_drv;
  iosMaxFd = max_fd;

  /* Setup empty path */
  strcpy(iosDefaultPath, "");

  /* Init file table */
  size = iosMaxFd * sizeof(FD_ENTRY);

  iosFdTable = malloc(size);
  if (iosFdTable == NULL)
    return(ERROR);

  memset(iosFdTable, 0, size);
  for (i = 0; i < iosMaxFd; i++) {
    iosFdFree(STD_FIX(i));
  }

  /* Init driver table */
  size = iosMaxDrv * sizeof(DRV_ENTRY);

  iosDrvTable = (DRV_ENTRY *) malloc(size);
  if (iosDrvTable == NULL)
    return(ERROR);

  memset(iosDrvTable, 0, size);
  for (i = 0; i < iosMaxDrv; i++) {
    iosDrvTable[i].dev_used = FALSE;
  }

  /* Initialize semahpore */
  if (semBInit(&iosSem, SEM_Q_PRIORITY, SEM_FULL) != OK)
    return(ERROR);

  /* Initialize linked list */
  if (listInit(&iosDevList) != OK)
    return(ERROR);

  /* Add null device */
  iosDrvTable[0].dev_write = iosNullWrite;
  iosDevAdd(&iosNullDevHeader, nullDevName, 0);

  iosLibInstalled = TRUE;

  return(OK);
}

/*******************************************************************************
* iosDrvInstall - Install a driver
*
* RETURNS: Driver number or ERROR
*******************************************************************************/

int iosDrvInstall(FUNCPTR createMethod,
		  FUNCPTR deleteMethod,
		  FUNCPTR openMethod,
		  FUNCPTR closeMethod,
		  FUNCPTR readMethod,
		  FUNCPTR writeMethod,
		  FUNCPTR ioctlMethod)
{
  int drvNumber;
  DRV_ENTRY *pDrvEntry;

  iosLock();

  /* Find a free place in table */
  for (drvNumber = 1; drvNumber < iosMaxDrv; drvNumber++) {
    if (iosDrvTable[drvNumber].dev_used == FALSE) {
      pDrvEntry = &iosDrvTable[drvNumber];
      break;
    }
  }

  if (pDrvEntry == NULL) {
    errnoSet(S_iosLib_DRIVER_GLUT);
    iosUnlock();
    return(ERROR);
  }

  /* Setup struct */
  pDrvEntry->dev_used = TRUE;
  pDrvEntry->dev_create = createMethod;
  pDrvEntry->dev_delete = deleteMethod;
  pDrvEntry->dev_open = openMethod;
  pDrvEntry->dev_close = closeMethod;
  pDrvEntry->dev_read = readMethod;
  pDrvEntry->dev_write = writeMethod;
  pDrvEntry->dev_ioctl = ioctlMethod;

  iosUnlock();

  return(drvNumber);
}

/*******************************************************************************
* iosDrvRemove - Remove a driver
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS iosDrvRemove(int drvNumber, BOOL forceClose)
{
  DEV_HEADER *pDevHeader;
  int fd;
  FD_ENTRY *pFdEntry;
  DRV_ENTRY *pDrvEntry;
  FUNCPTR drvClose;

  pDrvEntry = &iosDrvTable[drvNumber];
  drvClose = pDrvEntry->dev_close;

  iosLock();

  /* Check if any open files */
  for (fd = 0; fd < iosMaxFd; fd++) {
    pFdEntry = &iosFdTable[fd];
    if (pFdEntry->used &&
	pFdEntry->pDevHeader->drvNumber == drvNumber) {
      if (!forceClose) {
        /* Don't close, ERROR */
        iosUnlock();
        return(ERROR);
      }
      else {
        /* Close file */
        if (drvClose != NULL) (*drvClose)(pFdEntry->value);
        iosFdFree(STD_FIX(fd));
      }
    }
  } /* End check open files */

  /* Remove devices for driver */
  for (pDevHeader = (DEV_HEADER *) LIST_HEAD(&iosDevList);
       pDevHeader != NULL;
       pDevHeader = (DEV_HEADER *) LIST_NEXT(&pDevHeader->node)) {
    if (pDevHeader->drvNumber == drvNumber) {
      free(pDevHeader->name);
      listRemove(&iosDevList, &pDevHeader->node);
    }
  }

  /* Clear struct */
  pDrvEntry->dev_used = FALSE;
  pDrvEntry->dev_create = NULL;
  pDrvEntry->dev_delete = NULL;
  pDrvEntry->dev_open = NULL;
  pDrvEntry->dev_close = NULL;
  pDrvEntry->dev_read = NULL;
  pDrvEntry->dev_write = NULL;
  pDrvEntry->dev_ioctl = NULL;

  iosUnlock();

  return(OK);
}

/*******************************************************************************
* iosDefaultPathSet - Initialize library
*
* RETURNS: OK or ERROR
*******************************************************************************/

void iosDefaultPathSet(char *name)
{
  strcpy(iosDefaultPath, name);
}

/*******************************************************************************
* iosDevAdd - Add a device entry
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS iosDevAdd(DEV_HEADER *pDevHeader,
		 char *name,
		 int drvNumber)
{
  DEV_HEADER *pMatch;

  /* Find best matching device */
  pMatch = iosDevMatch(name);

  /* Check if no full matching device exists */
  if ( (pMatch != NULL) && (strcmp(pMatch->name, name) == 0) ) {
    errnoSet(S_iosLib_DUPLICATE_DEVICE_NAME);
    return(ERROR);
  }

  /* Store name */
  pDevHeader->name = (char *) malloc(strlen(name) + 1);
  if (pDevHeader == NULL)
    return(ERROR);
  strcpy(pDevHeader->name, name);

  /* Store driver number */
  pDevHeader->drvNumber = drvNumber;

  iosLock();

  /* Add to list */
  listAdd(&iosDevList, &pDevHeader->node);

  iosUnlock();

  return(OK);
}

/*******************************************************************************
* iosDevDelete - Delete a device entry
*
* RETURNS: N/A
*******************************************************************************/

void iosDevDelete(DEV_HEADER *pDevHeader)
{
  iosLock();

  free(pDevHeader->name);
  listRemove(&iosDevList, &pDevHeader->node);

  iosUnlock();
}

/*******************************************************************************
* iosDevFind - Find a device by name
*
* RETURNS: Device header or NULL
*******************************************************************************/

DEV_HEADER *iosDevFind(char *name, char **pNameTail)
{
  DEV_HEADER *pDevHeader;

  /* Find best match */
  pDevHeader = iosDevMatch(name);

  if (pDevHeader != NULL) {
    /* Find match */
    *pNameTail = name + strlen(pDevHeader->name);
  }
  else {
    /* Get default match */
    pDevHeader = iosDevMatch(iosDefaultPath);
    *pNameTail = name;
  }

  if (pDevHeader == NULL) {
    errnoSet(S_iosLib_DEVICE_NOT_FOUND);
    return(NULL);
  }

  return(pDevHeader);
}

/*******************************************************************************
* iosDevMatch - Find device with matching name
*
* RETURNS: DEV_HEADER pointer or NULL
*******************************************************************************/

LOCAL DEV_HEADER *iosDevMatch(char *name)
{
  DEV_HEADER *pDevHeader, *pBestDevHeader = NULL;
  int len, maxLen = 0;

  iosLock();

  for (pDevHeader = (DEV_HEADER *) LIST_HEAD(&iosDevList);
       pDevHeader != NULL;
       pDevHeader = (DEV_HEADER *) LIST_NEXT(&pDevHeader->node)) {

    /* Get length */
    len = strlen(pDevHeader->name);

    if (strncmp(pDevHeader->name, name, len) == 0) {

      /* Name is within device name */
      if (len > maxLen) {
        pBestDevHeader = pDevHeader;
        maxLen = len;
      }
    }
  }

  iosUnlock();

  return(pBestDevHeader);
}

/*******************************************************************************
* iosNextDevGet - Get next device in list
*
* RETURNS: DEV_HEADER pointer or NULL
*******************************************************************************/

DEV_HEADER *iosNextDevGet(DEV_HEADER *pDevHeader)
{
  if (pDevHeader == NULL)
    return((DEV_HEADER *) LIST_HEAD(&iosDevList));

  return((DEV_HEADER *) LIST_NEXT(&pDevHeader->node));
}

/*******************************************************************************
* iosFdValue - Get driver specific value
*
* RETURNS: Driver value or ERROR
*******************************************************************************/

ARG iosFdValue(int fd)
{
  int f;

  /* Validate and get fd */
  f = STD_MAP(fd);

  if ( (f >= 0) && (f < iosMaxFd) && 
       (iosFdTable[f].used != FALSE) ) {
    return(iosFdTable[f].value);
  }

  errnoSet(S_iosLib_INVALID_FILE_DESCRIPTOR);
  return(NULL);
}

/*******************************************************************************
* iosFdDevFind - Get driver for file descriptor
*
* RETURNS: Deveice header pointer or ERROR
*******************************************************************************/

DEV_HEADER *iosFdDevFind(int fd)
{
  int f;

  /* Validate and get fd */
  f = STD_MAP(fd);

  if ( (f >= 0) && (f < iosMaxFd) && 
       (iosFdTable[f].used != FALSE) ) {
    return(iosFdTable[f].pDevHeader);
  }

  errnoSet(S_iosLib_INVALID_FILE_DESCRIPTOR);
  return(NULL);
}

/*******************************************************************************
* iosFdFree - Free a file descriptor
*
* RETURNS: N/A
*******************************************************************************/

void iosFdFree(int fd)
{
  FD_ENTRY *pFdEntry;
  int f;

  /* Get and validate fd */
  f = STD_MAP(fd);

  /* Get entry */
  pFdEntry = FD_CHECK(f);

  if (pFdEntry != NULL) {

    if (pFdEntry->name != NULL) {

      if (pFdEntry->name != pFdEntry->pDevHeader->name)
        free(pFdEntry->name);

      pFdEntry->name = NULL;
    }

    /* Call free hook */
    if (iosFdFreeHook != NULL)
      (*iosFdFreeHook)(fd);

    pFdEntry->used = FALSE;
  }
}

/*******************************************************************************
* iosFdSet - Set file descriptor
*
* RETURNS: N/A
*******************************************************************************/

STATUS iosFdSet(int fd,
		DEV_HEADER *pDevHeader,
		char *name,
		ARG value)
{
    FD_ENTRY *  pFdEntry;
    char *      ptr = NULL;

    if (fd >= iosMaxFd) {     /* Bounds check */
        return (ERROR);
    }

    /* Initialize locals */
    pFdEntry = &iosFdTable[STD_UNFIX(fd)];

    if (!pFdEntry->used) {    /* Do nothing if fd not in use. */
        return (ERROR);
    }

    /* Try allocating space for new name */
    if (name != NULL) {
        if ((ptr = (char *) malloc (strlen (name) + 1)) == NULL) {
            return (ERROR);
        }
    }

    if (pFdEntry->name != NULL) {
        free (pFdEntry->name);
    }

    pFdEntry->name = ptr;

    if (name != NULL) {
        if ((ptr = (char *) malloc (strlen (name) + 1)) == NULL) {
            return (ERROR);
        }
        strcpy (ptr, name);
    }

    pFdEntry->name = ptr;
    pFdEntry->pDevHeader = pDevHeader;
    pFdEntry->value = value;

    return(OK);
}

/*******************************************************************************
* ioFdNew - Allocate and initialize new file descriptor
*
* RETURNS: File descriptor or ERROR
*******************************************************************************/

int iosFdNew(DEV_HEADER *pDevHeader,
	     char *name,
	     ARG value)
{
  int fd;
  FD_ENTRY *pFdEntry;

  /* Initailize locals */
  pFdEntry = NULL;

  iosLock();

  /* Loop for free fd */
  for (fd  = 0; fd < iosMaxFd; fd++) {
    if (iosFdTable[fd].used == FALSE) {
      /* Fond one */
      pFdEntry = &iosFdTable[fd];
      pFdEntry->used = TRUE;
      pFdEntry->obsolete = FALSE;
      pFdEntry->name = NULL;
      break;
    }
  }

  iosUnlock();

  fd = STD_FIX(fd);

  if (fd >= iosMaxFd) {
    errnoSet(S_iosLib_TO_MANY_OPEN_FILES);
    return(ERROR);
  }

  if (iosFdSet(fd, pDevHeader, name, value) != OK)
    return(ERROR);

  /* Call hook */
  if (iosFdNewHook != NULL)
    (*iosFdNewHook)(fd);

  return(fd);
}

/*******************************************************************************
* iosCreate - Invoke driver function (if exists) to create file
*
* RETURNS: OK or ERROR
*******************************************************************************/

int iosCreate(DEV_HEADER *  pDevHeader,
	      char *        filename,
	      int           mode,
              const char *  symlink)
{
  FUNCPTR func;

  func = iosDrvTable[pDevHeader->drvNumber].dev_create;

  if (func != NULL)
    return ( (*func)(pDevHeader, filename, mode, symlink) );
  else
    return(OK);
}

/*******************************************************************************
* iosDelete - Invoke driver function (if exists) to delete file
*
* RETURNS: OK or ERROR
*******************************************************************************/

int iosDelete(DEV_HEADER *  pDevHeader,
	      char *        filename,
              mode_t        mode)
{
  FUNCPTR func;

  func = iosDrvTable[pDevHeader->drvNumber].dev_delete;

  if (func != NULL)
    return ( (*func)(pDevHeader, filename, mode) );
  else
    return(OK);
}

/*******************************************************************************
* iosOpen - Invoke driver function (if exists) to open file
*
* RETURNS: File descriptor or ERROR
*******************************************************************************/

int iosOpen(DEV_HEADER *pDevHeader,
	    char *filename,
	    int flags,
	    int mode)
{
  FUNCPTR func;

  func = iosDrvTable[pDevHeader->drvNumber].dev_open;

  if (func != NULL)
    return ( (*func)(pDevHeader, filename, flags, mode) );
  else
    return(OK);
}

/*******************************************************************************
* iosClose - Invoke driver function (if exists) to close file
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS iosClose(int fd)
{
  FD_ENTRY *pFdEntry;
  FUNCPTR func;
  int fDesc;
  STATUS status;

  fDesc = STD_MAP(fd);
  pFdEntry = FD_CHECK(fDesc);
  if (pFdEntry == NULL)
    return(ERROR);

  func = iosDrvTable[pFdEntry->pDevHeader->drvNumber].dev_close;

  if (func != NULL)
    status = (*func)(pFdEntry->value);
  else
    status = OK;

  /* Remove fd */
  iosFdFree(STD_FIX(fDesc));

  return(status);
}

/*******************************************************************************
* iosRead - Invoke driver function (if exists) to read file
*
* RETURNS: Bytes read
*******************************************************************************/

int iosRead(int fd,
	    void *buffer,
	    int maxBytes)
{
  FD_ENTRY *pFdEntry;
  FUNCPTR func;
  int fDesc;

  fDesc = STD_MAP(fd);
  pFdEntry = FD_CHECK(fDesc);
  if (pFdEntry == NULL)
    return(ERROR);

  func = iosDrvTable[pFdEntry->pDevHeader->drvNumber].dev_read;

  if (func != NULL) {
    return ( (*func)(pFdEntry->value, buffer, maxBytes) );
  }
  else {
    errnoSet(EOPNOTSUPP);
    return(ERROR);
  }
}

/*******************************************************************************
* iosWrite - Invoke driver function (if exists) to write file
*
* RETURNS: Bytes written
*******************************************************************************/

int iosWrite(int fd,
	     void *buffer,
	     int maxBytes)
{
  FD_ENTRY *pFdEntry;
  FUNCPTR func;
  int fDesc;

  fDesc = STD_MAP(fd);
  pFdEntry = FD_CHECK(fDesc);
  if (pFdEntry == NULL)
    return(ERROR);

  func = iosDrvTable[pFdEntry->pDevHeader->drvNumber].dev_write;

  if (func != NULL) {
    return ( (*func)(pFdEntry->value, buffer, maxBytes) );
  }
  else {
    errnoSet(EOPNOTSUPP);
    return(ERROR);
  }
}

/*******************************************************************************
* iosIoctl - Invoke driver function (if exists) ioctl
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS iosIoctl(int fd,
	        int function,
	        ARG arg)
{
  FD_ENTRY *pFdEntry;
  FUNCPTR func;
  int fDesc;

  fDesc = STD_MAP(fd);
  pFdEntry = FD_CHECK(fDesc);
  if (pFdEntry == NULL)
    return(ERROR);

  /* Check if name get */
  if (function == FIOGETNAME) {
    strcpy((char *) arg, pFdEntry->name);
    return(OK);
  }

  func = iosDrvTable[pFdEntry->pDevHeader->drvNumber].dev_ioctl;

  if (func != NULL) {
    return ( (*func)(pFdEntry->value, function, arg) );
  }
  else {

    if (function == FIONREAD) {
      * (char *) arg = 0;
      return(OK);
    }

    errnoSet(S_ioLib_UNKNOWN_REQUEST);
    return(ERROR);
  }
}

/*******************************************************************************
* iosLock - Get exclusive access to I/O struct
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void iosLock(void)
{
  semTake(&iosSem, WAIT_FOREVER);
}

/*******************************************************************************
* iosUnlock - Release exclusive access to I/O struct
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void iosUnlock(void)
{
  semGive(&iosSem);
}

/*******************************************************************************
* iosNullWrite - Null write
*
* RETURNS: OK or ERROR
*******************************************************************************/

LOCAL STATUS iosNullWrite(int dummy, void *buf, int n)
{
  return(n);
}

