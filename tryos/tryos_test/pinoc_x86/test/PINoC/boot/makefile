BOOTSCT	=	bootsct
BOOTLD		=	bootld
BOOTSCTLS	=	bootsct.ls
BOOTLDLS	=	bootld.ls

img:
	#ブートセクタ 134->512byte
	g++ src/$(BOOTSCT).cpp -c -o $(BOOTSCT).o -fno-exceptions
	ld $(BOOTSCT).o -T $(BOOTSCTLS) -o $(BOOTSCT).bin
	../../../tool/FillNop 512 $(BOOTSCT).bin
	../../../tool/BootSigned $(BOOTSCT).bin
	#ブートローダ 2071->2560byte
	g++ src/$(BOOTLD).cpp -c -o $(BOOTLD).o -fno-exceptions
	ld $(BOOTLD).o -T $(BOOTLDLS) -o $(BOOTLD).bin
	../../../tool/FillNop 512 $(BOOTLD).bin
	#ブートデータ生成 512+2560 = 3072byte
	../../../tool/link $(BOOTSCT).bin $(BOOTLD).bin boot.bin


run:
	qemu -fda boot.bin
	
debug:
	qemu -S -s -fda boot.bin

del:
	rm -f *.o
	rm -f *.bin
	rm -f *.s

rundel:
	make img
	make run
	make del

as:
	g++ src/$(BOOTSCT).cpp -S -fno-exceptions
	g++ src/$(BOOTLD).cpp -S -fno-exceptions
