# Makefile -
#

OBJS = excLib.o funcTable.o sigLib.o selectLib.o logLib.o symLib.o symShow.o \
       pipeDrv.o vmLib.o cacheLib.o envLib.o envShow.o clockLib.o miscLib.o
SUBDIRS =
LIB_TGT = os

ADDED_DFLAGS += #-DDEBUG_SPLSEM -DNO_SPLSEM

include $(RTOS_BASE)/include/make/buildDefaults.mk
