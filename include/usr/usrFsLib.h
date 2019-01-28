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

/* usrFsLib.h - User filesystem tools header */

#ifndef _usrFsLib_h
#define _usrFsLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

IMPORT void usrFsLibInit(void);
IMPORT void ioHelp(void);
IMPORT STATUS cd(const char *path);
IMPORT void pwd(void);
IMPORT STATUS ls(char *dirname, BOOL doLong);
IMPORT STATUS ll(char *dirname);
IMPORT STATUS lsr(char *dirname);
IMPORT STATUS llr(char *dirname);
IMPORT STATUS copy(const char *inFilename, const char *outFilename);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _usrFsLib_h */

