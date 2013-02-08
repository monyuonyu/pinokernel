BOOTDIR		:=	boot/
KERNELDIR	:=	kernel/

$(BOOTDIR) $(KERNELDIR):
	$(MAKE) --directory = $@
	
img :
	../tool/link boot/boot.bin Kernel/main.bin pinoc.img
	
run:
	qemu -fda pinoc.img
	