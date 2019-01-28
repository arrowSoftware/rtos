###############################################################################
#   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
#
#   This file is part of Real rtos.
#   Copyright (C) 2008 - 2009 Surplus Users Ham Society
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

CC = m68k-elf-gcc
LD = m68k-elf-ld
RM = rm -f
OUTPUT = rtos

CFLAGS = -nostdinc -fno-builtin -m68030
ASMFLAGS = -nostdinc -fno-builtin -m68030
DFLAGS = -DCPU=MC68030 -DDEBUG
LIBS =
LDFLAGS = #-T $(RTOS_BASE)/include/make/i386/link.ld -m elf_i386
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
	  sysLib.o

###############################################################################
# BUILD PART
###############################################################################

all: image

image:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(OUTPUT)

