include ../../../gcctool.g

OBJS	= memcmp.o memccpy.o strncmp.o memset.o memcpy.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -Os -Wall -I ../../include

all :  $(OBJS)
	mv *.o ../
	# string finish!!

	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)

clean :
	rm -f *.o *.a
