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

/* strncat.c - String concat */

#include <sys/types.h>
#include <rtos.h>

char *strncat (
    char *        s1,
    const char *  s2,
    size_t        n
    ) {
    char *s = s1;

    if (n != 0) {

        /* Find end of <s1> */
        while (*s++ != EOS) {
        }
        --s;

        /* Append at most <n> characters to s1. */
        while (((*s++ = *s2++) != EOS) && (--n > 0)) {
        }

        if (n == 0) {    /* Broke out due to <n> reaching 0. */
            *s = EOS;   /* Ensure EOS is there */
        }
    }

    return s1;
}

