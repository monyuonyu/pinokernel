MAINSRC		=	main
SEGLS		=	SEGLS.ls

img:
	#メインカーネル 573->1024byte
	gcc $(MAINSRC).c -c -o $(MAINSRC).o
	ld $(MAINSRC).o -T ../../common/$(SEGLS) -o $(MAINSRC).bin
	../../tool/FillNop 512 $(MAINSRC).bin
	../../tool/BootSigned $(MAINSRC).bin
	
	