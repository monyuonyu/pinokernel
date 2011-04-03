all: pinoc
	# PINoC Make finish!!
	
pinoc:
	cd ./src; $(MAKE)
	
clean:
	cd ./src; $(MAKE) clean
	# ALL CLEAN!!
