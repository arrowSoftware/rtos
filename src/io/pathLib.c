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

/* pathLib.c - Path name functions */

#include <rtos.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <io/iosLib.h>
#include <io/pathLib.h>

/* Locals */
LOCAL BOOL pathLibInitialized = FALSE;
LOCAL char pathCwd[PATH_MAX + 1];

LOCAL void pathParse(char *longname, char **namearray, char *namebuf);
LOCAL void pathArrayReduce(char **namearray);
LOCAL STATUS pathBuild(char **namearray, char **arrayend, char *result);
LOCAL STATUS pathCat(char *dirname, char *filename, char *result);
LOCAL char* pathSlashLastIndex(char *str);

/***************************************************************************
 *
 * pathLibInit - initialize the path library
 *
 * RETURNS: OK
 */

STATUS pathLibInit (void) {
    if (pathLibInitialized) {
        return (OK);
    }

    strcpy (pathCwd, "");
    pathLibInitialized = TRUE;
}

/*******************************************************************************
 * ioDefPathSet - Set current working directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS ioDefPathSet(char *path)
{
  char *pTail;

  /* Initialize locals */
  pTail = path;

  /* Find device */
  if (iosDevFind(path, &pTail) == NULL)
    return ERROR;

  if (pTail == path)
    return ERROR;

  if (strlen(path) > PATH_MAX)
    return ERROR;

  strcpy(pathCwd, path);

  return OK;
}

/*******************************************************************************
 * ioDefPathGet - Get current working directory
 *
 * RETURNS: N/A
 ******************************************************************************/

void ioDefPathGet(char *path)
{
  strcpy(path, pathCwd);
}

/*******************************************************************************
 * ioDefPathCat - Conatenate path to current working directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS ioDefPathCat(char *path)
{
  char newpath[PATH_MAX];
  char *pTail;

  /* Concatenate path to current */
  if (pathCat(pathCwd, path, newpath) == ERROR)
    return ERROR;

  /* Verify that path starts with device name */
  iosDevFind(newpath, &pTail);
  if (pTail == newpath)
    return ERROR;

  /* Resolve path */
  pathCondense(newpath);

  strncpy(pathCwd, newpath, PATH_MAX);

  return OK;
}

/*******************************************************************************
 * chdir - Set current working directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

int chdir(char *path)
{
  return ioDefPathSet(path);
}

/***************************************************************************
 *
 * getcwd - get current working directory
 *
 * It is assumed that the current working directory will always have a
 * trailing '/' character.
 *
 * RETURNS: <buf> on success, NULL otherwise
 */

char * getcwd (
    char *  buf,
    size_t  size
    ) {
    int  len;

    if (size <= 0) {
        errnoSet (EINVAL);
        return (NULL);
    }

    len = strlen (pathCwd);
    if (len > size) {
        errnoSet (ERANGE);
        return (NULL);
    }

    strcpy (buf, pathCwd);

    return (buf);
}

/*******************************************************************************
 * pathCwdLen - Get length of working directory
 *
 * RETURNS: Length
 ******************************************************************************/

int pathCwdLen(void)
{
  return strlen(pathCwd);
}

/*******************************************************************************
 * pathPrependCwd - Prepend current working directory for filename
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS pathPrependCwd(char *filename, char *fullPath)
{
  return pathCat(pathCwd, filename, fullPath);
}

/*******************************************************************************
 * pathSplit - Split path into directory and filename
 *
 * RETURNS: N/A
 ******************************************************************************/

void pathSplit(char *path, char *dirname, char *filename)
{
  char *p;
  int n;

  /* If empty path */
  if (path == NULL) {

    strcpy(dirname, "");
    strcpy(filename, "");

    return;

  } /* End if empty path */

  /* Get last slash index */
  p = pathSlashLastIndex(path);
  if (p == NULL) {

    strcpy(dirname, "");
    strcpy(filename, path);

    return;

  }

  /* Get index */
  n = p - path;

  /* Copy directory name */
  strncpy(dirname, path, n);

  /* Copy path name */
  dirname[n] = EOS;
  strcpy(filename, ++p);
}

/*******************************************************************************
 * pathCondense - Process path
 *
 * RETURNS: N/A
 ******************************************************************************/

void pathCondense(char *path)
{
  char *nameArray[10];
  char nameBuffer[PATH_MAX];
  char newPath[PATH_MAX];
  char *pTail;

  /* Remove devicename */
  iosDevFind(path, &pTail);

  /* Parse path into directory name */
  pathParse(pTail, nameArray, nameBuffer);

  /* Remove "." and ".." in path */
  pathArrayReduce(nameArray);

  /* Build path */
  if ( (*pTail == '/') ||
       (*pTail == '\\') ) {

    newPath[0] = *pTail;
    newPath[1] = EOS;

  }

  else {

    newPath[0] = EOS;

  }

  pathBuild(nameArray, (char **) NULL, newPath);

  strcpy(pTail, newPath);
}

/*******************************************************************************
 * pathParse - Parse path
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void pathParse(char *longname, char **namearray, char *namebuf)
{
  char *pName, *p0, *p1;
  int nChars, nameCount;

  /* Initialize locals */
  pName = namebuf;
  nameCount = 0;
  nameCount = 0;
  p0 = longname;
  p1 = longname;

  /* While not end-of-string */
  while (*p0 != EOS) {

    /* Move to next separator */
    while ( (*p1 != '/') &&
	    (*p1 != '\\') &&
	    (*p1 != EOS) )
      p1++;

    /* If no advancement made */
    if (p0 == p1) {

      p0++;
      p1++;
      continue;

    }

    /* Get number of characters */
    nChars = p1 - p0;

    /* Copy dir name */
    memcpy(pName, p0, nChars);
    pName[nChars] = EOS;

    /* Store in namearray */
    namearray[nameCount] = pName;

    /* Advance */
    pName += nChars + 1;

    /* If separator */
    if ( (*p1 == '/') ||
	 (*p1 == '\\') )
      p1++;

    /* Advance */
    p0 = p1;
    nameCount++;

  }

  /* If zero separators */
  if (nameCount == 0) {

    strcpy(namebuf, ".");
    *namearray = namebuf;
    nameCount++;

  }

  /* Terminate array */
  namearray[nameCount] = NULL;
}

/*******************************************************************************
 * pathArrayReduce - Remove "." and ".." from path
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void pathArrayReduce(char **namearray)
{
  char *pString;
  char **pArray, **ppPrevString;

  /* Initialize locals */
  pArray = namearray;

  /* While more pices in array */
  while ( (pString = *pArray) != NULL) {

    if (strcmp(pString, ".") == 0) {

      *pString = EOS;

    }

    else if (strcmp(pString, "..") == 0) {

      *pString = EOS;

      ppPrevString = pArray - 1;
      while (ppPrevString >= namearray) {

        if (**ppPrevString == EOS) {

          ppPrevString--;

        }

        else {

          **ppPrevString = EOS;
          break;

        }

      }

    }

    pArray++;

  }

}

/*******************************************************************************
 * pathBuild - Build path
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS pathBuild(char **namearray, char **arrayend, char *result)
{
  char **pp;
  int len, newlen;
  BOOL slashEnd;

  /* Initialize locals */
  len = strlen(result);
  newlen = 0;
  slashEnd = FALSE;

  if (len >= PATH_MAX)
    return ERROR;

  if ( (len > 0) &&
       (result[len - 1] != '/') &&
       (result[len - 1] != '\\') ) {

    result[len] = '/';
    result[len + 1] = EOS;
    slashEnd = TRUE;

  }

  for (pp = namearray;
       (pp != NULL) && (*pp != NULL) && (pp != arrayend);
       pp++) {

    if (**pp != EOS) {

      newlen = (len + strlen(*pp) + 1);
      if (newlen > PATH_MAX)
        return ERROR;

      strcat(result, *pp);
      strcat(result, "/");
      slashEnd = TRUE;
      len = newlen;

    }

  }

  if (slashEnd)
    result[strlen(result) - 1] = EOS;

  return OK;
}

/*******************************************************************************
 * pathCat - Contatenate directory path and file name
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS pathCat(char *dirname, char *filename, char *result)
{
  char *pTail;

  /* If no filename given */
  if ( (filename == NULL) ||
       (filename[0] == EOS) ) {

    strcpy(result, dirname);
    return OK;

  }

  /* If no dirname given */
  if ( (dirname == NULL) ||
       (dirname[0] == EOS) ) {

    strcpy(result, filename);
    return OK;

  }

  /* If filename starts with a device name */
  iosDevFind(filename, &pTail);
  if (pTail != filename) {

    strcpy(result, filename);
    return OK;

  }

  /* Terminate string */
  *result = EOS;

  if (dirname[0] != '/') {

    if (iosDevFind(dirname, &pTail) == NULL)
      pTail = dirname;
    if (pTail != dirname) {

      strncat(result, dirname, pTail - dirname);
      dirname = pTail;

    }

  }

  /* If filename is relative */
  if ( (strchr("/\\~$", filename[0]) == NULL) &&
       (dirname[0] != EOS) ) {

    strncat(result, dirname, PATH_MAX);

    if (dirname[strlen(dirname) - 1] != '/')
      strncat(result, "/", PATH_MAX);

  }

  /* Concatenate filename */
  strncat(result, filename, PATH_MAX);

  return OK;
}

/*******************************************************************************
 * pathSlashLastIndex - Find last slash in name
 *
 * RETURNS: Pointer char or NULL
 ******************************************************************************/

LOCAL char* pathSlashLastIndex(char *str)
{
  char *f, *b;

  f = strrchr(str, '/');
  b = strrchr(str, '\\');

  return (max(f, b));
}

