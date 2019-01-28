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

/* iosLibP.h - Serial io driver private header */

#ifndef _iosLibP_h
#define _iosLibP_h

#define FOLLOW_LINK     -10   /* follow link to a new device */
#define FOLLOW_DOTDOT   -11   /* follow ".." to a new device */

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  DEV_HEADER *pDevHeader;
  ARG value;
  char *name;
  int taskId;
  BOOL used;
  BOOL obsolete;
  void *auxValue;
  void *reserved;
} FD_ENTRY;

typedef struct {
  FUNCPTR dev_create;
  FUNCPTR dev_delete;
  FUNCPTR dev_open;
  FUNCPTR dev_close;
  FUNCPTR dev_read;
  FUNCPTR dev_write;
  FUNCPTR dev_ioctl;
  BOOL dev_used;
} DRV_ENTRY;

#define STD_FIX(fd)	((fd) + 3)
#define STD_UNFIX(fd)	((fd) - 3)
#define STD_MAP(fd)	(STD_UNFIX(((fd) >= 0 && (fd) < 3) ? \
				ioTaskStdGet(0, fd) : (fd)))
#define FD_CHECK(fd)	(((fd) >= 0 && (fd) < iosMaxFd && \
			iosFdTable[(fd)].used && !iosFdTable[(fd)].obsolete) ?\
			&iosFdTable[(fd)] : NULL)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _iosLibP_h */

