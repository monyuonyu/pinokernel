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
	cd $(BD)/prj_build/iodriver/$(TARGET); $(MAKE) all

protocol: libs iodriver
	cd $(BD)/prj_build/protocol; $(MAKE) all

bootloader: libs iodriver protocol
	cd $(BD)/prj_build/bootloader/$(TARGET); $(MAKE) all

kernel: libs iodriver protocol bootloader
	cd $(BD)/prj_build/kernel/$(KERNEL); $(MAKE) all

apl: libs iodriver protocol bootloader kernel
	cd $(BD)/prj_build/apl; $(MAKE) all

bin: libs iodriver protocol bootloader kernel apl
	$(LD) -o $(KERNEL).elf $(LDFLAGS) \
	$(BD)/prj_build/apl/libapl.a \
	$(BD)/prj_build/bootloader/$(TARGET)/libboot.a \
	$(BD)/prj_build/iodriver/$(TARGET)/libiod.a \
	$(BD)/prj_build/kernel/pinok/libkernel.a \
	$(BD)/prj_build/lib/libc/libc.a \
	$(BD)/prj_build/lib/libfunc/libfunc.a \
	$(BD)/prj_build/protocol/libprotocol.a

	cp $(KERNEL).elf $(KERNEL).bin
	$(STRIP) $(KERNEL).bin
	$(OBJCOPY) -O srec $(KERNEL).elf $(KERNEL).mot

	mv $(KERNEL).elf ../release/
	mv $(KERNEL).mot ../release/
	mv $(KERNEL).bin ../release/
