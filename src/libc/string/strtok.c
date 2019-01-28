/*******************************************************************************
 * File:
 *  strtok.c
 *
 * Description:
 *	This file contains the function to tokenize a string.  To turn a string into
 *	smaller stings by breaking the string about specified by the delimiters.
 *
 * Members:
 *	strtok()
 *
 * Todo:
 *	None.
 *
 * Modification History:
 *  Date         Who         Description
 *  ============================================================================
 *	01/25/2019   T.Gajewski	 Initial Version.
 ******************************************************************************/
#include "string.h"

static char *strtokSave = 0;

char *strtok(char *argStr, const char *argDelim)
{
	return strtok_r(argStr, argDelim, &strtokSave);
}

