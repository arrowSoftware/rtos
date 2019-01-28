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

/* kernQLib.h - Kernel work queue library */

#ifndef _kernQLib_h
#define _kernQLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Struct */
typedef struct {
    FUNCPTR func;    /* ptr to function that was added */
    int     numArgs; /* # of arguments to added item */
    ARG     arg1;    /* 1st argument of added function (if applicable) */
    ARG     arg2;    /* 2nd argument of added function (if applicable) */
} KERN_JOB;

/* Globals */
extern volatile BOOL           kernQEmpty;

/* Functions */
extern STATUS kernQLibInit (void);
extern void kernQAdd0 (FUNCPTR func);
extern void kernQAdd1 (FUNCPTR func, ARG arg1);
extern void kernQAdd2 (FUNCPTR func, ARG arg1, ARG arg2);
extern void kernQDoWork (void);
extern void kernQPanic (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _kernQLib_h */

