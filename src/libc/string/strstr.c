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

/* strstr.c - Find string within string */

#include <sys/types.h>
#include <rtos.h>
#include <stdlib.h>

char *strstr(const char *s1, const char *s2)
{
        FAST const char *s = s1;
        FAST const char *p = s2;

        do {
                if (!*p) {
                        return (char *) s1;;
                }
                if (*p == *s) {
                        ++p;
                        ++s;
                } else {
                        p = s2;
                        if (!*s) {
                                return NULL;
                        }
                        s = ++s1;
                }
        } while (1);
}

