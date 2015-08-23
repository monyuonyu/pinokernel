#CommonMakefile
include ../config/CommonMakefile

all: pinokernel
	# pinokernel Make finish!!

pinokernel: lib iodriver protocol bootloader kernel apl bin

libs:
	#cd ./lib; $(MAKE)
	cd $(BD)/prj_build/lib/libc; $(MAKE) all
	cd $(BD)/prj_build/lib/libfunc; $(MAKE) all

iodriver: libs
	cd $(BD)/prj_build/iodriver/$(TARGET_WIDE); $(MAKE) all

protocol: libs iodriver
	cd $(BD)/prj_build/protocol; $(MAKE) all

bootloader: libs iodriver protocol
	cd $(BD)/prj_build/bootloader/$(TARGET_DETAIL); $(MAKE) all

kernel: libs iodriver protocol bootloader
	cd $(BD)/prj_build/kernel/$(KERNEL); $(MAKE) all

apl: libs iodriver protocol bootloader kernel
	cd $(BD)/prj_build/apl; $(MAKE) all

bin: libs iodriver protocol bootloader kernel apl
	$(LD) -o $(KERNEL).elf $(LDFLAGS) \
	$(BD)/prj_build/apl/relapl.rel \
	$(BD)/prj_build/bootloader/$(TARGET_DETAIL)/relboot.rel \
	$(BD)/prj_build/iodriver/$(TARGET_WIDE)/reliod.rel \
	$(BD)/prj_build/kernel/$(KERNEL)/relkernel.rel \
	$(BD)/prj_build/lib/libc/relc.rel \
	$(BD)/prj_build/lib/libfunc/relfunc.rel \
	$(BD)/prj_build/protocol/relprotocol.rel

	mv $(KERNEL).elf ../release/
	mv $(KERNEL).mot ../release/
	mv $(KERNEL).bin ../release/
