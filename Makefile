all:
	cd $(RTOS_BASE)/src; make
	cd $(RTOS_BASE)/src/build/i386; make
	cp $(RTOS_BASE)/src/build/i386/rtos $(RTOS_BASE)/src/build/i386/rtos_iso/boot/.
	grub-mkrescue -o $(RTOS_BASE)/src/build/i386/rtos.iso $(RTOS_BASE)/src/build/i386/rtos_iso

clean:
	cd $(RTOS_BASE)/src; make clean
	cd $(RTOS_BASE)/src/build/i386; make clean
	rm -vf $(RTOS_BASE)/src/build/i386/rtos.iso

