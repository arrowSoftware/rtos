###############################################################################
#   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
#
#   This file is part of Real rtos.
#   Copyright (C) 2008 - 2009 Surplus Users Ham Society
#
#   Real rtos is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   Real rtos is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Real rtos.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

###############################################################################
# COMPILE PART
###############################################################################

CC = sh-elf-gcc
LD = sh-elf-ld
RM = rm -f
OUTPUT = rtos

CFLAGS = -m4-single-only -ml -nostdinc -fno-builtin -fno-strict-aliasing -fomit-frame-pointer -fno-optimize-sibling-calls
ASMFLAGS = -m4-single-only -ml -nostdinc -fno-builtin
DFLAGS = -DCPU=SH7750 -DDEBUG
LIBS =
LDFLAGS = -T $(RTOS_BASE)/include/make/sh/link.ld
#LDFLAGS = -nostartfiles -nostdlib -m4-single-only -ml -Wl,-Ttext=0x8c010000
INCFLAGS = -I$(RTOS_BASE)/include -I$(RTOS_BASE)/src

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(INCFLAGS) $(DFLAGS) -o $*.o -c $*.c
.s.o:
	$(CC) $(ASMFLAGS) $(INCFLAGS) $(DFLAGS) -o $*.o -x assembler-with-cpp -c $*.s

###############################################################################
# LINK PART
###############################################################################

OBJECTS = sysALib.o \
	  sysLib.o \
	  ../all/usrConfigMin.o

###############################################################################
# BUILD PART
###############################################################################

all: image

image:	$(OBJECTS)
	$(LD) $(LDFLAGS) $(LIBS) -o $(OUTPUT) $(OBJECTS)

clean:	$(OBJECTS) $(OUTPUT)
	$(RM) $(OBJECTS) $(OUTPUT)

