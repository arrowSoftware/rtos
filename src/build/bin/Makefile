###############################################################################
#   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
#
#   This file is part of Real rtos.
#   Copyright (C) 2008 - 2010 Surplus Users Ham Society
#
#   Real rtos is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation, either version 2.1 of the License, or
#   (at your option) any later version.
#
#   Real rtos is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with Real rtos.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

###############################################################################
# COMPILE PART
###############################################################################

CC = gcc
RM = rm -f

CPU = PENTIUM2
CFLAGS = -c -nostdinc -fno-builtin -m32 -fno-stack-protector
DFLAGS = -DCPU=$(CPU) \
	 -DCOMPILE_DATE=\"`date +%d_%B_%Y.%H:%M`\"
INCFLAGS = -I./ -I../all -I$(RTOS_BASE)/include -I$(RTOS_BASE)/src

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(INCFLAGS) $(DFLAGS) -o $*.o -c $*.c

###############################################################################
# OBJECT FILES
###############################################################################

OBJECTS = calc.o cat.o

###############################################################################
# BUILD PART
###############################################################################

all: image

image:	$(OBJECTS)

clean:
	$(RM) $(OBJECTS)

