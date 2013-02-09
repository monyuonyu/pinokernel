include ../gcctool.g

all : libs
	# libs finish!!

libs : libc.a lib3069.a

libc.a :
	cd ./c; $(MAKE)
	
lib3069.a:
	cd ./3069; $(MAKE)

clean :
	cd ./c; $(MAKE) clean
	cd ./3069; $(MAKE) clean
	rm -f *.o *.a
	