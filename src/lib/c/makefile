CC		= h8300-elf-gcc
C+		= h8300-elf-g++
AS		= h8300-elf-as
AR		= h8300-elf-ar
LD		= h8300-elf-ld
OBJCOPY= h8300-elf-objcopy
OBJDUMP= h8300-elf-objdump
STRIP	= h8300-elf-strip

OBJS	= string.o stdlib.o ctype.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -Os -Wall -I ../include

all : libc.a
	# libc.a finish!!

libc.a : OBJS
	$(AR) rv $@  $(OBJS)
	mv libc.a ../libc.a
	
OBJS :
	## ライブラリのコンパイル ##
	cd ./ctype; $(MAKE)
	cd ./stdlib; $(MAKE)
	cd ./string; $(MAKE)

clean :
	rm -f *.o *.a
	cd ./ctype; $(MAKE) clean
	cd ./stdlib; $(MAKE) clean
	cd ./string; $(MAKE) clean
	