all: pinoc
	# PINoC Make finish!!
	
pinoc: bootld kernel
	
bootld: libs
	cd ./bootloader; $(MAKE)
	
kernel: libs
	cd ./kernel; $(MAKE)
	
libs:
	cd ./lib; $(MAKE)
	
clean:
	cd ./lib; $(MAKE) clean
	cd ./bootloader; $(MAKE) clean
	cd ./kernel; $(MAKE) clean
	# ALL CLEAN!!