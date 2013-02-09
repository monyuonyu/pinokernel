include ../../../gcctool.g

OBJS	= atoi.o atol.o atoll.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -Os -Wall -I ../../include

all :  $(OBJS)
	mv *.o ../
	# stdlib finish!!

	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)

clean :
	rm -f *.o *.a
