RM     = rm -rvf
OUTPUT = rtos
AR     = i386-elf-ar
CC     = i386-elf-gcc
LD     = i386-elf-ld
GDB    = i386-elf-gdb
#AR = ar
#CC = gcc
#LD = ld
#GDB = gdb
NASM = nasm

CPU = PENTIUM2
CFLAGS = -nostdinc -fno-builtin -m32 -fno-stack-protector -g
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

OBJECTS = sysALib.o sysLib.o usrConfig.o usrKernel.o \
          usrMmuInit.o usrNetwork.o \
          pcDemo.o symTblAll.o

all: image

image: $(OBJECTS)
	#rm -f rtos.symbols
	#find ../../../ -name "*.o" | xargs nm -g >> rtos.symbols
	#python createSymTbl.py rtos.symbols symTblAll.c
	#$(CC) $(CFLAGS) $(INCFLAGS) $(DFLAGS) -o symTblAll.o -c symTblAll.c
	$(NASM) -f elf32 multiboot_header.asm
	$(LD) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS) $(LD_LIBRARY_PATH) $(LIBS)

clean:
	$(RM) $(OBJECTS) $(OUTPUT) multiboot_header.o rtos.symbols
