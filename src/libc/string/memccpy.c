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

/* memccpy.c - copy bytes in memory */

#include <stdlib.h>
#include <sys/types.h>
#include <rtos.h>

void * memccpy (
    void *        s1,
    const void *  s2,
    int           c,
    size_t        n
    ) {
    const u_int8_t * sp = (const u_int8_t *) s2;
    u_int8_t * dp = (u_int8_t *) s1;

    for (; (n > 0); n--, sp++) {
        *dp++ = *sp;
        if (*sp == c) {
            return (dp);
        }
    }

    return (NULL);
}
