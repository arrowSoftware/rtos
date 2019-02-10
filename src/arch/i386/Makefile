AR       = ar
CC       = gcc
LD       = ld
GDB      = gdb
#AR     = /usr/local/i386elfgcc/bin/i386-elf-ar
#CC     = /usr/local/i386elfgcc/bin/i386-elf-gcc
#LD     = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB    = /usr/local/i386elfgcc/bin/i386-elf-gdb
OBJS     = excALib.o excArchLib.o intALib.o intArchLib.o kernALib.o memcpy.o \
    	   mmuALib.o mmuArchLib.o sysALib.o taskArchLib.o sigCtxALib.o \
	       sigCtxArchLib.o excArchShow.o cacheALib.o cacheArchLib.o \
           elfArchLib.o
LIB_TGT  = arch
CPU      = PENTIUM2
CFLAGS   = -nostdinc -fno-builtin -m32 -fno-stack-protector -Wall
ASMFLAGS = -nostdinc -fno-builtin -m32
INCFLAGS = -I$(RTOS_BASE)/include -I$(RTOS_BASE)/src
DFLAGS   = -DCPU=$(CPU)
ARFLAGS  = -r

.SUFFIXES: .c

.c.o:
	@mkdir -p $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)
	$(CC) $(CFLAGS) $(INCFLAGS) $(DFLAGS) -o $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)/$*.o -c $*.c
	$(AR) $(ARFLAGS) $(RTOS_BASE)/lib/$(CPU)/lib$(LIB_TGT).a $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)/$*.o

.s.o:
	@mkdir -p $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)
	$(CC) $(ASMFLAGS) $(INCFLAGS) $(DFLAGS) -o $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)/$*.o -x assembler-with-cpp -c $*.s
	$(AR) $(ARFLAGS) $(RTOS_BASE)/lib/$(CPU)/lib$(LIB_TGT).a $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)/$*.o

default: $(OBJS)

clean:
	rm -rf $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)

