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

libc.a : $(OBJS)
	$(AR) rv $@  $(OBJS)
	mv libc.a ../libc.a
	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)

clean :
	rm -f *.o *.a
