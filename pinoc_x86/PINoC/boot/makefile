BOOTSRC		=	bootsct
BOOTLD		=	bootld
SEGLS		=	SEGLS.ls

img:
	#ブートセクタ 134->512byte
	gcc $(BOOTSRC).c -c -o $(BOOTSRC).o
	ld $(BOOTSRC).o -T ../../common/$(SEGLS) -o $(BOOTSRC).bin
	../../tool/FillNop 512 $(BOOTSRC).bin
	../../tool/BootSigned $(BOOTSRC).bin
	#ブートローダ 2071->2560byte
	gcc $(BOOTLD).c -c -o $(BOOTLD).o
	ld $(BOOTLD).o -T ../../common/$(SEGLS) -o $(BOOTLD).bin
	../../tool/FillNop 512 bootld.bin
	#ブートデータ生成 512+2560 = 3072byte
	../../tool/link $(BOOTSRC).bin $(BOOTLD).bin boot.bin

run:
	qemu -fda boot.bin
	
debug:
	qemu -S -s -fda boot.bin

del:
	rm -f *.o
	rm -f *.bin
	rm -f $(BOOTSRC).s

rundel:
	make img
	make run
	make del

as:
	gcc $(BOOTSRC).c -S
	gcc $(BOOTLD).c -S
