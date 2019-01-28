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

/* hashLib.c - Hash table library */

#include <stdlib.h>
#include <rtos.h>
#include <rtos/memPartLib.h>
#include <util/hashLib.h>

/* Imports */

/* Locals */
LOCAL OBJ_CLASS hashClass;
LOCAL BOOL hashLibInstalled = FALSE;

/* Globals */
CLASS_ID hashClassId = &hashClass;

/*******************************************************************************
 * hashLibInit - Initialize hash table library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashLibInit(void)
{
  /* Check if already installed */
  if (hashLibInstalled)
    return OK;

  /* Install object class */
  if (classInit(hashClassId, sizeof(HASH_TABLE),
		OFFSET(HASH_TABLE, objCore),
		memSysPartId,
		(FUNCPTR) hashTableCreate,
		(FUNCPTR) hashTableInit,
		(FUNCPTR) hashTableDestroy) != OK)
    return ERROR;

  /* Mark as installed */
  hashLibInstalled = TRUE;
}

/*******************************************************************************
 * hashTableCreate - Create hash table
 *
 * RETURNS: HASH_ID
 ******************************************************************************/

HASH_ID hashTableCreate(int log2Size, FUNCPTR keyCmpFunc,
		        FUNCPTR keyFunc, int keyArg)
{
  HASH_ID hashId;
  SL_LIST *pList;
  int size;

  /* Get size plus size log 2 */
  size = (1 << log2Size) * sizeof(SL_LIST);

  /* Allocate object */
  hashId = (HASH_ID) objAllocPad(hashClassId, size, (void **) &pList);
  if (hashId == NULL)
    return NULL;

  /* Initialize hash table */
  hashTableInit(hashId, pList, log2Size, keyCmpFunc, keyFunc, keyArg);

  return hashId;
}

/*******************************************************************************
 * hashTableInit - Initialize hash table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashTableInit(HASH_ID hashId, SL_LIST *pList, int log2Size,
		     FUNCPTR keyCmpFunc, FUNCPTR keyFunc, int keyArg)
{
  int i;

  /* Initialize data struct */
  hashId->numElements = 1 << log2Size;
  hashId->keyCmpFunc = keyCmpFunc;
  hashId->keyFunc = keyFunc;
  hashId->keyArg = keyArg;
  hashId->pHashTable = pList;

  /* Initialize bucket lists in table */
  for (i = 0; i < hashId->numElements; i++)
    sllInit(&hashId->pHashTable[i]);

  /* Initialize object class */
  objCoreInit(&hashId->objCore, hashClassId);

  return OK;
}

/*******************************************************************************
 * hashTableDestroy - Destroy hash table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashTableDestroy(HASH_ID hashId, BOOL dealloc)
{
  /* Check object */
  if (OBJ_VERIFY(hashId, hashClassId) != OK)
    return ERROR;

  /* Kill object class */
  objCoreTerminate(&hashId->objCore);

  /* Deallocate */
  if (dealloc)
    return objFree(hashClassId, hashId);

  return OK;
}

/*******************************************************************************
 * hashTableTerminate - Devalidate hash table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashTableTerminate(HASH_ID hashId)
{
  return hashTableDestroy(hashId, FALSE);
}

/*******************************************************************************
 * hashTableDelete - Delete hash table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashTableDelete(HASH_ID hashId)
{
  return hashTableDestroy(hashId, TRUE);
}

/*******************************************************************************
 * hashTablePut - Put a node on the hash table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashTablePut(HASH_ID hashId, HASH_NODE *pHashNode)
{
  int i;

  /* Verify object */
  if (OBJ_VERIFY(hashId, hashClassId) != OK)
    return ERROR;

  /* Invoke hash function to get index to bucket in hash table */
  i = (* hashId->keyFunc) (hashId->numElements, pHashNode, hashId->keyArg);

  /* Put node in the correct bucket */
  sllPutAtHead(&hashId->pHashTable[i], pHashNode);

  return OK;
}

/*******************************************************************************
 * hashTableFind - Find node in the hash table
 *
 * RETURNS: Pointer to hash node
 ******************************************************************************/

HASH_NODE* hashTableFind(HASH_ID hashId, HASH_NODE *pMatchNode,
			 int keyCmpArg)
{
  HASH_NODE *pHashNode;
  int i;

  /* Verify object */
  if (OBJ_VERIFY(hashId, hashClassId) != OK)
    return NULL;

  /* Invoke hash function to get index to bucket in hash table */
  i = (* hashId->keyFunc) (hashId->numElements, pMatchNode, hashId->keyArg);

  /* Get first node from bucket */
  pHashNode = (HASH_NODE *) SLL_HEAD(&hashId->pHashTable[i]);

  /* Search bucket for match */
  while ( (pHashNode != NULL) &&
          !( (* hashId->keyCmpFunc) (pMatchNode, pHashNode, keyCmpArg) ) )
    pHashNode = (HASH_NODE *) SLL_NEXT(pHashNode);

  return pHashNode;
}

/*******************************************************************************
 * hashTableEach - Call function for each node in hash table
 *
 * RETURNS: Hash node id where it stopped
 ******************************************************************************/

HASH_NODE* hashTableEach(HASH_ID hashId, FUNCPTR func, ARG arg)
{
  int i;
  HASH_NODE *pHashNode;

  /* Verify object class */
  if ( OBJ_VERIFY(hashId, hashClassId) != OK)
    return NULL;

  /* Setup locals */
  pHashNode = NULL;

  /* For all nodes in table */
  for (i = 0; (i < hashId->numElements) && (pHashNode == NULL); i++)
    pHashNode = (HASH_NODE *) sllEach(&hashId->pHashTable[i], func, arg);

  return pHashNode;
}

/*******************************************************************************
 * hashRemove - Remove node from hash table
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS hashTableRemove(HASH_ID hashId, HASH_NODE *pHashNode)
{
  HASH_NODE *pPrevNode;
  int i;

  /* Verify object */
  if (OBJ_VERIFY(hashId, hashClassId) != OK)
    return ERROR;

  /* Invoke hash function to get index to bucket in hash table */
  i = (* hashId->keyFunc) (hashId->numElements, pHashNode, hashId->keyArg);

  /* Get node before the requested node */
  pPrevNode = sllPrevious(&hashId->pHashTable[i], pHashNode);

  /* Remove node */
  sllRemove(&hashId->pHashTable[i], pHashNode, pPrevNode);

  return OK;
}

/*******************************************************************************
 * hashKeyCmp - Compare two hash keys
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL hashKeyCmp(H_NODE_INT *pMatchNode, H_NODE_INT *pHashNode,
		int keyCmpArg)
{

  /* If match */
  if (pHashNode->key == pMatchNode->key)
    return TRUE;

  return FALSE;
}

/*******************************************************************************
 * hashKeyStringCmp - Compare two hash key strings
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

BOOL hashKeyStringCmp(H_NODE_STRING *pMatchNode, H_NODE_STRING *pHashNode,
		      int keyCmpArg)
{

  /* If match */
  if ( strcmp(pHashNode->str, pMatchNode->str) == 0 )
    return TRUE;

  return FALSE;
}

