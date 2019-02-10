RM     = rm -rvf
OUTPUT = rtos
AR     = /usr/local/i386elfgcc/bin/i386-elf-ar
CC     = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD     = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB    = /usr/local/i386elfgcc/bin/i386-elf-gdb
NASM = nasm
#AR = ar
#CC = gcc
#LD = ld
#GDB = gdb

CPU = PENTIUM2
CFLAGS = -nostdinc -fno-builtin -m32 -fno-stack-protector
ASMFLAGS = -nostdinc -fno-builtin -m32
DFLAGS = -DCPU=$(CPU) -DCOMPILE_DATE=\"`date +%d_%B_%Y.%H:%M`\"
LIBS = -lstdc -lfs -ltools -lio -loldfs -los -lrtos -larch -lkern -lugl-2d -lugl-os -lugl-driver -lutil -lnet -lnetinet -lusr
LDFLAGS = -T $(RTOS_BASE)/include/make/i386/link.ld -m elf_i386 multiboot_header.o
INCFLAGS = -I./ -I../all -I$(RTOS_BASE)/include -I$(RTOS_BASE)/src
LD_LIBRARY_PATH = -L$(RTOS_BASE)/lib/$(CPU)/

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(INCFLAGS) $(DFLAGS) -o $*.o -c $*.c
.s.o:
	$(CC) $(ASMFLAGS) $(INCFLAGS) $(DFLAGS) -o $*.o -x assembler-with-cpp -c $*.s

###############################################################################
# LINK PART
###############################################################################

OBJECTS = sysALib.o sysLib.o ../all/usrConfig.o ../all/usrKernel.o \
          ../all/usrMmuInit.o ../all/usrNetwork.o ../all/standTable.o \
          ../all/pcDemo.o

all: image

image:	$(OBJECTS)
	$(NASM) -f elf32 multiboot_header.asm
	$(LD) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS) $(LD_LIBRARY_PATH) $(LIBS)

clean:
	$(RM) $(OBJECTS) $(OUTPUT) multiboot_header.o
