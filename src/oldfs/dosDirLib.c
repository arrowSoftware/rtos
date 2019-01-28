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

/* dosDirLib.c - Dos directory library */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/semLib.h>
#include <os/logLib.h>
#include <io/cbioLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/private/dosFsLibP.h>
#include <oldfs/private/dosFatLibP.h>
#include <oldfs/private/dosDirLibP.h>

/* Defines */
//#define DEBUG_DIR

#define CHAR_PER_ENTRY		13
#define DOS_ATTR_VFAT		0x0f
#define DOS_LAST_ENTRY		0x40
#define DOS_VFAT_ENTRY_MASK	0x1f
#define DOS_VFAT_CHKSUM_OFF	13

#define DH_ALLOC		(1 << 7)

#define PUT_CURRENT		0x01
#define PUT_NEXT		0x02

#define MAX_DIRENT ((DOS_VFAT_NAME_LEN + CHAR_PER_ENTRY - 1) / CHAR_PER_ENTRY)
#define MAX_FULL_DIRENT (DOS_DIRENT_STD_LEN * (MAX_DIRENT) * (MAX_DIRENT + 1))

/* Imports */
IMPORT DOS_HDLR_DESC dosDirHdlrsList[];

IMPORT STATUS dosFsHdlrInstall(DOS_HDLR_DESC *hdlrList,
			       DOS_HDLR_DESC *hdlr);

/* Types */
typedef enum {
  STRICT_SHORT,
  NOT_STRICT_SHORT,
  NO_SHORT
} SHORT_ENCODE;

typedef enum {
  RD_FIRST,
  RD_CURRENT,
  RD_NEXT,
  RD_ENTRY
} RDE_OPTION;

/* Locals */
LOCAL unsigned char shortNameChars[] = /* 0123456789abcdef */
				         "||||||||||||||||"
				         "||||||||||||||||"
				         " !|#$%&'()|||-||"
				         "0123456789||||||"
				         "@ABCDEFGHIJKLMNO"
				         "PQRSTUVWXYZ|||^_"
				         "`ABCDEFGHIJKLMNO"
				         "PQRSTUVWXYZ{|}~|";

LOCAL unsigned char longNameChars[] =  /* 0123456789abcdef */
				        "||||||||||||||||"
				        "||||||||||||||||"
				        " !|#$%&'()|+,-.|"
				        "0123456789|;|=||"
				        "@ABCDEFGHIJKLMNO"
				        "PQRSTUVWXYZ[|]^_"
				        "`abcdefghijklmno"
				        "pqrstuvwxyz{|}~|";

LOCAL unsigned char charOffsets[] = {
  1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30
};

LOCAL STATUS dosDirVolMount(DOS_VOL_DESC_ID volId, ARG arg);
LOCAL void dosDirVolUnmount(DOS_VOL_DESC_ID volId);

LOCAL STATUS dosDirCharEncode(unsigned char *src,
			      unsigned char *dest,
			      const unsigned char *codeTable);
LOCAL SHORT_ENCODE dosDirNameEncodeShort(DOS_DIR_PDESC *pDirDesc,
					 PATH_ARRAY *nameArray,
					 unsigned char *pDestName);
LOCAL int dosDirNameEncode(DOS_DIR_PDESC *pDirDesc,
			   PATH_ARRAY *nameArray,
			   unsigned char *pDestName,
			   SHORT_ENCODE *pEncode);
LOCAL void dosDirNameDecodeShort(DOS_DIR_PDESC *pDirDesc,
				 unsigned char *pDirEnt,
				 unsigned char *pDestName);
LOCAL void dosDirNameDecode(DOS_DIR_PDESC *pDirDesc,
			    unsigned char *pDirEnt,
			    unsigned char *pDestName);

LOCAL void dosDirDateEncode(DOS_DIR_PDESC *pDirDesc,
			    unsigned char *pDirEnt,
			    unsigned int mask,
			    time_t currTime);
LOCAL time_t dosDirDateDecode(DOS_DIR_PDESC *pDirDesc,
			      unsigned char *pDirEnt,
			      unsigned int which);
LOCAL STATUS dosDirDateGet(DOS_FILE_DESC *pFd,
			   struct stat *pStat);
LOCAL STATUS dosDirVolLabel(DOS_VOL_DESC_ID volId,
			    unsigned char *label,
			    unsigned int req);
LOCAL STATUS dosDirNameChk(DOS_VOL_DESC_ID volId,
			   unsigned char *name);
LOCAL STATUS dosDirShow(DOS_VOL_DESC_ID volId);

LOCAL int dosDirPathParse(DOS_VOL_DESC_ID volId,
			  unsigned char *path,
			  PATH_ARRAY *nameArray);

LOCAL STATUS dosDirFullEntGet(DOS_FILE_DESC *pFd,
			      unsigned char *pEntry,
			      RDE_OPTION which,
			      DIRENT_PTR *pLname,
			      unsigned int *nEntries);

LOCAL STATUS dosDirLkupInDir(DOS_FILE_DESC *pFd,
			     PATH_ARRAY *nameArray,
			     DIRENT_PTR *pFreeEnt,
			     int *pFreeEntLen,
			     BOOL caseSens);

LOCAL void dosDirAliasCreate(DOS_DIR_PDESC * pDirDesc,
			     PATH_ARRAY *nameArray,
			     unsigned char *pNameAlias,
			     unsigned int entNum);
LOCAL STATUS dosDirFileCreateInDir(DOS_FILE_DESC *pFd,
				   PATH_ARRAY *nameArray,
				   unsigned int options,
				   DIRENT_PTR *pFreeEnt,
				   int freeEntLen);

LOCAL STATUS dosDirPathLkup(DOS_FILE_DESC *pFd,
			    void *path,
			    unsigned int options);

LOCAL void dosDirRewind(DOS_FILE_DESC *pFd);
LOCAL STATUS dosDirDirentGet(DOS_FILE_DESC *pFd,
			     unsigned char *pDirEnt,
			     RDE_OPTION which);
LOCAL STATUS dosDirReaddir(DOS_FILE_DESC *pFd,
			   struct dirent *pDir,
			   DOS_FILE_DESC *pResFd);
LOCAL STATUS dosDirEntryDel(DOS_VOL_DESC_ID volId,
			    DIRENT_PTR *pLname,
			    unsigned int nEnt,
			    BOOL force);
LOCAL STATUS dosDirUpdateEntry(DOS_FILE_DESC *pFd, int flags, time_t currTime);

LOCAL STATUS dosDirClustNext(DOS_FILE_DESC *pFd, unsigned int alloc);
LOCAL STATUS dosDirDeStore(DOS_FILE_DESC *pFd,
			   off_t offset,
			   size_t nBytes,
			   void *buffer,
			   unsigned int which);
LOCAL unsigned char dosDirChkSum(unsigned char *alias);
LOCAL STATUS dosDirNameCmp(DOS_DIR_PDESC *pDirDesc,
			   unsigned char *pName,
			   unsigned char *pNameDisk,
			   BOOL caseSens);
LOCAL void dosDirFillFd(DOS_FILE_DESC *pFd,
			unsigned char *pDirEnt,
			DIRENT_PTR *pLname);

/* Globals */
int dosDirSemOptions = SEM_Q_PRIORITY;

/* Functions */

/*******************************************************************************
 * dosDirLibInit - Initialize dos directory library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosDirLibInit(void)
{
  DOS_HDLR_DESC hdlr;

  /* Setup struct */
  hdlr.id = DOS_VDIR_HDLR_ID;
  hdlr.mountFunc = (FUNCPTR) dosDirVolMount;
  hdlr.arg = (ARG) 0;

  return dosFsHdlrInstall(dosDirHdlrsList, &hdlr);
}

/*******************************************************************************
 * dosDirVolMount - Mount function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirVolMount(DOS_VOL_DESC_ID volId, ARG arg)
{
  DOS_DIR_PDESC *pDirDesc;
  DIRENT_DESC *pDeDesc;
  cookie_t cookie;
  char bootSec[DOS_BOOT_BUF_SIZE];

  /* Initialize locals */
  pDirDesc = NULL;
  pDeDesc = NULL;
  cookie = 0;

  assert( (volId != NULL) &&
	  (volId->magic == DOS_FS_MAGIC) );

  /* Read boot sector */
  if (cbioBytesRW(volId->pCbio, volId->bootSecNum, 0, bootSec,
		  min(sizeof(bootSec), volId->bytesPerSec),
		  CBIO_READ, &cookie) == ERROR)
    return ERROR;

  /* Unmount previous handler */
  if ( (volId->pDirDesc == NULL) ||
       (volId->pDirDesc->volUnmount != (VOIDFUNCPTR) dosDirVolUnmount) ) {

    if ( (volId->pDirDesc != NULL) &&
	 (volId->pDirDesc->volUnmount != NULL) )
      ( *volId->pDirDesc->volUnmount) (volId);

    /* Allocate struct */
    volId->pDirDesc = (DOS_DIR_DESC *) realloc(volId->pDirDesc,
					       sizeof(DOS_DIR_PDESC));
    if (volId->pDirDesc == NULL)
      return ERROR;

    /* Clear struct */
    memset(volId->pDirDesc, 0, sizeof(DOS_DIR_PDESC));

  }

  /* Setup locals */
  pDirDesc = (DOS_DIR_PDESC *) volId->pDirDesc;
  pDeDesc = &pDirDesc->deDesc;

  /* Initialize dirent type */
  pDirDesc->nameStyle = STDDOS;

  /* Initialize filename related */
  pDeDesc->dirEntSize = DOS_DIRENT_STD_LEN;
  pDeDesc->nameLen = DOS_STDNAME_LEN;
  pDeDesc->extLen = DOS_STDEXT_LEN;
  pDeDesc->attribOff = DOS_ATTRIB_OFF;

  /* Initialize time and date related */
  pDeDesc->creatTimeOff = DOS_CREAT_TIME_OFF;
  pDeDesc->creatDateOff = DOS_CREAT_DATE_OFF;
  pDeDesc->accessTimeOff = DOS_LAST_ACCESS_TIME_OFF;
  pDeDesc->accessDateOff = DOS_LAST_ACCESS_DATE_OFF;
  pDeDesc->modifTimeOff = DOS_MODIF_TIME_OFF;
  pDeDesc->modifDateOff = DOS_MODIF_DATE_OFF;

  /* Initialize fat related */
  pDeDesc->startClustOff = DOS_START_CLUST_OFF;
  pDeDesc->extStartClustOff = DOS_EXT_START_CLUST_OFF;
  pDeDesc->sizeOff = DOS_FILE_SIZE_OFF;
  pDeDesc->extSizeOff = DOS_EXT_FILE_SIZE_OFF;
  pDeDesc->extSizeLen = DOS_EXT_FILE_SIZE_LEN;

  /* Verify dir entry size */
  if (pDeDesc->dirEntSize > (volId->secPerClust << volId->secSizeShift) )
    return ERROR;

  /* If fat32 */
  if (volId->fatType == FAT32) {

    pDirDesc->rootStartClust =
	DISK_TO_32BIT(&bootSec[DOS32_BOOT_ROOT_START_CLUST]);
    if (pDirDesc->rootStartClust < 2)
      return ERROR;

    pDirDesc->dirDesc.rootStartSec = 0;
    pDirDesc->dirDesc.rootNSec = 0;
    pDirDesc->rootMaxEntries = (unsigned int) (-1);

  }

  /* Else fat12 or 16 */
  else {

    pDirDesc->rootMaxEntries =
	DISK_TO_16BIT(&bootSec[DOS_BOOT_MAX_ROOT_ENTS]);
    if (pDirDesc->rootMaxEntries == 0)
      return ERROR;

    pDirDesc->rootStartClust = 0;
    pDirDesc->dirDesc.rootNSec =
	(((pDirDesc->rootMaxEntries * pDeDesc->dirEntSize) +
	  (volId->bytesPerSec - 1)) / volId->bytesPerSec);
    pDirDesc->dirDesc.rootStartSec = volId->dataStartSec;
    volId->dataStartSec += pDirDesc->dirDesc.rootNSec;

  }

  /* Set root direction modification time */
  pDirDesc->rootModifTime = time(NULL);

  /* Setup functions */
  pDirDesc->dirDesc.pathLkup = (FUNCPTR) dosDirPathLkup;
  pDirDesc->dirDesc.readDir = (FUNCPTR) dosDirReaddir;
  pDirDesc->dirDesc.updateEntry = (FUNCPTR) dosDirUpdateEntry;
  pDirDesc->dirDesc.dateGet = (FUNCPTR) dosDirDateGet;
  pDirDesc->dirDesc.volLabel = (FUNCPTR) dosDirVolLabel;
  pDirDesc->dirDesc.nameChk = (FUNCPTR) dosDirNameChk;
  pDirDesc->dirDesc.volUnmount = (VOIDFUNCPTR) dosDirVolUnmount;
  pDirDesc->dirDesc.show = (FUNCPTR) dosDirShow;

  /* If need to allocate name buffer */
  if (pDirDesc->nameBuf == NULL) {

    pDirDesc->nameBuf = (unsigned char *) malloc(2 * MAX_FULL_DIRENT);
    if (pDirDesc->nameBuf == NULL)
      return ERROR;

  }

  /* Else clear name buffer */
  else {

    memset(pDirDesc->nameBuf, 0, 2 * MAX_FULL_DIRENT);

  }

  /* Create semaphore */
  pDirDesc->bufSem = semMCreate(dosDirSemOptions);
  if (pDirDesc->bufSem == NULL)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * dosDirVolUnmount - Mount function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL void dosDirVolUnmount(DOS_VOL_DESC_ID volId)
{
}

/*******************************************************************************
 * dosDirCharEncode - Encode character
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirCharEncode(unsigned char *src,
			      unsigned char *dest,
			      const unsigned char *codeTable)
{
  if (*src & 0x80)
    *dest = *src;
  else if (codeTable[*src] != '|')
    *dest = codeTable[*src];
  else
    return ERROR;

  return OK;
}

/*******************************************************************************
 * dosDirNameEncodeShort - Encode short name
 *
 * RETURNS: Encode version
 ******************************************************************************/

LOCAL SHORT_ENCODE dosDirNameEncodeShort(DOS_DIR_PDESC *pDirDesc,
					 PATH_ARRAY *nameArray,
					 unsigned char *pDestName)
{
  unsigned char *src, *dest;
  unsigned char extLen;
  int i, j;
  SHORT_ENCODE encode;

  /* Inititalize locals */
  dest = pDestName;
  extLen = pDirDesc->deDesc.extLen;
  encode = STRICT_SHORT;
  memset(dest, ' ', pDirDesc->deDesc.nameLen + pDirDesc->deDesc.extLen);

  /* For all characters in name */
  for (i = 0, src = nameArray->pName;
       i < min(pDirDesc->deDesc.nameLen, nameArray->nameLen);
       i++, pDestName++, src++) {

    if ( (extLen != 0) &&
	 (*src == '.') )
      break;

    if (dosDirCharEncode(src, pDestName, shortNameChars) == ERROR)
      goto shortEncodeError;

    if (*pDestName != *src)
      encode = NOT_STRICT_SHORT;

  } /* End for all characters in name */

  /* If name complete */
  if (i == nameArray->nameLen)
    goto shortEncodeRet;

  /* If name to long */
  if (*src != '.')
    goto shortEncodeError;

  /* Advance */
  src++;
  pDestName += pDirDesc->deDesc.nameLen - i;
  i++;

  /* For all characters in extension */
  for (j = 0;
       (j < extLen) && (i < nameArray->nameLen);
       i++, j++, pDestName++, src++) {

    if (dosDirCharEncode(src, pDestName, shortNameChars) == ERROR)
      goto shortEncodeError;

    if (*pDestName != *src)
      encode = NOT_STRICT_SHORT;

  } /* End for all characters in extension */

  /* If to long */
  if (i < nameArray->nameLen)
    goto shortEncodeError;

shortEncodeRet:

  /* Check for empty filenames */
  i = nameArray->nameLen - 1;
  while (i > 1) {

    if (nameArray->pName[i] != ' ')
      break;

    i--;

  }

  if (i != 1) {

    while (i > 0) {

      if (nameArray->pName[i] == ' ')
        goto shortEncodeError;

      i--;

    }

  }

  return encode;

shortEncodeError:

  /* Clear */
  memset(dest, 0, DOS_DIRENT_STD_LEN);

  return NO_SHORT;
}

/*******************************************************************************
 * dosDirNameEncode - Encode name
 *
 * RETURNS: Directory entries or ERROR
 ******************************************************************************/

LOCAL int dosDirNameEncode(DOS_DIR_PDESC *pDirDesc,
			   PATH_ARRAY *nameArray,
			   unsigned char *pDestName,
			   SHORT_ENCODE *pEncode)
{
  unsigned char *src, *dest;
  unsigned char numEnt, entNum, chNum;

  /* Inititalize locals */
  src = nameArray->pName;

  /* If name to long */
  if (nameArray->nameLen > DOS_VFAT_NAME_LEN)
    goto encodeError;

  /* Get number of entries */
  numEnt = (nameArray->nameLen + CHAR_PER_ENTRY - 1) / CHAR_PER_ENTRY;

  /* Clear */
  memset(pDestName, 0, DOS_DIRENT_STD_LEN);

  /* Try to encode shortname */
  if (dosDirNameEncodeShort(pDirDesc, nameArray, pDestName) == STRICT_SHORT) {

    if (*pEncode == STRICT_SHORT)
      return 1;

    *pEncode = STRICT_SHORT;

  }

  else {

    *pEncode = NOT_STRICT_SHORT;

  }

  memcpy(&pDestName[numEnt * DOS_DIRENT_STD_LEN],
	 pDestName,
	 DOS_DIRENT_STD_LEN);
  memset(pDestName, 0, numEnt * DOS_DIRENT_STD_LEN);

  dest = &pDestName[(numEnt - 1) * DOS_DIRENT_STD_LEN];

  /* For all entries */
  for (entNum = 1;
       entNum <= numEnt;
       entNum++, dest -= DOS_DIRENT_STD_LEN) {

    *dest = entNum;
    *(dest + pDirDesc->deDesc.attribOff) = DOS_ATTR_VFAT;

    /* For all characters */
    for (chNum = 0;
         (chNum < CHAR_PER_ENTRY) &&
	 (src < nameArray->pName + nameArray->nameLen);
         src++, chNum++) {

      if (dosDirCharEncode(src,
			   dest + charOffsets[chNum],
			   longNameChars) == ERROR)
        goto encodeError;

    } /* End for all characters */

    /* For all extra characters */
    for (chNum++; chNum < CHAR_PER_ENTRY; chNum++) {

      assert(src >= nameArray->pName + nameArray->nameLen);

      *(dest + charOffsets[chNum]) = 0xff;
      *(dest + charOffsets[chNum] + 1) = 0xff;

    } /* End for all extra characters */

  } /* End for all entries */

  /* Mark last entry */
  dest += DOS_DIRENT_STD_LEN;
  *dest |= DOS_LAST_ENTRY;

  return numEnt + 1;

encodeError:

  return ERROR;
}

/*******************************************************************************
 * dosDirNameDecodeShort - Decode short name
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosDirNameDecodeShort(DOS_DIR_PDESC *pDirDesc,
				 unsigned char *pDirEnt,
				 unsigned char *pDestName)
{
  unsigned char *src, *pc;
  unsigned char *pNameEnd;
  int i, j, nameLen;

  /* Inititalize locals */
  pNameEnd = pDestName;
  src = pDirEnt;
  pc = NULL;

  for (i = 0, pc = NULL, nameLen = pDirDesc->deDesc.nameLen;
       i < 2;
       i++, nameLen = pDirDesc->deDesc.extLen) {

    for (j = 0; j < nameLen; j++, pDestName++) {

      *pDestName = *src++;

      if (*pDestName != ' ')
        pNameEnd = pDestName + 1;

    }

    /* If zero extension length */
    if (pDirDesc->deDesc.extLen == 0)
      break;

    /* Separate name and extension */
    if (pc == NULL) {

      pc = pNameEnd;
      *pNameEnd++ = '.';
      pDestName = pNameEnd;

    }

    else if (pc + 1 == pNameEnd) {

      pNameEnd--;
      break;

    }

  }

  *pNameEnd = EOS;

}

/*******************************************************************************
 * dosDirNameDecode - Decode name
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosDirNameDecode(DOS_DIR_PDESC *pDirDesc,
			    unsigned char *pDirEnt,
			    unsigned char *pDestName)
{
  unsigned char *src, *dest, *pAlias;
  int i;

  /* Inititalize locals */
  src = pDirEnt;
  dest = pDestName;

  if (src[pDirDesc->deDesc.attribOff] != DOS_ATTR_VFAT) {

    dosDirNameDecodeShort(pDirDesc, src, pDestName);
    return;

  }

  src += (*src & DOS_VFAT_ENTRY_MASK) * DOS_DIRENT_STD_LEN;
  pAlias = src;


  /* Do while not last entry */
  do {

    src -= DOS_DIRENT_STD_LEN;

    for (i = 0;
	(i < CHAR_PER_ENTRY) && (src[charOffsets[i]] != EOS);
	i++, dest++) {

      if (dest - pDestName == PATH_MAX) {

        dosDirNameDecodeShort(pDirDesc, pAlias, pDestName);
        return;

      }

      *dest = src[charOffsets[i]];

    }

  } while ((*src & DOS_LAST_ENTRY) == 0);

  *dest = EOS;
}

/*******************************************************************************
 * dosDirDateEncode - Encode time/date
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosDirDateEncode(DOS_DIR_PDESC *pDirDesc,
			    unsigned char *pDirEnt,
			    unsigned int mask,
			    time_t currTime)
{
  unsigned char timeBuf[2], dateBuf[2];
  struct tm tm;

  /* Convert time/date */
  localtime_r(&currTime, &tm);

  /* Encode time */
  TO_DISK_16BIT( (tm.tm_sec >> 1) |
		 (tm.tm_min << 5) |
		 (tm.tm_hour << 11),
		 timeBuf);

  /* Check year range */
  if (tm.tm_year < 80)
    tm.tm_year = 80;

  /* Encode date */
  TO_DISK_16BIT(tm.tm_mday |
		((tm.tm_mon + 1) << 5) |
		((tm.tm_year - 80) << 9),
		dateBuf);

  /* If creation mask */
  if ( (mask & DH_TIME_CREAT) &&
       (pDirDesc->deDesc.creatDateOff != (unsigned char) NONE) ) {

    memcpy(pDirEnt + pDirDesc->deDesc.creatTimeOff, timeBuf, 2);
    memcpy(pDirEnt + pDirDesc->deDesc.creatDateOff, dateBuf, 2);

  }

  /* If modify mask */
  if ( (mask & DH_TIME_MODIFY) &&
       (pDirDesc->deDesc.modifDateOff != (unsigned char ) NONE) ) {

    memcpy(pDirEnt + pDirDesc->deDesc.modifTimeOff, timeBuf, 2);
    memcpy(pDirEnt + pDirDesc->deDesc.modifDateOff, dateBuf, 2);

  }

  /* If access mask */
  if ( (mask & DH_TIME_ACCESS) &&
       (pDirDesc->deDesc.accessDateOff != (unsigned char ) NONE) ) {

    memcpy(pDirEnt + pDirDesc->deDesc.accessTimeOff, timeBuf, 2);
    memcpy(pDirEnt + pDirDesc->deDesc.accessDateOff, dateBuf, 2);

  }

}

/*******************************************************************************
 * dosDirDateDecode - Decode time/date
 *
 * RETURNS: Time
 ******************************************************************************/

LOCAL time_t dosDirDateDecode(DOS_DIR_PDESC *pDirDesc,
			      unsigned char *pDirEnt,
			      unsigned int which)
{
  struct tm tm;
  u_int16_t val;
  unsigned char timeOff, dateOff;

  /* Inititalize locals */
  memset(&tm, 0, sizeof(struct tm));

  /* Select which date */
  switch (which) {

    case DH_TIME_CREAT:
      timeOff = pDirDesc->deDesc.creatTimeOff;
      dateOff = pDirDesc->deDesc.creatDateOff;
    break;

    case DH_TIME_MODIFY:
      timeOff = pDirDesc->deDesc.modifTimeOff;
      dateOff = pDirDesc->deDesc.modifDateOff;
    break;

    case DH_TIME_ACCESS:
      timeOff = pDirDesc->deDesc.accessTimeOff;
      dateOff = pDirDesc->deDesc.accessDateOff;
    break;

    default:
      assert(FALSE);
  }

  /* If date decode */
  if (dateOff != (unsigned char) NONE) {

    val = DISK_TO_16BIT(&pDirEnt[dateOff]);
    tm.tm_mday = val & 0x1f;
    tm.tm_mon = ((val >> 5) & 0x0f) - 1;
    tm.tm_year = ((val >> 9) &0x7f) + 1980 - 1900;

  }

  /* If time decode */
  if (timeOff != (unsigned char) NONE) {

    val = DISK_TO_16BIT(&pDirEnt[timeOff]);
    tm.tm_sec = (val & 0x1f) << 1;
    tm.tm_min = (val >> 5) & 0x3f;
    tm.tm_hour = (val >> 11) & 0x1f;

  }

  return mktime(&tm);
}

/*******************************************************************************
 * dosDirDateGet - Get time/date
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirDateGet(DOS_FILE_DESC *pFd,
			   struct stat *pStat)
{
  DOS_DIR_PDESC *pDirDesc;
  unsigned char pDirEnt[DOS_DIRENT_STD_LEN];

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;

  /* If root dir */
  if (IS_ROOT(pFd)) {

    pStat->st_mtime = pDirDesc->rootModifTime;
    return OK;

  }

  /* Get directory entry */
  if (dosDirDirentGet(pFd, pDirEnt, RD_ENTRY) == ERROR)
    return ERROR;

  pStat->st_ctime = dosDirDateDecode(pDirDesc, pDirEnt, DH_TIME_CREAT);
  pStat->st_mtime = dosDirDateDecode(pDirDesc, pDirEnt, DH_TIME_MODIFY);
  pStat->st_atime = dosDirDateDecode(pDirDesc, pDirEnt, DH_TIME_ACCESS);
}

/*******************************************************************************
 * dosDirVolLabel - Set/get volume label
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirVolLabel(DOS_VOL_DESC_ID volId,
			    unsigned char *label,
			    unsigned int req)
{
  static char *noname = "NO LABEL";

  DOS_DIR_PDESC *pDirDesc;
  DOS_FILE_DESC fDesc;
  DOS_FILE_HDL fHdl;
  unsigned char pDirEnt[DOS_DIRENT_STD_LEN];
  unsigned short labelOff;
  unsigned int numEnt;
  STATUS status;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) volId->pDirDesc;
  status = ERROR;

  assert( (req == FIOLABELSET) ||
	  (req == FIOLABELGET) );

  /* Get label offset */
  if (volId->fatType == FAT32)
    labelOff = DOS32_BOOT_VOL_LABEL;
  else
    labelOff = DOS_BOOT_VOL_LABEL;

  /* If no label given */
  if (label == NULL) {

    if (req == FIOLABELSET)
      label = (unsigned char *) noname;
    else
      return ERROR;
  }

  /* Initialize file descriptor */
  fDesc.pVolDesc = volId;
  fDesc.pFileHdl = &fHdl;
  dosDirFillFd(&fDesc, ROOT_DIRENT, NULL);

  /* For all dir entries */
  for (numEnt = 0, status = dosDirDirentGet(&fDesc, pDirEnt, RD_FIRST);
       (status != ERROR) && (*pDirEnt != EOS);
       numEnt++, status = dosDirDirentGet(&fDesc, pDirEnt, RD_NEXT) ) {

    /* If label */
    if ( (pDirEnt[pDirDesc->deDesc.attribOff] != DOS_ATTR_VFAT) &&
	 ((pDirEnt[pDirDesc->deDesc.attribOff] & DOS_ATTR_VOL_LABEL) != 0) )
      break;

  } /* End for all dir entries */

  /* If get label */
  if (req == FIOLABELGET) {

    /* Label not found, try to get from boot sector */
    if ( (status == ERROR) ||
	 (*pDirEnt == EOS) )
      memcpy(pDirEnt, volId->bootVolLab, DOS_VOL_LABEL_LEN);

    /* Copy label */
    memcpy(label, pDirEnt, DOS_VOL_LABEL_LEN);
    numEnt = DOS_VOL_LABEL_LEN;
    while ( (numEnt > 0) &&
	    (label[numEnt - 1] == ' ') )
      label[--numEnt] = EOS;

    return OK;

  } /* End if get label */

  if (status == ERROR) {

    if (numEnt >= pDirDesc->rootMaxEntries)
      return ERROR;

  }

  /* Set label */
  memset(pDirEnt, 0, sizeof(pDirEnt));
  memset(pDirEnt, ' ', pDirDesc->deDesc.nameLen + pDirDesc->deDesc.extLen);
  memcpy(pDirEnt, label, min(DOS_VOL_LABEL_LEN, strlen((char *) label)));
  pDirEnt[pDirDesc->deDesc.attribOff] = DOS_ATTR_VOL_LABEL;

  status = dosDirDeStore(&fDesc, 0, DOS_DIRENT_STD_LEN, pDirEnt,
			 PUT_CURRENT | DH_ALLOC);

  /* Change label in boot sector */
  memcpy(volId->bootVolLab, pDirEnt, DOS_VOL_LABEL_LEN);
  status |= cbioBytesRW(volId->pCbio,
			DOS_BOOT_SEC_NUM,
			labelOff,
			pDirEnt,
			DOS_VOL_LABEL_LEN,
			CBIO_WRITE,
			NULL);
  return status;
}

/*******************************************************************************
 * dosDirNameChk - Check filename
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirNameChk(DOS_VOL_DESC_ID volId,
			   unsigned char *name)
{
  DOS_DIR_PDESC *pDirDesc;
  SHORT_ENCODE shortNameEncode;
  PATH_ARRAY nameArray;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) volId->pDirDesc;
  shortNameEncode = NOT_STRICT_SHORT;

  nameArray.pName = name;
  nameArray.nameLen = strlen((char *) name);

  semTake(pDirDesc->bufSem, WAIT_FOREVER);

  if (dosDirNameEncode(pDirDesc, &nameArray,
		       pDirDesc->nameBuf, &shortNameEncode) == ERROR) {

    semGive(pDirDesc->bufSem);
    return ERROR;

  }

  semGive(pDirDesc->bufSem);

  return OK;
}

/*******************************************************************************
 * dosDirShow - Dos dir show function
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirShow(DOS_VOL_DESC_ID volId)
{
  DOS_DIR_PDESC *pDirDesc;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) volId->pDirDesc;

  printf("Directory handler information:\n"
	 "------------------------------\n");
  printf(" - directory structure:                    VFAT\n");

  if (!volId->mounted)
    return ERROR;

  /* If fat12 or fat 16 */
  if (pDirDesc->dirDesc.rootStartSec != 0) {

    printf(" - root dir start sector:                  %u\n",
	   pDirDesc->dirDesc.rootStartSec);
    printf(" - # of sectors per root:                  %u\n",
	    pDirDesc->dirDesc.rootNSec);
    printf(" - max # of entries in root:               %u\n",
	   pDirDesc->rootMaxEntries);
  }

  /* Else fat32 */
  else {

    printf(" - root dir start cluster:                 %u\n",
	   pDirDesc->rootStartClust);

  }

  return OK;
}

/*******************************************************************************
 * dosDirPathParse - Parse path name
 *
 * RETURNS: Number of levels in path
 ******************************************************************************/

LOCAL int dosDirPathParse(DOS_VOL_DESC_ID volId,
			  unsigned char *path,
			  PATH_ARRAY *nameArray)
{
  unsigned int numLevels;
  unsigned char *pName, *pChar;

  /* Inititlaize locals */
  pName = path;
  numLevels = 0;

  /* Terminate name array */
  nameArray[0].pName = NULL;

  /* While path has more chars */
  while (*pName != EOS) {

    /* If slash or backslash */
    if ( (*pName == '/') ||
	 (*pName == '\\') ) {

      pName++;
      continue;

    } /* End if slash of backslash */

    /* If dot */
    if (*pName == '.') {

      /* If dot-slash */
      if ( (pName[1] == EOS) ||
	   (pName[1] == '/') ||
           (pName[1] == '\\') ) {

        pName++;
        continue;

      } /* End if dot-slash */

      /* If dot-dot-slash */
      if ( (pName[1] == '.') &&
	   ((pName[2] == EOS) || (pName[2] == '/') || (pName[2] == '\\')) ) {

        if (numLevels > 0)
          numLevels--;

        pName += 2;
        continue;

      } /* End if dot-dot-slash */

    } /* End if dot */

    /* If past max dir levels */
    if (numLevels >= DOS_MAX_DIR_LEVELS)
      break;

    /* Store path pice */
    nameArray[numLevels].pName = pName;
    nameArray[numLevels + 1].pName = NULL;
    pChar = NULL;

    /* While not slash */
    while ( (*pName != '/') &&
	    (*pName != '\\') &&
	    (*pName != EOS) ) {

      if ( (*pName != '.') ||
	   (*pName != ' ') )
        pChar = pName;

      pName++;

    } /* End while not slash */

    /* If only dots found */
    if (pChar == NULL)
      return ERROR;

    /* Store name length */
    nameArray[numLevels].nameLen = pChar + 1 - nameArray[numLevels].pName;

    /* Advance */
    numLevels++;

  } /* End while path has more chars */

  /* If anything left */
  if (*pName != EOS)
    return ERROR;

  return numLevels;
}

/*******************************************************************************
 * dosDirFullEntGet - Get directory entry from disk
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirFullEntGet(DOS_FILE_DESC *pFd,
			      unsigned char *pEntry,
			      RDE_OPTION which,
			      DIRENT_PTR *pLname,
			      unsigned int *nEntries)
{
  DOS_DIR_PDESC *pDirDesc;
  unsigned char *pDest;
  unsigned char attribOff;
  int entNum, numEnt, chkSum;
  STATUS status;

  assert(nEntries != NULL);

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;
  pDest = pEntry;
  attribOff = pDirDesc->deDesc.attribOff;
  entNum = NONE;
  numEnt = 0;
  chkSum = 0;
  status = ERROR;

  /* Reset long name */
  pLname->sector = 0;
  pLname->offset = 0;

  /* For all entries */
  for (status = dosDirDirentGet(pFd, pDest, which);
       status != ERROR;
       status = dosDirDirentGet(pFd, pDest, RD_NEXT)) {

    /* If not last dirent */
    if (*pDest != EOS)
      (*nEntries)++;

    /* If short entry */
    if ( (entNum == NONE) &&
	 (pDest[attribOff] != DOS_ATTR_VFAT) ||
	 (*pDest == DOS_DEL_MARK) ||
	 (*pDest == EOS) ||
	 ( ((pDest[attribOff] & DOS_ATTR_VOL_LABEL) != 0) &&
	   (pDest[attribOff] != DOS_ATTR_VFAT) ) ) {

      memcpy(pEntry, pDest, DOS_DIRENT_STD_LEN);
      goto fullEntShortRet;

    } /* End if short entry */

    /* Long entry */

    /* If vfat attribute */
    if (pDest[attribOff] == DOS_ATTR_VFAT) {

      /* If not last entry */
      if ((*pDest & DOS_LAST_ENTRY) != 0) {

        /* If longname already started */
        if (entNum != NONE) {

          logMsg("Bad long name structure (breached)\n",
		 (ARG) 1,
		 (ARG) 2,
		 (ARG) 3,
		 (ARG) 4,
		 (ARG) 5,
		 (ARG) 6);

	  assert(numEnt != 0);

          /* Remove */
          dosDirEntryDel(pFd->pVolDesc, pLname, numEnt - entNum, FALSE);

          memcpy(pEntry, pDest, DOS_DIRENT_STD_LEN);
          pDest = pEntry;

        } /* End if longname already started */

        numEnt = *pDest & DOS_VFAT_ENTRY_MASK;
	entNum = numEnt;
        if (entNum > MAX_DIRENT) {

          logMsg("Bad long name structure (to long)\n",
		 (ARG) 1,
		 (ARG) 2,
		 (ARG) 3,
		 (ARG) 4,
		 (ARG) 5,
		 (ARG) 6);

	  goto fullEntError;

        }

        chkSum = pDest[DOS_VFAT_CHKSUM_OFF];
        pLname->sector = pFd->curSec;
	pLname->offset = pFd->pos;

      } /* End if not last entry */

      /* Else if no entry number */
      else if (entNum == NONE) {

        logMsg("Long name internal entry without first one\n",
	       (ARG) 1,
	       (ARG) 2,
	       (ARG) 3,
	       (ARG) 4,
	       (ARG) 5,
	       (ARG) 6);

        goto fullEntError;

      } /* End else if no entry number */

      /* Else if malformed longname */
      else if (entNum != (*pDest & DOS_VFAT_ENTRY_MASK)) {

        logMsg("Malformed long name structure\n",
	       (ARG) 1,
	       (ARG) 2,
	       (ARG) 3,
	       (ARG) 4,
	       (ARG) 5,
	       (ARG) 6);

        goto fullEntError;

      } /* End else if malformed longname */

      /* Else one more entry in longname */
      else {

        /* If invalid checksum */
        if (chkSum != pDest[DOS_VFAT_CHKSUM_OFF]) {

          logMsg("Malformed long name structure\n",
	         (ARG) 1,
	         (ARG) 2,
	         (ARG) 3,
	         (ARG) 4,
	         (ARG) 5,
	         (ARG) 6);

	  goto fullEntError;

        } /* Else if ivnalid checksum */

      } /* End else one more entry in longname */

      /* Expected next entry for longname */
      entNum--;
      pDest += DOS_DIRENT_STD_LEN;
      continue;

    } /* If vfat attribute */

    assert(entNum != NONE);

    /* If invalid alias checksum */
    if ( (entNum != 0) &&
	 (dosDirChkSum(pDest) != chkSum) ) {

      logMsg("Malformed long name structure\n",
	     (ARG) 1,
	     (ARG) 2,
	     (ARG) 3,
	     (ARG) 4,
	     (ARG) 5,
	     (ARG) 6);

      goto fullEntShortRet;

    } /* End if invalid alias checksum */

    /* Got out of loop */
    goto fullEntRet;

fullEntError:

    /* Remove invalid entry */
    dosDirEntryDel(pFd->pVolDesc, pLname, numEnt - entNum, FALSE);

    entNum = NONE;
    pDest = pEntry;

    pLname->sector = 0;
    pLname->offset = 0;

  } /* End for all entries */

fullEntRet:

  return status;

fullEntShortRet:

  if (entNum != NONE) {

    logMsg("Erroneous long name\n",
	   (ARG) 1,
	   (ARG) 2,
	   (ARG) 3,
	   (ARG) 4,
	   (ARG) 5,
	   (ARG) 6);

    assert(numEnt != 0);

    /* Remove invalid entry */
    dosDirEntryDel(pFd->pVolDesc, pLname, numEnt - entNum, FALSE);

    memcpy(pEntry, pDest, DOS_DIRENT_STD_LEN);

  }

  pLname->sector = 0;
  pLname->offset = 0;

  return OK;
}

/*******************************************************************************
 * dosDirLkupInDir - Lookup directory for name given
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirLkupInDir(DOS_FILE_DESC *pFd,
			     PATH_ARRAY *nameArray,
			     DIRENT_PTR *pFreeEnt,
			     int *pFreeEntLen,
			     BOOL caseSens)
{
  DOS_DIR_PDESC *pDirDesc;
  DIRENT_PTR pLname;
  SHORT_ENCODE shortNameEncode;
  unsigned char *pName, *pNameAlias, *pNameDisk;
  int aliasOff, nEntInName;
  unsigned int nEntries, which;
  size_t freeLen;
  STATUS status;

  /* Initialize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;
  memset(&pLname, 0, sizeof(DIRENT_PTR));
  pName = pDirDesc->nameBuf;
  pNameDisk = pDirDesc->nameBuf + MAX_FULL_DIRENT;
  aliasOff = 0;
  freeLen = 0;
  status = ERROR;

  /* Inititalize arguments */
  *pFreeEntLen = 0;
  pFreeEnt->deNum = 0;
  pFreeEnt->sector = 0;
  pFreeEnt->offset = 0;

  semTake(pDirDesc->bufSem, WAIT_FOREVER);

  /* Encode name */
  shortNameEncode = NOT_STRICT_SHORT;
  nEntInName = dosDirNameEncode(pDirDesc,
				nameArray,
				pName,
				&shortNameEncode);
  if (nEntInName == ERROR)
    goto lkupInDirRet;


  aliasOff = DOS_DIRENT_STD_LEN * (nEntInName - 1);
  pNameAlias = pName + aliasOff;

  if (shortNameEncode == STRICT_SHORT)
    nEntInName = 1;
  else if (caseSens)
    *pNameAlias = EOS;

  nEntries = 0;

#ifdef DEBUG_DIR
  char str[255];
  dosDirNameDecode(pDirDesc, pName, str);
  printf("Filename to check: %s\n", str);
#endif

  /* For all dir entries */
  for (which = RD_FIRST; 1; which = RD_NEXT) {

    status = dosDirFullEntGet(pFd,
			      pNameDisk,
			      which,
			      &pLname,
			      &nEntries);
    if ( (status == ERROR) ||
	 (*pNameDisk == EOS) )
      break;

#ifdef DEBUG_DIR
    dosDirNameDecode(pDirDesc, pNameDisk, str);
    printf("File name on disk: %s\n", str);
#endif

    /* If deleted entry */
    if (*pNameDisk == DOS_DEL_MARK) {

      if (freeLen == 0) {

        pFreeEnt->deNum = nEntries;
	pFreeEnt->sector = pFd->curSec;
	pFreeEnt->offset = pFd->pos;

      }

      freeLen++;
      continue;

    } /* End if deleted entry */

    if (freeLen < (size_t) nEntInName)
      freeLen = 0;

    /* If longname entry */
    if (pNameDisk[pDirDesc->deDesc.attribOff] == DOS_ATTR_VFAT) {

      if (dosDirNameCmp(pDirDesc, pName, pNameDisk, caseSens) == OK) {

#ifdef DEBUG_DIR
        printf("Long name match.\n");
#endif

        goto lkupInDirRet;

      }

      continue;

    } /* End if longname entry */

    /* If volume label */
    if ((pNameDisk[pDirDesc->deDesc.attribOff] & DOS_ATTR_VOL_LABEL) != 0)
      continue;

    /* If shortname match */
    if (memcmp(pNameAlias, pNameDisk, DOS_STDNAME_LEN + DOS_STDEXT_LEN) == 0) {

#ifdef DEBUG_DIR
      printf("Short name match.\n");
#endif

      aliasOff = 0;
      goto lkupInDirRet;

    } /* End if shortname match */

  } /* End for all dir entries */

  /* If space not found */
  if (freeLen < (size_t) nEntInName) {

    if (status == ERROR)
      pFreeEnt->deNum = nEntries + 1;
    else
      pFreeEnt->deNum = nEntries;

    pFreeEnt->sector = 0;
    pFreeEnt->offset = 0;

  } /* End if space not found */

  if (status == ERROR)
    goto lkupInDirRet;

  status = ERROR;

  /* If root dir full */
  if (IS_ROOT(pFd) &&
      (nEntries + nEntInName > pDirDesc->rootMaxEntries) )
    goto lkupInDirRet;

  /* If possible to create new entry */
  if (pFreeEnt->sector == 0) {

    pFreeEnt->deNum = nEntries;
    pFreeEnt->sector = pFd->curSec;
    pFreeEnt->offset = pFd->pos;

  } /* End if possible to create new entry */

lkupInDirRet:

  /* If success */
  if (status != ERROR)
    dosDirFillFd(pFd, pNameDisk + aliasOff, &pLname);

  /* Set length */
  *pFreeEntLen = freeLen;

  semGive(pDirDesc->bufSem);

  return status;
}

/*******************************************************************************
 * dosDirAliasCreate - Create alias for longname
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL void dosDirAliasCreate(DOS_DIR_PDESC * pDirDesc,
			     PATH_ARRAY *nameArray,
			     unsigned char *pNameAlias,
			     unsigned int entNum)
{
  unsigned char *src, *dest;
  char buf[DOS_STDNAME_LEN + DOS_STDEXT_LEN + 1];

  /* Inititalize locals */
  src = nameArray->pName;
  dest = pNameAlias;
  memset(dest, ' ', DOS_STDNAME_LEN + DOS_STDEXT_LEN);

  /* Initialize arguments */
  entNum = 1000000 - entNum % 1000000;

  /* Move beyond dot */
  for (; *src == '.'; src++);

  assert(*src != EOS);

  /* For all characters */
  for (;
       (*src != '.') &&
       (src != nameArray->pName + nameArray->nameLen) &&
       (dest != pNameAlias + DOS_STDNAME_LEN);
       src++, dest++) {

    if (dosDirCharEncode(src, dest, shortNameChars) == ERROR)
      break;

  } /* End for all characters */

  /* Encode dirent number */
  sprintf(buf, "%c%u", '~', entNum);
  entNum = strlen(buf);

  /* Check range */
  if (dest > pNameAlias + DOS_STDNAME_LEN - entNum)
    dest = pNameAlias + DOS_STDNAME_LEN - entNum;

  /* Copy to destination */
  memcpy(dest, buf, entNum);

  /* Process extension */
  dest = pNameAlias + DOS_STDNAME_LEN;

  /* Find last dot */
  for (src = nameArray->pName + nameArray->nameLen - 1;
       (src != nameArray->pName) && (*src != '.');
       src--);

  /* If no extension */
  if (src == nameArray->pName)
    return;

  /* For all characters in externsion */
  for (src++;
       (dest != pNameAlias + DOS_STDNAME_LEN + DOS_STDEXT_LEN) &&
       (src != nameArray->pName + nameArray->nameLen);
       src++, dest++) {

    if (dosDirCharEncode(src, dest, shortNameChars) == ERROR) {

      *dest = ' ';
      break;

    }

  } /* End for all characters in extension */
}

/*******************************************************************************
 * dosDirFileCreateInDir - Create file entry in directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirFileCreateInDir(DOS_FILE_DESC *pFd,
				   PATH_ARRAY *nameArray,
				   unsigned int options,
				   DIRENT_PTR *pFreeEnt,
				   int freeEntLen)
{
  DOS_DIR_PDESC *pDirDesc;
  DOS_DIR_HDL *pDirHdl;
  SHORT_ENCODE shortNameEncode;
  unsigned char *pDirEnt;
  unsigned char *pNameAlias;
  unsigned char chkSum;
  unsigned int which, allocFlag;
  int i, numEnt;
  BOOL parentIsRoot;
  time_t currTime;
  cookie_t cookie;
  STATUS status;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;
  pDirHdl = &pFd->pFileHdl->dirHdl;
  shortNameEncode = STRICT_SHORT;
  pDirEnt = pDirDesc->nameBuf;
  parentIsRoot = FALSE;
  status = ERROR;

  /* Check permissions */
  if (pFd->pFileHdl->attrib & DOS_ATTR_RDONLY)
    return ERROR;

  /* Check file type */
  if ( !(S_ISREG(options) || S_ISDIR(options)) )
    return ERROR;

  /* If root directory */
  if (IS_ROOT(pFd))
    parentIsRoot = TRUE;

  semTake(pDirDesc->bufSem, WAIT_FOREVER);

  /* Encode filename */
  numEnt = dosDirNameEncode(pDirDesc, nameArray, pDirEnt, &shortNameEncode);
  if (numEnt == ERROR)
    goto fileCreateRet;

  /* Set alias */
  pNameAlias = pDirEnt + DOS_DIRENT_STD_LEN * (numEnt - 1);
  if (*pNameAlias == EOS) {

    assert(pNameAlias != pDirEnt);
    dosDirAliasCreate(pDirDesc, nameArray, pNameAlias,
		      pFreeEnt->deNum + numEnt);

  }

  /* Encode time/date */
  currTime = time(&currTime);
  dosDirDateEncode(pDirDesc, pNameAlias,
		   DH_TIME_CREAT | DH_TIME_MODIFY | DH_TIME_ACCESS,
		   currTime);

  /* Calculate checksum */
  chkSum = dosDirChkSum(pNameAlias);

  /* If sector zero */
  if (pFreeEnt->sector != 0) {

    /* If no space left in root */
    if (IS_ROOT(pFd) &&
	(pDirDesc->rootMaxEntries < (unsigned int) -1) ) {

      pFd->nSec += pFd->curSec - pFreeEnt->sector;
      pFd->curSec = pFreeEnt->sector;

    } /* End if no space left in root */

    /* Else if seek fails */
    else if ( ( *pFd->pVolDesc->pFatDesc->seek)
	      (pFd, pFreeEnt->sector, 0) == ERROR ) {

      goto fileCreateRet;

    } /* End else if seek fails */

    /* Advance */
    pFd->pos = pFreeEnt->offset;

  } /* End if sector zero */

  /* Else if no space left in root */
  else if (IS_ROOT(pFd) &&
	   (pDirDesc->rootMaxEntries < (unsigned int) -1) ) {

    goto fileCreateRet;

  } /* End else if no space left in root */

  /* Create a new entry */
  for (i = numEnt, which = PUT_CURRENT;
       i > 0;
       i--, pDirEnt += DOS_DIRENT_STD_LEN, which = PUT_NEXT, --freeEntLen) {

    /* Encode checksum and attribute for longname */
    if (i > 1) {

      pDirEnt[DOS_VFAT_CHKSUM_OFF] = chkSum;
      pDirEnt[pDirDesc->deDesc.attribOff] = DOS_ATTR_VFAT;

    }

    /* Determine alloc policy */
    if (freeEntLen > 0)
      allocFlag = 0;
    else
      allocFlag = DH_ALLOC;

    /* Store entry */
    if (dosDirDeStore(pFd, 0, DOS_DIRENT_STD_LEN,
		      pDirEnt, which | allocFlag) == ERROR)
      goto fileCreateRet;

  }

  if (i == numEnt)  {

    pFreeEnt->sector = pFd->curSec;
    pFreeEnt->offset = pFd->pos;

  }

  /* Update */
  currTime = time(&currTime);
  dosDirUpdateEntry(pFd, DH_TIME_MODIFY, currTime);

  cookie = pDirHdl->cookie;

  /* Fill fd */
  if (numEnt > 1)
    dosDirFillFd(pFd, pNameAlias, pFreeEnt);
  else
    dosDirFillFd(pFd, pNameAlias, NULL);

  if (S_ISREG(options)) {

    status = OK;
    goto fileCreateRet;

  }

  /* Process directory */
  pDirEnt = pNameAlias;
  pNameAlias[pDirDesc->deDesc.attribOff] = DOS_ATTR_DIRECTORY;
  pFd->pFileHdl->attrib = DOS_ATTR_DIRECTORY;

  if (dosDirClustNext(pFd, DH_ALLOC) == ERROR)
    goto fileCreateRet;

  /* Encode */
  START_CLUST_ENCODE(&pDirDesc->deDesc, pFd->pFileHdl->startClust, pNameAlias);

  /* Initialize entry for dot */
  memcpy(pDirEnt, pNameAlias, DOS_DIRENT_STD_LEN);
  memset(pDirEnt, ' ', pDirDesc->deDesc.nameLen + pDirDesc->deDesc.extLen);
  *pDirEnt = '.';
  if (dosDirDeStore(pFd, 0, DOS_DIRENT_STD_LEN, pDirEnt, PUT_CURRENT) == ERROR)
    goto fileCreateRet;

  /* Initialize entry for dot-dot */
  if (parentIsRoot) {

    START_CLUST_ENCODE(&pDirDesc->deDesc, 0, pDirEnt);

  }

  else {

    START_CLUST_ENCODE(&pDirDesc->deDesc, pDirHdl->parDirStartCluster, pDirEnt);
  }

  pDirEnt[1] = '.';
  if (dosDirDeStore(pFd, 0, DOS_DIRENT_STD_LEN, pDirEnt,
		    PUT_NEXT | DH_ALLOC) == ERROR)
    goto fileCreateRet;

  if (dosDirUpdateEntry(pFd, 0, 0) == ERROR)
    goto fileCreateRet;

  /* Rewind */
  dosDirRewind(pFd);

  status = OK;

fileCreateRet:

  semGive(pDirDesc->bufSem);

  return status;
}

/*******************************************************************************
 * dosDirPathLkup - Lookup path directory entry
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirPathLkup(DOS_FILE_DESC *pFd,
			    void *path,
			    unsigned int options)
{
  DIRENT_PTR freeEnt;
  PATH_ARRAY nameArray[DOS_MAX_DIR_LEVELS + 1];
  int numLevels, freeEntLen;
  unsigned char dirLevel;

  assert(path != NULL);

  /* Parse path */
  numLevels = dosDirPathParse(pFd->pVolDesc, path, nameArray);
  if (numLevels == ERROR)
    return ERROR;

#ifdef DEBUG_DIR
  int i;
  printf("Path levels:\n");
  for (i = 0; i < numLevels; i++)
    printf("#%d: %s\n", i, nameArray[i].pName);
#endif

  assert(numLevels <= DOS_MAX_DIR_LEVELS);

  /* Fill in file descriptor */
  dosDirFillFd(pFd, ROOT_DIRENT, NULL);

  /* For all path levels */
  for (dirLevel = 0; dirLevel < numLevels; dirLevel++) {

    /* If not a directory */
    if ((pFd->pFileHdl->attrib & DOS_ATTR_DIRECTORY) == 0)
      break;

    /* Lookup in directory */
    if (dosDirLkupInDir(pFd, nameArray + dirLevel, &freeEnt, &freeEntLen,
			((options & DOS_O_CASENS) != 0)) == ERROR)
      break;

  } /* End for all path levels */

  if (dirLevel == numLevels)
    return OK;

  if ( (dirLevel == numLevels - 1) &&
       (options & O_CREAT) ) {

#ifdef DEBUG_DIR
    printf("Creating new file...");
#endif

    if (dosDirFileCreateInDir(pFd,
			      nameArray + dirLevel,
			      options,
			      &freeEnt,
			      freeEntLen) == OK) {

#ifdef DEBUG_DIR
        printf("Success.\n");
#endif

        return OK;

    }

    else {

#ifdef DEBUG_DIR
        printf("Failed.\n");
#endif

    }

  }

  return ERROR;
}

/*******************************************************************************
 * dosDirRewind - Rewind directory
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosDirRewind(DOS_FILE_DESC *pFd)
{
  DOS_DIR_PDESC *pDirDesc;

  /* Initialize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;

  /* Rewind file descriptor */
  pFd->pos = 0;
  pFd->nSec = 0;
  pFd->curSec = 0;

  /* If root directory */
  if (IS_ROOT(pFd) &&
      (pDirDesc->dirDesc.rootNSec != 0) ) {

    pFd->curSec = pDirDesc->dirDesc.rootStartSec;
    pFd->nSec = pDirDesc->dirDesc.rootNSec;

    return;

  }

  /* Seek to current position */
  if ( ( *pFd->pVolDesc->pFatDesc->seek) (pFd, FH_FILE_START, 0) == ERROR )
    assert(FALSE);
}

/*******************************************************************************
 * dosDirDirentGet - Get directory entries
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirDirentGet(DOS_FILE_DESC *pFd,
			     unsigned char *pDirEnt,
			     RDE_OPTION which)
{
  DOS_DIR_PDESC *pDirDesc;
  DOS_DIR_HDL *pDirHdl;
  DOS_FILE_DESC fDesc;
  int dirEntSize;

  /* Initialize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;
  pDirHdl = &pFd->pFileHdl->dirHdl;
  dirEntSize = pDirDesc->deDesc.dirEntSize;

  /* If file */
  if (which == RD_ENTRY) {

    fDesc = *pFd;
    fDesc.curSec = pDirHdl->sector;
    fDesc.pos = pDirHdl->offset;
    fDesc.cbioCookie = pDirHdl->cookie;

    goto getEntry;

  }

  /* Else if current */
  if (which == RD_CURRENT) {

    assert(pFd->nSec != 0);
    assert(pFd->curSec != 0);

    goto getEntry;

  }

  /* If first dir entry */
  if (which == RD_FIRST) {

    dosDirRewind(pFd);

  }

  /* Else if next dir entry */
  else if (which == RD_NEXT) {

    assert(pFd->curSec != 0);

    pFd->pos += dirEntSize;
    if (OFFSET_IN_SEC(pFd->pVolDesc, pFd->pos) == 0) {

      pFd->curSec++;
      pFd->nSec--;

    }

  }

  /* Else invalid option */
  else {

    assert(FALSE);

  }

  /* If contiguous */
  if (pFd->nSec == 0) {

    if ( ( *pFd->pVolDesc->pFatDesc->getNext) (pFd, FAT_NOT_ALLOC) == ERROR )
      return ERROR;

    pFd->cbioCookie = (cookie_t) NULL;

  }

  /* Store current file descriptor */
  fDesc = *pFd;

getEntry:

  /* Read directory entry */
  if (cbioBytesRW(fDesc.pVolDesc->pCbio, fDesc.curSec,
      OFFSET_IN_SEC(fDesc.pVolDesc, fDesc.pos),
      (char *) pDirEnt, dirEntSize, CBIO_READ,
      &fDesc.cbioCookie) == ERROR)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * dosDirReaddir - Read dir
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirReaddir(DOS_FILE_DESC *pFd,
			   struct dirent *pDir,
			   DOS_FILE_DESC *pResFd)
{
  DOS_DIR_PDESC *pDirDesc;
  DOS_FILE_HDL *pFileHdl;
  DIRENT_PTR pLname;
  int readFlag;
  unsigned char *pDirEnt;
  unsigned int nEntries;
  STATUS status;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;
  memset(&pLname, 0, sizeof(DIRENT_PTR));
  pDirEnt = pDirDesc->nameBuf;
  nEntries = 0;
  status = ERROR;

  assert(pFd->pFileHdl->attrib & DOS_ATTR_DIRECTORY);

  if (pDir->d_ino == (ino_t) -1)
    return ERROR;

  /* Get read flag */
  if (pDir->d_ino == 0)
    readFlag = RD_FIRST;
  else
    readFlag = RD_NEXT;

  if (pFd->pos % DOS_STDNAME_LEN != 0)
    goto readDirRet;

  semTake(pDirDesc->bufSem, WAIT_FOREVER);

  do {

    if (dosDirFullEntGet(pFd, pDirEnt, readFlag,
			 &pLname, &nEntries) == ERROR)
      goto readDirRet;

    readFlag = RD_NEXT;

  } while ( (*pDirEnt == DOS_DEL_MARK) ||
	    ((pDirEnt[pDirDesc->deDesc.attribOff] != DOS_ATTR_VFAT) &&
	     ((pDirEnt[pDirDesc->deDesc.attribOff] & DOS_ATTR_VOL_LABEL) != 0))
	  );

  if (*pDirEnt == EOS)
    goto readDirRet;

  /* Decode name */
  dosDirNameDecode(pDirDesc, pDirEnt, (unsigned char *) pDir->d_name);

  /* If resulting file descriptor requested */
  if (pResFd != NULL) {

    pFileHdl = pResFd->pFileHdl;
    *pResFd = *pFd;
    pResFd->pFileHdl = pFileHdl;
    *pResFd->pFileHdl = *pFd->pFileHdl;

    if (pDirEnt[pDirDesc->deDesc.attribOff] == DOS_ATTR_VFAT)
      pDirEnt += (*pDirEnt & DOS_VFAT_ENTRY_MASK) * DOS_DIRENT_STD_LEN;

    dosDirFillFd(pResFd, pDirEnt, &pLname);

  }

  status = OK;

readDirRet:

  if (status == OK)
    pDir->d_ino = POS_TO_INO(pFd->pos);
  else
    pDir->d_ino = (ino_t) -1;

  semGive(pDirDesc->bufSem);

  return status;
}

/*******************************************************************************
 * dosDirEntryDel - Delete directory entry
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirEntryDel(DOS_VOL_DESC_ID volId,
			    DIRENT_PTR *pLname,
			    unsigned int nEnt,
			    BOOL force)
{
  DOS_DIR_PDESC *pDirDesc;
  DOS_FILE_DESC fDesc;
  DOS_FILE_HDL fHdl;
  unsigned char dirEnt[DOS_DIRENT_STD_LEN];
  cookie_t cookie;
  unsigned int which;

  if (!force && (volId->chkLevel <= DOS_CHK_ONLY) )
    return OK;

  if (pLname->sector == 0)
    return ERROR;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) volId->pDirDesc;
  cookie = (cookie_t) NULL;
  memset(&fDesc, 0, sizeof(DOS_FILE_DESC));
  memset(&fHdl, 0, sizeof(DOS_FILE_HDL));

  /* Inititalize file descriptor */
  fDesc.pVolDesc = volId;
  fDesc.pFileHdl = &fHdl;
  fDesc.curSec = pLname->sector;
  fDesc.pos = pLname->offset;

  /* Inititalize file handle */
  fHdl.startClust = NONE;

  if (fDesc.curSec < volId->dataStartSec) {

    fDesc.nSec = volId->dataStartSec - fDesc.curSec;

  }

  else if ( (*volId->pFatDesc->seek) (&fDesc, fDesc.curSec, 0) == ERROR) {

    assert(FALSE);
    return ERROR;

  }

  /* If no entries read */
  if (nEnt == 0) {

    if (cbioBytesRW(volId->pCbio, fDesc.curSec,
		    OFFSET_IN_SEC(volId, fDesc.pos),
		    (char *) dirEnt, DOS_DIRENT_STD_LEN,
		    CBIO_READ, &cookie) == ERROR)
      return ERROR;

    assert(dirEnt[pDirDesc->deDesc.attribOff] == DOS_ATTR_VFAT);
 
    /* Get number of entries */
    nEnt = (*dirEnt & DOS_VFAT_ENTRY_MASK) + 1;

  }

  /* Delete entry */
  for (*dirEnt = DOS_DEL_MARK, which = PUT_CURRENT;
       nEnt > 0;
       nEnt--, which = PUT_NEXT) {

    if (dosDirDeStore(&fDesc, 0, 1, &dirEnt, which) == ERROR)
      return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * dosDirUpdateEntry - Update directory entry
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirUpdateEntry(DOS_FILE_DESC *pFd, int flags, time_t currTime)
{
  DOS_DIR_PDESC *pDirDesc;
  DIRENT_PTR de;
  unsigned char pDirEnt[DOS_DIRENT_STD_LEN];
  unsigned int nEnt;

  /* Initialize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;

  /* Get time if zero */
  if (currTime == 0)
    currTime = time(&currTime);

  /* If root directory */
  if (IS_ROOT(pFd)) {

    if ( (flags & DH_TIME_MODIFY) != 0)
      pDirDesc->rootModifTime = currTime;
    else
      assert( (flags & DH_TIME_MODIFY) != 0 );

    return OK;

  }

  /* If delete requested */
  if (flags & DH_DELETE) {

    /* If shortname */
    if (pFd->pFileHdl->dirHdl.lnSector == 0) {

      de.sector = pFd->pFileHdl->dirHdl.sector;
      de.offset = pFd->pFileHdl->dirHdl.offset;
      nEnt = 1;

    }

    /* Else longname */
    else {

      de.sector = pFd->pFileHdl->dirHdl.lnSector;
      de.offset = pFd->pFileHdl->dirHdl.lnOffset;
      nEnt = 0;

    }

    /* Delete */
    dosDirEntryDel(pFd->pVolDesc, &de, nEnt, TRUE);

  }

  /* Get directory entry */
  if (dosDirDirentGet(pFd, pDirEnt, RD_ENTRY) == ERROR)
    return ERROR;

  /* Encode */
  START_CLUST_ENCODE(&pDirDesc->deDesc, pFd->pFileHdl->startClust, pDirEnt);
  pDirEnt[pDirDesc->deDesc.attribOff] = pFd->pFileHdl->attrib;
  TO_DISK_32BIT(pFd->pFileHdl->size, &pDirEnt[pDirDesc->deDesc.sizeOff]);
  if (pDirDesc->deDesc.extSizeOff != (unsigned char) NONE)
    EXT_SIZE_ENCODE(&pDirDesc->deDesc, pDirEnt, pFd->pFileHdl->size);

  /* Encode time/date */
  dosDirDateEncode(pDirDesc, pDirEnt, flags, currTime);

  /* Write */
  return cbioBytesRW(pFd->pVolDesc->pCbio,
		     pFd->pFileHdl->dirHdl.sector,
		     OFFSET_IN_SEC(pFd->pVolDesc, pFd->pFileHdl->dirHdl.offset),
		     (char *) pDirEnt,
		     pDirDesc->deDesc.dirEntSize,
		     CBIO_WRITE,
		     &pFd->pFileHdl->dirHdl.cookie);
}

/*******************************************************************************
 * dosDirClustNext - Get next or add cluster to directory
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirClustNext(DOS_FILE_DESC *pFd, unsigned int alloc)
{
  block_t sec;

  /* Get alloc policy */
  if (alloc == 0)
    alloc = FAT_NOT_ALLOC;
  else
    alloc = FAT_ALLOC_ONE;

  /* Get next cluster */
  if ( ( *pFd->pVolDesc->pFatDesc->getNext) (pFd, alloc) == ERROR)
    return ERROR;

  assert(pFd->pFileHdl->startClust != 0);

  /* Nothing more to do */
  if (alloc == FAT_NOT_ALLOC)
    return OK;

  for (sec = pFd->curSec; sec < pFd->curSec + pFd->nSec; sec++) {

    if (cbioIoctl(pFd->pVolDesc->pCbio,
		  CBIO_CACHE_NEWBLK,
		  (void *) sec) == ERROR)
      return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * dosDirDeStore - Store dirent fields
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirDeStore(DOS_FILE_DESC *pFd,
			   off_t offset,
			   size_t nBytes,
			   void *buffer,
			   unsigned int which)
{
  DOS_VOL_DESC_ID volId;

  /* Inititalize locals */
  volId = pFd->pVolDesc;

  if ((which & PUT_NEXT) != 0) {

    pFd->pos += DOS_DIRENT_STD_LEN;
    if (OFFSET_IN_SEC(volId, pFd->pos) == 0) {

      pFd->curSec++;
      pFd->nSec--;

    }

  }

  /* Need new cluster group */
  if (pFd->nSec == 0)
    if (dosDirClustNext(pFd, (which & DH_ALLOC)) == ERROR)
      return ERROR;

  /* Store */
  if (cbioBytesRW(volId->pCbio, pFd->curSec,
		  OFFSET_IN_SEC(volId, pFd->pos) + offset,
		  (char *) buffer, nBytes,
		  CBIO_WRITE, &pFd->cbioCookie) == ERROR)
    return ERROR;

  return OK;
}

/*******************************************************************************
 * dosDirChkSum - Calculate checksum for longname alias
 *
 * RETURNS: Checksum
 ******************************************************************************/

LOCAL unsigned char dosDirChkSum(unsigned char *alias)
{
  unsigned char chkSum;
  unsigned int i;

  for (chkSum = 0, i = 0; i < DOS_STDNAME_LEN + DOS_STDEXT_LEN; i++)
    chkSum = ( ((chkSum & 0x01) << 7) |
	       ((chkSum & 0xfe) >> 1) ) +
	       alias[i];

  return chkSum;
}

/*******************************************************************************
 * dosDirNameCmp - Compare names
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS dosDirNameCmp(DOS_DIR_PDESC *pDirDesc,
			   unsigned char *pName,
			   unsigned char *pNameDisk,
			   BOOL caseSens)
{
  int i, entNum;

  /* Initialize locals */
  entNum = *pName & DOS_VFAT_ENTRY_MASK;

  if (entNum != (*pNameDisk & DOS_VFAT_ENTRY_MASK) )
    return ERROR;

  /* For all entries */
  for (; entNum > 0;
       pName += DOS_DIRENT_STD_LEN,
       pNameDisk += DOS_DIRENT_STD_LEN,
       entNum--) {

    /* For all chars */
    for (i = 0; i < CHAR_PER_ENTRY; i++) {

      /* If case sensitive */
      if (caseSens) {

        if (pName[charOffsets[i]] != pNameDisk[charOffsets[i]])
          return ERROR;

      } /* End if case sensitive */

      /* Else if no case sensitivity */
      else if ( __toupper(pName[charOffsets[i]]) !=
		__toupper(pNameDisk[charOffsets[i]]) ) {

        return ERROR;

      } /* End else no case sensitivity */

      if (pName[charOffsets[i]] == 0)
        break;

    } /* End for all chars */

  } /* End for all entries */

  return OK;
}

/*******************************************************************************
 * dosDirFillFd - Fill in file descriptor for directory entry
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void dosDirFillFd(DOS_FILE_DESC *pFd,
			unsigned char *pDirEnt,
			DIRENT_PTR *pLname)
{
  DOS_DIR_PDESC *pDirDesc;
  DOS_DIR_HDL *pDirHdl;
  DIRENT_DESC *pDeDesc;

  /* Inititalize locals */
  pDirDesc = (DOS_DIR_PDESC *) pFd->pVolDesc->pDirDesc;
  pDirHdl = &pFd->pFileHdl->dirHdl;
  pDeDesc = &pDirDesc->deDesc;

  /* If zero dirent */
  if (pDirEnt == NULL) {

    ROOT_SET(pFd);

    pFd->curSec = pDirDesc->dirDesc.rootStartSec;
    pFd->nSec = pDirDesc->dirDesc.rootNSec;
    pFd->pos = 0;
    pFd->cbioCookie = (cookie_t) NULL;

    pFd->pFileHdl->attrib = DOS_ATTR_DIRECTORY;
    pFd->pFileHdl->startClust = pDirDesc->rootStartClust;

    pDirHdl->sector = NONE;
    pDirHdl->offset = NONE;
    pDirHdl->lnSector = 0;
    pDirHdl->lnOffset = 0;
    pDirHdl->cookie = (cookie_t) NULL;

    goto fillFdEnd;

  }

  pDirHdl->parDirStartCluster = pFd->pFileHdl->startClust;
  pDirHdl->sector = pFd->curSec;
  pDirHdl->offset = pFd->pos;
  pDirHdl->cookie = pFd->cbioCookie;

  /* If longname */
  if (pLname) {

    pDirHdl->lnSector = pLname->sector;
    pDirHdl->lnOffset = pLname->offset;

  }

  /* Else shortname */
  else {

    pDirHdl->lnSector = 0;
    pDirHdl->lnOffset = 0;

  }

  pFd->cbioCookie = (cookie_t) NULL;

  pFd->pos = 0;
  pFd->curSec = 0;
  pFd->nSec = 0;

  /* Extract directory entry */
  pFd->pFileHdl->attrib = pDirEnt[pDeDesc->attribOff];
  pFd->pFileHdl->startClust =
	START_CLUST_DECODE(pFd->pVolDesc, pDeDesc, pDirEnt);
  pFd->pFileHdl->size = DISK_TO_32BIT(&pDirEnt[pDeDesc->sizeOff]);
  if (pDeDesc->extSizeOff != (unsigned char ) NONE)
    pFd->pFileHdl->size += EXT_SIZE_DECODE(pDeDesc, pDirEnt);

fillFdEnd:

  /* Clear */
  memset(&pFd->fatHdl, 0, sizeof(pFd->fatHdl));

  return;
}

