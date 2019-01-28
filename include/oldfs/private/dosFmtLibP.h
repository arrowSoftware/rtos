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

/* dosFmntLib.h - Dos filesystem format utilities header */

#ifndef _dosFmtLibP_h
#define _dosFmtLibP_h

#ifndef _ASMLANGUAGE

/* Defines */
#define DEFAULT_ROOT_ENTS	112		/* Default num root entries */
#define DEFAULT_MAX_ROOT_ENTS	512		/* Default max root entries */
#define DEFAULT_SEC_PER_CLUST	2		/* Default sectors per clust */
#define DEFAULT_MEDIA_BYTE	0xf8		/* Default media byte */
#define DEFAULT_NFATS		2		/* Default number of fats */
#define MAX_NFATS		16		/* Max number of fats */
#define DEFAULT_NRESERVED	1		/* Default number of reserved */

#define DOS_FAT_12BIT_MAX	4085		/* Max clusters for fat12 */
#define DOS_FAT_16BIT_MAX	(0x10000 - 2)	/* Max clusters for fat16 */
#define DOS_FAT_32BIT_MAX	(0x20000 - 2)	/* Max clusters for fat32 */

#define DOS32_INFO_SEC		1		/* Info location */
#define DOS32_BACKUP_BOOT	6		/* Boot sector backup */

#ifdef __cplusplus
extern "C" {
#endif

/* Functions */
IMPORT void dosFmtLibInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _dosFmtLibP_h */

