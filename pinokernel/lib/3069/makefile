CC		= h8300-elf-gcc
C+		= h8300-elf-g++
AS		= h8300-elf-as
AR		= h8300-elf-ar
LD		= h8300-elf-ld
OBJCOPY= h8300-elf-objcopy
OBJDUMP= h8300-elf-objdump
STRIP	= h8300-elf-strip

OBJS	= 3069f_sci.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -O0 -Wall -I ../include

all : lib3069.a
	# lib3069.a finish!!

lib3069.a : $(OBJS)
	$(AR) rv $@  $(OBJS)
	mv lib3069.a ../lib3069.a
	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)

clean :
	rm -f *.o *.a
