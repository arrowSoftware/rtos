RM     = rm -rvf
OUTPUT = rtos
#AR     = /usr/local/i386elfgcc/bin/i386-elf-ar
#CC     = /usr/local/i386elfgcc/bin/i386-elf-gcc
#LD     = /usr/local/i386elfgcc/bin/i386-elf-ld
#GDB    = /usr/local/i386elfgcc/bin/i386-elf-gdb
AR = ar
CC = gcc
LD = ld
GDB = gdb
# $(ADDED_*FLAGS) is to come from the Makefile.
# $(EXTRA_*FLAGS) is to come from the command line.

CFLAGS   = -nostdinc -fno-builtin -m32 -fno-stack-protector $(ADDED_CFLAGS) $(EXTRA_CFLAGS)
INCFLAGS = -I$(RTOS_BASE)/include -I$(RTOS_BASE)/src $(ADDED_INCFLAGS) $(EXTRA_INCFLAGS)
DFLAGS   = -DCPU=$(CPU) $(ADDED_DFLAGS) $(EXTRA_DFLAGS)
ARFLAGS  = -r

# default build
ifeq ($(CPU),)
    CPU = PENTIUM2
endif

%.o: %.c
	@mkdir -p $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)
	$(CC) $(CFLAGS) $(INCFLAGS) $(DFLAGS) -c $^ -o $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)/$@
	$(AR) $(ARFLAGS) $(RTOS_BASE)/lib/$(CPU)/lib$(LIB_TGT).a $(RTOS_BASE)/lib/$(CPU)/$(LIB_TGT)/$@

#
# Added $(OBJS) to list of dependencies to cover case when a directory
# has both SUBDIRS and files to compile.  If the $(OBJS) is not present
# here then only the SUBDIRS will build for this case.
#
$(SUBDIRS): force_look $(OBJS)
	@$(foreach subdir, $(SUBDIRS), $(MAKE) -C $(subdir);)

default: $(OBJS) $(SUBDIRS)

force_look:
	true

clean:
	$(RM) $(RTOS_BASE)/lib/$(CPU)

help:
	@echo "make CPU=<cpu>, where <cpu> is one of the following ..."
	@echo "     PENTIUM2"
	@echo ""
	@echo "clean  - does not yet exist"
	@echo "       - current rules force a rebuild"
	@echo "       - to be fixed later"
	@echo ""
	@echo "rclean - see 'clean'"
	@echo ""
