include ../../gcctool.g

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
