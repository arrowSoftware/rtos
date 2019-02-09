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

/* loadLib.c - Loader library */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <rtos/memLib.h>
#include <util/sllLib.h>
#include <util/hashLib.h>
#include <os/symbol.h>
#include <os/symLib.h>
#include <tools/moduleLib.h>
#include <tools/private/loadLibP.h>
#include <tools/loadLib.h>

/* Defines */
#define SYM_BASIC_MASK			0x0f
#define SYM_BASIC_NOT_COMM_MASK		0x0d

/* Imports */
IMPORT SYMTAB_ID sysSymTable;

/* Locals */

/* Globals */
FUNCPTR loadFunc = (FUNCPTR) NULL;
FUNCPTR loadSyncFunc = (FUNCPTR) NULL;

/* Functions */

/*******************************************************************************
 * loadLibInit - Inititalize loader library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS loadLibInit(void)
{
  return OK;
}

/*******************************************************************************
 * loadModule - Load module
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID loadModule(int fd, int symFlags)
{
  return loadModuleAt(fd, symFlags,
		      (char **) NULL, (char **) NULL, (char **) NULL);
}

/*******************************************************************************
 * loadModuleAt - Load module at system symbol table
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID loadModuleAt(int fd,
		       int symFlags,
		       char **ppText,
		       char **ppData,
		       char **ppBss)
{
  return loadModuleAtSym(fd, symFlags, ppText, ppData, ppBss, sysSymTable);
}

/*******************************************************************************
 * loadModuleAtSym - Load module at symbol table
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID loadModuleAtSym(int fd,
			  int symFlags,
			  char **ppText,
			  char **ppData,
			  char **ppBss,
			  SYMTAB_ID symTable)
{
  MODULE_ID modId;

  /* If no load func */
  if (loadFunc == NULL) {

    errnoSet(S_loadLib_ROUTINE_NOT_INSTALLED);
    return NULL;

  } /* End if no load func */

  /* Load module */
  modId = (MODULE_ID) ( *loadFunc) (fd,
				    symFlags,
				    ppText,
				    ppData,
				    ppBss,
				    symTable);
  if (modId == NULL)
    return NULL;

  /* Synchronize if function set */
  if (loadSyncFunc != NULL)
    ( *loadSyncFunc) (modId);

  return modId;
}

/*******************************************************************************
 * loadModuleGet - Create module for loadable code
 *
 * RETURNS: MODULE_ID or NULL
 ******************************************************************************/

MODULE_ID loadModuleGet(char *name, int format, int *symFlags)
{
  /* Select flags */
  switch(*symFlags) {

    case NO_SYMBOLS:
      *symFlags = LOAD_NO_SYMBOLS;
    break;

    case GLOBAL_SYMBOLS:
      *symFlags = LOAD_GLOBAL_SYMBOLS;
    break;

    case ALL_SYMBOLS:
      *symFlags = LOAD_ALL_SYMBOLS;
    break;

    default:
      *symFlags = LOAD_ALL_SYMBOLS;
    break;

  } /* End select flags */

  /* Create module */
  return moduleCreate(name, format, *symFlags);
}

/*******************************************************************************
 * loadSegmentsAllocate - Allocate text, data and bss segments
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS loadSegmentsAllocate(SEG_INFO *pSegInfo)
{
  int align, textAlign, dataAlign, bssAlign;

  /* Inititalize locals */
  textAlign = pSegInfo->flagsText;
  dataAlign = pSegInfo->flagsData;
  bssAlign = pSegInfo->flagsBss;

  /* Clear flags */
  pSegInfo->flagsText = 0;
  pSegInfo->flagsData = 0;
  pSegInfo->flagsBss = 0;

  /* If all-in-one segment */
  if ( (pSegInfo->addrText == LD_NO_ADDRESS) &&
       (pSegInfo->addrData == LD_NO_ADDRESS) &&
       (pSegInfo->addrBss == LD_NO_ADDRESS) ) {

    align = 0;
    if (textAlign > dataAlign)
      align = max(textAlign, bssAlign);
    else
      align = max(dataAlign, bssAlign);

    if (align != 0)
      pSegInfo->addrText = (char *) memalign(align, pSegInfo->sizeText +
					   	    pSegInfo->sizeData +
					   	    pSegInfo->sizeBss);
    else
      pSegInfo->addrText = (char *) malloc(pSegInfo->sizeText +
					   pSegInfo->sizeData +
					   pSegInfo->sizeBss);

    if (pSegInfo->addrText == NULL)
      return ERROR;

    /* Setup segment pointers */
    pSegInfo->addrData = pSegInfo->addrText + pSegInfo->sizeText;
    pSegInfo->addrBss = pSegInfo->addrData + pSegInfo->sizeData;

    pSegInfo->flagsText |= SEG_FREE_MEMORY;

  } /* End if all-in-one segment */

  /* If allocate memory for text segment */
  if ( (pSegInfo->addrText == LD_NO_ADDRESS) &&
       (pSegInfo->sizeText != 0) ) {

    if (textAlign != 0) {

      pSegInfo->addrText = (char *) memalign(textAlign, pSegInfo->sizeText);
      if (pSegInfo->addrText == NULL)
        return ERROR;

    }

    else {

      pSegInfo->addrText = (char *) malloc(pSegInfo->sizeText);
      if (pSegInfo->addrText == NULL)
        return ERROR;

    }

    pSegInfo->flagsText |= SEG_FREE_MEMORY;

  } /* End if allocate memory for text segment */

  /* If allocate memory for data segment */
  if ( (pSegInfo->addrData == LD_NO_ADDRESS) &&
       (pSegInfo->sizeData != 0) ) {

    if (dataAlign != 0) {

      pSegInfo->addrData = (char *) memalign(dataAlign, pSegInfo->sizeData);
      if (pSegInfo->addrData == NULL)
        return ERROR;

    }

    else {

      pSegInfo->addrData = (char *) malloc(pSegInfo->sizeData);
      if (pSegInfo->addrData == NULL)
        return ERROR;

    }

    pSegInfo->flagsData |= SEG_FREE_MEMORY;

  } /* End if allocate memory for data segment */

  /* If allocate memory for bss segment */
  if ( (pSegInfo->addrBss == LD_NO_ADDRESS) &&
       (pSegInfo->sizeBss != 0) ) {

    if (bssAlign != 0) {

      pSegInfo->addrBss = (char *) memalign(bssAlign, pSegInfo->sizeBss);
      if (pSegInfo->addrBss == NULL)
        return ERROR;

    }

    else {

      pSegInfo->addrBss = (char *) malloc(pSegInfo->sizeBss);
      if (pSegInfo->addrBss == NULL)
        return ERROR;

    }

    pSegInfo->flagsBss |= SEG_FREE_MEMORY;

  } /* End if allocate memory for bss segment */

  return OK;

}

/*******************************************************************************
 * loadCommonMatch - Fill in common info
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS loadCommonMatch(COMMON_INFO *pCommonInfo, SYMTAB_ID symTable)
{
  HASH_ID hashId;
  SYMBOL matchSym;
  SYMBOL *pSymNode;
  SYM_TYPE symType;
  int i, mask;

  /* Inititalize locals */
  hashId = symTable->nameHashId;
  pSymNode = NULL;
  matchSym.name = pCommonInfo->symName;
  matchSym.type = SYM_MASK_NONE;

  /* Get index */
  i = ( *hashId->keyFunc) (hashId->numElements, (HASH_NODE *) &matchSym,
			   hashId->keyArg);

  /* Get first symbol in bucket */
  pSymNode = (SYMBOL *) SLL_HEAD(&hashId->pHashTable[i]);

  /* For all symbols in bucket */
  while (pSymNode != NULL) {

    mask = SYM_BASIC_MASK;

    /* If symbol match */
    if ( ! ((pCommonInfo->symMatched == FALSE) &&
	    (pSymNode->group == 0)) &&
	   (strcmp(pCommonInfo->symName, pSymNode->name) == 0) ) {

      if ( (pSymNode->type & SYM_COMM) == SYM_COMM)
        mask = SYM_BASIC_NOT_COMM_MASK;

        /* Get type */
      symType = pSymNode->type & mask;

        /* If bss symbol */
      if ( (symType == (SYM_BSS | SYM_GLOBAL)) &&
	   (pCommonInfo->pSymAddrBss == NULL) ) {

        pCommonInfo->pSymAddrBss = pSymNode->value;
        pCommonInfo->bssSymType = pSymNode->type;

      } /* End if bss symbol */

      /* If data symbol */
      if ( (symType == (SYM_DATA | SYM_GLOBAL)) &&
	   (pCommonInfo->pSymAddrData == NULL) ) {

        pCommonInfo->pSymAddrData = pSymNode->value;
        pCommonInfo->dataSymType = pSymNode->type;

      } /* End if data symbol */

    } /* End if symbol match */

    /* Advance */
    pSymNode = (SYMBOL *) SLL_NEXT((HASH_NODE *) pSymNode);

  } /* End for all symbols in bucket */

  return OK;
}

/*******************************************************************************
 * loadCommonManage - Process common symbol
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS loadCommonManage(int size,
			int align,
			char *symName,
			SYMTAB_ID symTable,
			SYM_ADDR *pSymAddr,
			SYM_TYPE *pSymType,
			int loadFlag,
			SEG_INFO *pSegInfo,
			unsigned short group)
{
  COMMON_INFO commonInfo;

  /* Fix flags */
  if ( ((loadFlag & LOAD_COMMON_MATCH_ALL) == 0) &&
       ((loadFlag & LOAD_COMMON_MATCH_USER) == 0) &&
       ((loadFlag & LOAD_COMMON_MATCH_NONE) == 0) )
    loadFlag |= LOAD_COMMON_MATCH_NONE;

  /* If any match */
  if ((loadFlag & LOAD_COMMON_MATCH_NONE) == 0) {

    memset(&commonInfo, 0, sizeof(COMMON_INFO));
    commonInfo.symName = symName;

    if (loadFlag & LOAD_COMMON_MATCH_USER)
      commonInfo.symMatched = FALSE;
    else if (loadFlag & LOAD_COMMON_MATCH_ALL)
      commonInfo.symMatched = TRUE;

    /* Fill in common info struct */
    loadCommonMatch(&commonInfo, symTable);

    /* If bss symbol */
    if (commonInfo.pSymAddrBss != NULL) {

      *pSymAddr = commonInfo.pSymAddrBss;
      *pSymType = commonInfo.bssSymType;

    } /* End if bss symbol */

    /* If data symbol */
    if (commonInfo.pSymAddrData != NULL) {

      *pSymAddr = commonInfo.pSymAddrData;
      *pSymType = commonInfo.dataSymType;

    } /* End if bss symbol */

    /* Else not bss or data symbol */
    else {

      *pSymAddr = NULL;

    } /* End else not bss or data symbol */

    /* If match found, return */
    if (*pSymAddr != NULL)
      return OK;

  } /* End if any match */

  /* Set symbol type */
  *pSymType = SYM_BSS | SYM_COMM | SYM_GLOBAL;

  /* Allocate memory */
  if (align != 0)
    *pSymAddr = memalign(align, size);
  else
    *pSymAddr = malloc(size);

  if (*pSymAddr == NULL)
    return ERROR;

  /* Clear */
  memset((SYM_ADDR) *pSymAddr, 0, size);

  /* If load symbol */
  if ( ((loadFlag & LOAD_NO_SYMBOLS) == 0) &&
        (loadFlag & LOAD_GLOBAL_SYMBOLS) ) {

    if (symAdd(symTable, symName, *pSymAddr, *pSymType, group) != OK) {

      fprintf(stderr, "Can't add '%s' to symbol table.\n", symName);
      *pSymAddr = NULL;
      return ERROR;

    }

  } /* End if load symbol */

  return OK;
}

