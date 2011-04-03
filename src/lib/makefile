CC		= h8300-elf-gcc
C+		= h8300-elf-g++
AS		= h8300-elf-as
AR		= h8300-elf-ar
LD		= h8300-elf-ld
OBJCOPY= h8300-elf-objcopy
OBJDUMP= h8300-elf-objdump
STRIP	= h8300-elf-strip

OBJSC	= string.o stdlib.o ctype.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -O0 -Wall -I.

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