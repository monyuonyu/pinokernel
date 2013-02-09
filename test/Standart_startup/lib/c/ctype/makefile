include ../../../gcctool.g

OBJS	= tolower.o isupper.o ctype_.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -Os -Wall -I ../../include

all :  $(OBJS)
	mv *.o ../
	# ctype finish!!

	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)

clean :
	rm -f *.o *.a
