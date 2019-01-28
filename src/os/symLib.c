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

/* symLib.c - Symbol table library */

#include <stdlib.h>
#include <string.h>
#include <rtos/ffsLib.h>
#include <os/symbol.h>
#include <os/symLib.h>

/* Defines */
#define SYM_HASH_FUNC_SEED		1370364821

/* Impots */
IMPORT FUNCPTR _func_symFindSymbol;
IMPORT FUNCPTR _func_symNameGet;
IMPORT FUNCPTR _func_symValueGet;
IMPORT FUNCPTR _func_symTypeGet;

/* Locals */
LOCAL OBJ_CLASS symTableClass;
LOCAL BOOL symLibInstalled = FALSE;
LOCAL int symLibMutexOptions = SEM_Q_FIFO | SEM_DELETE_SAFE;

LOCAL BOOL symHashKeyCmpName(SYMBOL_ID matchSymId, SYMBOL_ID symId, int mask);
LOCAL int symHashFuncName(int numElements, SYMBOL_ID symId, int seed);
LOCAL BOOL symEachFunc(SYMBOL *pSym, SYM_FUNC_DESC *pFuncDesc);

/* Globals */
CLASS_ID symTableClassId = &symTableClass;
unsigned short symGroupDefault = 0;

/*******************************************************************************
 * symLibInit - Initialize symbol table libarary
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symLibInit(void)
{
  /* Check if already installed */
  if (symLibInstalled)
    return OK;

  /* Initialize global functions */
  _func_symFindSymbol = (FUNCPTR) symFindSymbol;
  _func_symNameGet = (FUNCPTR) symNameGet;
  _func_symValueGet = (FUNCPTR) symValueGet;
  _func_symTypeGet = (FUNCPTR) symTypeGet;

  /* Install class */
  if (classInit(&symTableClass, sizeof(SYMTAB),
		OFFSET(SYMTAB, objCore),
		memSysPartId,
		(FUNCPTR ) symTableCreate,
		(FUNCPTR ) symTableInit,
		(FUNCPTR ) symTableDestroy) != OK)
    return ERROR;

  /* Mark as installed */
  symLibInstalled = TRUE;
  
  return OK;
}

/*******************************************************************************
 * symTableCreate - Create a symbol table
 *
 * RETURNS: Symbol table id
 ******************************************************************************/

SYMTAB_ID symTableCreate(int log2Size, BOOL sameNameOk, PART_ID partId)
{

  SYMTAB_ID symTableId;
  HASH_ID nameHashId;

  /* Allocate object */
  symTableId = (SYMTAB_ID) objAlloc(symTableClassId);
  if (symTableId == NULL)
    return NULL;

  /* Create hash table */
  nameHashId = hashTableCreate(log2Size,
			       (FUNCPTR) symHashKeyCmpName,
			       (FUNCPTR) symHashFuncName,
			       SYM_HASH_FUNC_SEED);
  if (nameHashId == NULL) {

    objFree(symTableClassId, symTableId);
    return NULL;

  }

  /* Initalize object */
  if (symTableInit(symTableId, sameNameOk, partId, nameHashId) != OK) {

    hashTableDelete(nameHashId);
    objFree(symTableClassId, symTableId);
    return NULL;

  }

  return symTableId;
}

/*******************************************************************************
 * symTableInit - Initialize symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTableInit(SYMTAB_ID symTableId, BOOL sameNameOk,
		    PART_ID partId, HASH_ID nameHashId) 
{

  /* Verify hash table object class */
  if (OBJ_VERIFY(nameHashId, hashClassId) != OK)
    return ERROR;

  /* Initialize mutex */
  if (semMInit(&symTableId->symMutex, symLibMutexOptions) != OK)
    return ERROR;

  /* Initialize data struct */
  symTableId->nameHashId = nameHashId;
  symTableId->sameNameOk = sameNameOk;
  symTableId->numSymbols = 0;
  symTableId->partId = partId;

  /* Initialize object class */
  objCoreInit(&symTableId->objCore, symTableClassId);

  return OK;
}

/*******************************************************************************
 * symTableDestroy - Destroy symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTableDestroy(SYMTAB_ID symTableId, BOOL dealloc)
{
  /* Verify object class */
  if (OBJ_VERIFY(symTableId, symTableClassId) != OK)
    return ERROR;

  /* Get exclusive access */
  semTake(&symTableId->symMutex, WAIT_FOREVER);

  /* If not empty */
  if (symTableId->numSymbols > 0) {

    semGive(&symTableId->symMutex);
    return ERROR;

  }

  /* Terminate semaphore */
  semTerminate(&symTableId->symMutex);

  /* Terminate object core */
  objCoreTerminate(&symTableId->objCore);

  /* Destroy name hash table */
  hashTableDestroy(symTableId->nameHashId, dealloc);

  /* Deallocte */
  if (dealloc)
    return objFree(symTableClassId, symTableId);

  return OK;
}

/*******************************************************************************
 * symTableDelete - Delete symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTableDelete(SYMTAB_ID symTableId)
{
  return symTableDestroy(symTableId, TRUE);
}

/*******************************************************************************
 * symTableTerminate - Terminate symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTableTerminate(SYMTAB_ID symTableId)
{
  return symTableDestroy(symTableId, FALSE);
}

/*******************************************************************************
 * symCreate - Create a new symbol entry for symbol table
 *
 * RETURNS: Symbol id
 ******************************************************************************/

SYMBOL_ID symCreate(SYMTAB_ID symTableId, char *name, void *value,
		    SYM_TYPE type, unsigned short group)
{
  SYMBOL_ID symId;
  char *symName;
  int len;

  /* Verify object class */
  if (OBJ_VERIFY(symTableId, symTableClassId) != OK)
    return NULL;

  /* Verify symbol name string */
  if (name == NULL)
    return NULL;

  /* Get name string length */
  len = strlen(name);

  /* Allocate memory for symbol and symbol name string */
  symId = (SYMBOL *) memPartAlloc(symTableId->partId,
				  (unsigned) (sizeof(SYMBOL) + len + 1) );
  if (symId == NULL)
    return NULL;

  /* Copy symbol name string after symbol storage area */
  symName = (char *) ( (unsigned) symId + sizeof(SYMBOL) );
  symName[len] = EOS;
  strncpy(symName, name, len);

  /* Initialize symbol */
  symInit(symId, symName, value, type, group);

  return symId;
}

/*******************************************************************************
 * symInit - Initialize a symbol
 *
 * RETURNS: OK
 ******************************************************************************/

STATUS symInit(SYMBOL_ID symId, char *name, void *value,
	       SYM_TYPE type, unsigned short group)
{
  /* Setup struct */
  symId->name = name;
  symId->value = value;
  symId->type = type;
  symId->group = group;

  return OK;
}

/*******************************************************************************
 * symDestroy - Destroy a symbol
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symDestroy(SYMTAB_ID symTableId, SYMBOL_ID symId)
{
  /* Verify object class */
  if (OBJ_VERIFY(symTableId, symTableClassId) != OK)
    return ERROR;

  return memPartFree(symTableId->partId, symId);
}

/*******************************************************************************
 * symAdd - Create and add a symbol to symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symAdd(SYMTAB_ID symTableId, char *name, void *value,
	      SYM_TYPE type, unsigned short group)
{
  SYMBOL_ID symId;

  /* Allocate symbol */
  symId = symCreate(symTableId, name, value, type, group);

  /* Add symbol to symbol table */
  if (symTableAdd(symTableId, symId) != OK) {

    symDestroy(symTableId, symId);
    return ERROR;

  }

  return OK;
}

/*******************************************************************************
 * symTableAdd - Add a symbol to symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTableAdd(SYMTAB_ID symTableId, SYMBOL_ID symId)
{
  /* Verify object class */
  if (OBJ_VERIFY(symTableId, symTableClassId) != OK)
    return ERROR;

  /* Get exclusive access */
  semTake(&symTableId->symMutex, WAIT_FOREVER);

  /* If same name not okay */
  if (!symTableId->sameNameOk) {

    /* If matching symbol exists */
    if (hashTableFind(symTableId->nameHashId, &symId->nameHashNode,
		      SYM_MASK_EXACT_TYPE) != NULL) {

      /* Give away exclusive acccess */
      semGive(&symTableId->symMutex);
      return ERROR;

    } /* End if matching symbol exists */

  } /* End if same name not okay */

  /* Put symbol on the name hash table */
  hashTablePut(symTableId->nameHashId, &symId->nameHashNode);

  /* Increase number of symbols in symbol table */
  symTableId->numSymbols++;

  /* Give away exclusive access */
  semGive(&symTableId->symMutex);

  return OK;
}

/*******************************************************************************
 * symFindByName - Lookup a symbol in the symbol table by name
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symFindByName(SYMTAB_ID symTableId, char *name, void **pValue,
		     SYM_TYPE *pType)
{
  return symFindByNameAndType(symTableId, name, pValue, pType,
			      SYM_MASK_ANY_TYPE, SYM_MASK_ANY_TYPE);
}

/*******************************************************************************
 * symFindByNameAndType - Lookup a symbol in the symbol table by name and type
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symFindByNameAndType(SYMTAB_ID symTableId, char *name, void **pValue,
			    SYM_TYPE *pType, SYM_TYPE sType, SYM_TYPE mask)
{
  SYMBOL_ID symId;
  char **pVal;

  /* Initialize locals */
  symId = NULL;

  /* Search for symbol */
  if (symFindSymbol(symTableId, name, NULL, sType,
		    mask, &symId) != OK)
    return ERROR;

  /* Store value */
  if (pValue != NULL) {

    pVal = (char **) pValue;
    *pVal = (char *) symId->value;

  }

  /* Store type */
  if (pType != NULL)
    *pType = symId->type;

  return OK;
}

/*******************************************************************************
 * symFindSymbol - Lookup a symbol in the symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symFindSymbol(SYMTAB_ID symTableId, char *name, void *value,
		     SYM_TYPE type, SYM_TYPE mask, SYMBOL_ID *pSymId)
{
  HASH_NODE *pHashNode;
  SYMBOL keySym;
  SYMBOL *pSym, *pCloseSym;
  void *closeValue;
  int i;

  /* Verify object class */
  if (OBJ_VERIFY(symTableId, symTableClassId) != OK)
    return ERROR;

  /* Initialize locals */
  pCloseSym = NULL;
  closeValue = NULL;

  /* If non-null name */
  if (name != NULL) {

    /* Fill in key */
    keySym.name = name;
    keySym.type = type;

    /* Get exclusive access */
    semTake(&symTableId->symMutex, WAIT_FOREVER);

    pHashNode = hashTableFind(symTableId->nameHashId, &keySym.nameHashNode,
			      (int) mask);

    /* Give away exclusive acccess */
    semGive(&symTableId->symMutex);

    if (pHashNode == NULL)
      return ERROR;

    /* Store found symbol */
    *pSymId = (SYMBOL_ID) pHashNode;

  } /* End if non-null name */

  /* Else null name */
  else {

    /* Get exclusive access */
    semTake(&symTableId->symMutex, WAIT_FOREVER);

    /* For all hash buckets */
    for (i = 0; i < symTableId->nameHashId->numElements; i++) {

      /* Get first symbol in current bucket */
      pSym = (SYMBOL *) SLL_HEAD(&symTableId->nameHashId->pHashTable[i]);

      /* While symbol exists */
      while (pSym != NULL) {

        /* If symbol matches */
        if ( (pSym->type & mask) == (type & mask) &&
	     (pSym->value == value) ) {

          /* Store symbol */
          *pSymId = pSym;

          /* Give away exclusive acccess */
          semGive(&symTableId->symMutex);

          return OK;

        } /* End if symbol matches */

        /* Else if this is a closer match */
        else if ( ( (pSym->type & mask) == (type & mask) ) &&
		  ( (pSym->value <= value) && (pSym->value > closeValue) ) ) {

          /* Store close symbol-value pair */
	  pCloseSym = pSym;
          closeValue = pSym->value;

        } /* End else if this is a closer match */

        /* Advance */
        pSym = (SYMBOL *) SLL_NEXT(&pSym->nameHashNode);

      } /* While symbol exists */

    } /* End for all hash buckets */

    /* If a close match was not found */
    if (closeValue == NULL || pCloseSym == NULL) {

      /* Give away exclusive acccess */
      semGive(&symTableId->symMutex);

      return ERROR;

    } /* End if a close match was not found */

    /* Store pointer to symbol id */
    *pSymId = pCloseSym;

    /* Give away exclusive acccess */
    semGive(&symTableId->symMutex);

  } /* End else null name */

  return OK;
}

/*******************************************************************************
 * symNameGet - Get symbol name
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symNameGet(SYMBOL_ID symId, char **pName)
{
  if (pName == NULL)
     return ERROR;

  *pName = symId->name;

  return OK;
}

/*******************************************************************************
 * symValueGet - Get symbol value
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symValueGet(SYMBOL_ID symId, void **pValue)
{
  if (pValue == NULL)
     return ERROR;

  *pValue = symId->value;

  return OK;
}

/*******************************************************************************
 * symTypeGet - Get symbol type
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTypeGet(SYMBOL_ID symId, SYM_TYPE *pType)
{
  if (pType == NULL)
     return ERROR;

  *pType = symId->type;

  return OK;
}

/*******************************************************************************
 * symEach - Call routine for each symbol in symbol table
 *
 * RETURNS: Symbol id where it stopped
 ******************************************************************************/

SYMBOL* symEach(SYMTAB_ID symTableId, FUNCPTR func, ARG arg)
{
  SYM_FUNC_DESC funcDesc;
  SYMBOL *pSym;

  /* Verify object class */
  if ( OBJ_VERIFY(symTableId, symTableClassId) != OK )
    return NULL;

  /* Setup function struct */
  funcDesc.func = func;
  funcDesc.arg = arg;

  /* Get exclusive access to symbol table */
  semTake(&symTableId->symMutex, WAIT_FOREVER);

  /* Call hash table for each function */
  pSym = (SYMBOL *) hashTableEach(symTableId->nameHashId,
				  symEachFunc,
				  (ARG) &funcDesc);

  /* Unlock table */
  semGive(&symTableId->symMutex);

  return pSym;
}

/*******************************************************************************
 * symRemove - Remove and destroy a symbol from symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symRemove(SYMTAB_ID symTableId, char *name, SYM_TYPE type)
{
  SYMBOL_ID symId;

  /* Type to find symbol */
  if (symFindSymbol(symTableId, name, NULL,
		    type, SYM_MASK_EXACT_TYPE, &symId) != OK)
    return ERROR;

  /* Remove symbol */
  if (symTableRemove(symTableId, symId) != OK)
    return ERROR;

  return symDestroy(symTableId, symId);
}

/*******************************************************************************
 * symTableRemove - Remove a symbol from symbol table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS symTableRemove(SYMTAB_ID symTableId, SYMBOL_ID symId)
{
  HASH_NODE *pHashNode;

  /* Verify object class */
  if (OBJ_VERIFY(symTableId, symTableClassId) != OK)
    return ERROR;

  /* Get exclusive access */
  semTake(&symTableId->symMutex, WAIT_FOREVER);

  /* Find the requested node */
  pHashNode = hashTableFind(symTableId->nameHashId, &symId->nameHashNode,
			    SYM_MASK_EXACT);
  if (pHashNode == NULL) {

    /* Give away exclusive acccess */
    semGive(&symTableId->symMutex);
    return ERROR;

  }

  /* Remove node from name hash table */
  hashTableRemove(symTableId->nameHashId, pHashNode);

  /* Decrease number of symbols in symbol table */
  symTableId->numSymbols--;

  /* Give away exclusive access */
  semGive(&symTableId->symMutex);

  return OK;
}

/*******************************************************************************
 * symHashKeyCmpName - Name compare for hash table
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL symHashKeyCmpName(SYMBOL_ID matchSymId, SYMBOL_ID symId, int mask)
{
  SYM_TYPE symMask;

  /* If exact match requred */
  if (mask == SYM_MASK_EXACT) {

    /* If it is the same symbol */
    if (matchSymId == symId)
      return TRUE;
    else
      return FALSE;

  } /* End if exact match required */

  /* Get mask */
  symMask = mask;

  /* Return result of both mask and name match */
  return ( ( (symId->type & symMask) == (matchSymId->type & symMask) ) &&
	   ( strcmp(matchSymId->name, symId->name) == 0 ) );
}

/*******************************************************************************
 * symHashFuncName - Hash function for name
 *
 * RETURNS: Integer hash value
 ******************************************************************************/

LOCAL int symHashFuncName(int numElements, SYMBOL_ID symId, int seed)
{
  int key, hash;
  char *pKeyChar;

  /* Initialize locals */
  key = 0;

  /* For all characters in symbol name string */
  for (pKeyChar = symId->name; *pKeyChar != EOS; pKeyChar++)
    key = key + (unsigned int) *pKeyChar;

  /* Calculate hash value */
  hash = key * seed;
  hash = hash >> ( 33 - ffsMsb(numElements) );

  return ( hash & (numElements - 1) );
}

/*******************************************************************************
 * symEachFunc - Called for each node in hash table
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL symEachFunc(SYMBOL *pSym, SYM_FUNC_DESC *pFuncDesc)
{
  return ( ( *pFuncDesc->func) (pSym->name,
				pSym->value,
				pSym->type,
				pFuncDesc->arg,
				pSym->group,
				pSym) );

}

