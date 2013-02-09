include ./gcctool.g

all: pinoc
	# PINoC Make finish!!
	
pinoc: os
	
os: libs
	cd ./os; $(MAKE)
	
libs:
	cd ./lib; $(MAKE)
	
clean:
	cd ./lib; $(MAKE) clean
	cd ./os; $(MAKE) clean
	rm *.mot
	# ALL CLEAN!!