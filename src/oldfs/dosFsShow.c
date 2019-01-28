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

/* dosFsShow.c - Dos filesystem show functions */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/private/dosFsLibP.h>
#include <oldfs/dosFsShow.h>

/* Defines */

/* Imports */

/* Locals */
LOCAL char volLabel[DOS_VOL_LABEL_LEN + 1];

/* Globals */

/* Functions */

/*******************************************************************************
 * dosFsShowInit - Initialize show library
 *
 * RETURNS: N/A
 ******************************************************************************/

void dosFsShowInit(void)
{
}

/*******************************************************************************
 * dosFsShow - Show information about dos filesystem
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS dosFsShow(void *idOrName, int level)
{
  DOS_VOL_DESC_ID volId;
  DOS_FILE_DESC *pFd;
  DOS_FILE_HDL *pFileHdl;
  char str1[14], str2[19];
  int nUsed, nOpen, nDeleted, nObsolete;

  /* Get volume descriptor */
  volId = dosFsVolDescGet(idOrName, NULL);
  if (volId == NULL) {

    printf(" Not a DOSFS Device.\n");
    return ERROR;

  }

  /* Get volume label and mount device */
  dosFsVolLabelGet(volId, volLabel);
  if (*volLabel == EOS)
    strcpy(volLabel, "NO LABEL");

  printf("\n");
  printf("volume descriptor (pVolDesc):              %p\n", volId);
  printf("block I/O descriptor ptr (cbio):           %p\n", volId->pCbio);

  printf("auto disk check on mount:                  ");
  if (volId->autoChk == 0) {

    printf("NOT ENABLED\n");

  }

  else {

    if (volId->autoChk == DOS_CHK_ONLY)
      strcpy(str1, "DOS_CHK_ONLY");
    else
      strcpy(str1, "DOS_CHK_REPAIR");

    if ((volId->autoChkVerb << 8) == DOS_CHK_VERB_0) {

      strcpy(str2, "DOS_CHK_VERB_SILENT");

    }

    else {

      if ((volId->autoChkVerb << 8) == DOS_CHK_VERB_1)
        strcpy(str2, "DOS_CHK_VERB_1");
      else
        strcpy(str2, "DOS_CHK_VERB_2");

    }

    printf("%s | %s\n", str1, str2);

  }

  /* Open filedescriptors */
  nUsed = 0;
  nOpen = 0;
  nDeleted = 0;
  nObsolete = 0;
  for (pFd = volId->pFdList, pFileHdl = volId->pFhdlList;
       pFd < volId->pFdList + volId->maxFiles;
       pFd++, pFileHdl++) {

    if (pFd->busy)
      nUsed++;

    if (pFileHdl->nRef > 0) {

      nOpen++;

      if (pFileHdl->obsolete)
        nObsolete;
      else if (pFileHdl->deleted)
        nDeleted++;

    }

  }

  printf("max # of simultaneously open files:        %u\n", volId->maxFiles);
  printf("file descriptors in use:                   %d\n", nUsed);
  printf("# of different files in use:               %d\n", nOpen);
  printf("# of descriptors for deleted files:        %d\n", nDeleted);
  printf("# of obsolete descriptors:                 %d\n\n", nObsolete);

  if (level == 0)
    goto noBootPrint;

  /* Boot sector */
  if (!volId->mounted) {

    printf("can't mount volume\n\n");
    goto noBootPrint;

  }

  /* Get FAT string */
  switch (volId->fatType) {

    case FAT12: strcpy(str1, "FAT12"); break;
    case FAT16: strcpy(str1, "FAT16"); break;
    case FAT32: strcpy(str1, "FAT32"); break;

  }

  if (!volId->mounted) {

    printf(" can't mount volume\n\n");
    goto noBootPrint;

  }

  printf("current volume configuration:\n");

  printf(" - volume label:    %s; (in boot sector:    %s)\n",
	 volLabel, volId->bootVolLab);

  printf(" - volume Id:                              %p\n",
	 volId->volId);
  printf(" - total number of sectors:                %d\n",
	 volId->totalSec);
  printf(" - bytes per sector:                       %d\n",
	 volId->bytesPerSec);
  printf(" - # of sectors per cluster:               %d\n",
	 volId->secPerClust);
  printf(" - # of reserved sectors:                  %d\n",
	 volId->nReservedSecs);

  printf(" - FAT entry size:                         %s\n",
	 str1);

  printf(" - # of sectors per FAT copy:              %d\n",
	 volId->secPerFat);
  printf(" - # of FAT table copies:                  %u\n",
	 volId->nFats);
  printf(" - # of hidden sectors:                    %u\n",
	 volId->nHiddenSecs);
  printf(" - first cluster in sector #               %u\n",
	 volId->dataStartSec);

  if (volId->updateLastAccessDate)
    strcpy(str1, "TRUE");
  else
    strcpy(str1, "FALSE");

  printf(" - Update last access date for open-read-close = %s\n\n", str1);

noBootPrint:

  if (level < 2)
    goto noHandlerPrint;

  /* Handler show */
  if ( (volId->pDirDesc != NULL) &&
       (volId->pDirDesc != NULL) )
    ( *volId->pDirDesc->show) (volId);

  printf("\n");

  if ( (volId->pFatDesc != NULL) &&
       (volId->pFatDesc != NULL) )
    ( *volId->pFatDesc->show) (volId);

noHandlerPrint:

  return OK;
}

/*******************************************************************************
 * dosFsFmtShow - Show volume parameters
 *
 * RETURNS: N/A
 ******************************************************************************/

void dosFmtShow(DOS_VOL_CONFIG *pConf)
{
  printf("\n");
  printf("Volume Parameters: FAT type: FAT%d, sectors per cluster %d\n",
	 pConf->fatType, pConf->secPerClust);
  printf("  %d FAT copies, %ld clusters, %ld sectors per FAT\n",
	 pConf->nFats, pConf->nClust, pConf->secPerFat);
  printf("  Sectors reserved %d, hidden %ld, FAT sectors %ld\n",
         pConf->nReserved, pConf->nHidden, pConf->secPerFat * pConf->nFats);
  printf("  Root dir entries %d, sysId %-8s, serial number %lx\n",
	 pConf->maxRootEnts, pConf->sysId, pConf->volSerial);
  printf("  Label: \"%-11s\" ...\n", pConf->volLabel);
}

