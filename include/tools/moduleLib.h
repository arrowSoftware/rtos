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

/* moduleLib.h - Module library header */

#ifndef _moduleLib_h
#define _moduleLib_h

#include <tools/moduleNumber.h>

#define S_moduleLib_MODULE_NOT_FOUND		(M_moduleLib | 0x0001)
#define S_moduleLib_HOOK_NOT_FOUND		(M_moduleLib | 0x0002)
#define S_moduleLib_BAD_CHECKSUM		(M_moduleLib | 0x0003)
#define S_moduleLib_MAX_MODULES_LOADED		(M_moduleLib | 0x0004)

#include <rtos.h>
#include <util/classLib.h>
#include <util/dllLib.h>

#define MODULE_GROUP_MAX	65535

/* Module types */
#define MODULE_A_OUT		0
#define MODULE_B_OUT		1
#define MODULE_ECOFF		2
#define MODULE_ELF		3

#define MODULE_REPLACED		0x00010000	/* Flags if replaced */
#define SEG_FREE_MEMORY		1		/* Free segment */

#define NAME_MAX		32

#define SEGMENT_TEXT		2		/* Text segment */
#define SEGMENT_DATA		4		/* Data segment */
#define SEGMENT_BSS		8		/* Bss segment */

#define MODCHECK_NOPRINT	1		/* No printout */
#define MODCHECK_TEXT		SEGMENT_TEXT
#define MODCHECK_DATA		SEGMENT_DATA
#define MODCHECK_BSS		SEGMENT_BSS

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct {
  OBJ_CORE			objCore;	/* Object class */
  DL_NODE			moduleNode;	/* List node */
  char				name[NAME_MAX];	/* Module name */
  char				path[NAME_MAX];	/* Module path */
  int 				flags;		/* Flags for loader */
  DL_LIST			segmentList;	/* List of segments */
  int				format;		/* Object module format */
  unsigned short		group;		/* Group number */
  DL_LIST			dependencies;	/* List of depedencies */
  DL_LIST			referents;	/* List of referents */
  VOIDFUNCPTR			*ctors;		/* List of constructors */
  VOIDFUNCPTR			*dtors;		/* List of destructors */
  void				*user1;		/* User defined */
  void				*reserved1;
  void				*reserved2;
  void				*reserved3;
  void				*reserved4;
} MODULE;

typedef MODULE *MODULE_ID;

typedef struct {
  DL_NODE			segmentNode;	/* List node */
  void				*address;	/* Segment address */
  int				size;		/* Segment size */
  int				type;		/* Segment type */
  int				flags;		/* Segment flags */
  unsigned short		checksum;	/* Segment checksum */
} SEGMENT;

typedef SEGMENT *SEGMENT_ID;

typedef struct {
  void				*textAddr;	/* Text segment address */
  void				*dataAddr;	/* Data segment address */
  void				*bssAddr;	/* Bss segment address */
  int				textSize;	/* Text segment size */
  int				dataSize;	/* Data segment size */
  int				bssSize;	/* Bss segment size */
} MODULE_SEG_INFO;

typedef struct {
  char				name[NAME_MAX];	/* Module name */
  int				format;		/* Module format */
  int				group;		/* Module group */
  MODULE_SEG_INFO		segInfo;	/* Module segment info */
} MODULE_INFO;

IMPORT CLASS_ID moduleClassId;

IMPORT STATUS moduleLibInit(void);
IMPORT MODULE_ID moduleCreate(char *name, int format, int flags);
IMPORT STATUS moduleInit(MODULE_ID modId, char *name, int format, int flags);
IMPORT STATUS moduleDestroy(MODULE_ID modId, BOOL dealloc);
IMPORT STATUS moduleTerminate(MODULE_ID modId);
IMPORT STATUS moduleDelete(MODULE_ID modId);
IMPORT MODULE_ID moduleIdFigure(void *modNameOrId);
IMPORT int moduleIdListGet(MODULE_ID idList[], int max);
IMPORT char* moduleNameGet(MODULE_ID modId);
IMPORT int moduleGroupGet(MODULE_ID modId);
IMPORT int moduleFlagsGet(MODULE_ID modId);
IMPORT int moduleFormatGet(MODULE_ID modId);
IMPORT MODULE_ID moduleFindByName(char *name);
IMPORT MODULE_ID moduleFindByNameAndPath(char *name, char *path);
IMPORT MODULE_ID moduleFindByGroup(int group);
IMPORT MODULE_ID moduleEach(FUNCPTR func, ARG arg);
IMPORT STATUS moduleInfoGet(MODULE_ID modId, MODULE_INFO *pModInfo);
IMPORT STATUS moduleCheck(int options);
IMPORT STATUS moduleCreateHookAdd(FUNCPTR func);
IMPORT STATUS moduleCreateHookDelete(FUNCPTR func);
IMPORT STATUS moduleSegAdd(MODULE_ID modId,
			   int type,
			   void *localtion,
			   int length,
			   int flags);
IMPORT SEGMENT_ID moduleSegGet(MODULE_ID modId);
IMPORT SEGMENT_ID moduleSegFirst(MODULE_ID modId);
IMPORT SEGMENT_ID moduleSegNext(SEGMENT_ID segId);
IMPORT SEGMENT_ID moduleSegEach(MODULE_ID modId, FUNCPTR func, ARG arg);
IMPORT STATUS moduleSegInfoGet(MODULE_ID modId, MODULE_SEG_INFO *pSegInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _moduleLib_h */

