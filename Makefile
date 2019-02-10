all:
	cd $(RTOS_BASE)/src; make
	cd $(RTOS_BASE)/src/build/i386; make

clean:
	cd $(RTOS_BASE)/src; make clean
	cd $(RTOS_BASE)/src/build/i386; make clean

