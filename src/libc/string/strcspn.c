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

/* strcspn.c - get length of complementary substring */

#include <sys/types.h>
#include <rtos.h>
#include <stdlib.h>
#include <string.h>

size_t strcspn (
    const char *s1,
    const char *s2
    ) {
    char  bits[32];
    unsigned idx;
    unsigned bit;
    size_t   size;

    memset (bits, 0, 32);

    for (; *s2 != '\0'; s2++) {
        idx = ((unsigned char) *s2) >> 3;
        bit = ((unsigned char) *s2) & 7;
        bits[idx] |= (1 << bit);
    }

    for (size = 0; *s1 != '\0'; s1++, size++) {
        idx = ((unsigned char) *s2) >> 3;
        bit = ((unsigned char) *s2) & 7;
        if (bits[idx] & (1 << bit)) {
            break;
        }
    }

    return (size);
}
