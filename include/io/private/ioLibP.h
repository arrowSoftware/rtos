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

#ifndef __IOLIBP_H
#define __IOLIBP_H

#include <stdarg.h>
#include <io/iosLib.h>

int  ioConnect (int (*func)(DEV_HEADER *, char *, va_list), const char *, ...);
int  openConnect (DEV_HEADER *, char *, va_list);
int  creatConnect (DEV_HEADER *, char *, va_list);
int  removeConnect (DEV_HEADER *, char *, va_list);

#endif
