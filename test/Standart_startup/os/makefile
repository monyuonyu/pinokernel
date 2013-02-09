include ../gcctool.g

PINoC	= pinoc

OBJS	= vectors.o main.o start.o interrupt.o intr.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -O0 -Wall  -I ../lib/include -I ../lib/3069
LDFLAGS	= -T os.ls  -L ../lib -l3069 -lc

all : $(PINoC).mot
	# os finish!!

$(PINoC) : $(OBJS)
	$(LD) -o $(PINoC) $(OBJS) $(LDFLAGS) 
	cp $(PINoC) $(PINoC).elf
	$(STRIP) $(PINoC)
	
$(PINoC).mot : $(PINoC)
	$(OBJCOPY) -O srec $(PINoC) $(PINoC).mot
	mv $(PINoC).mot ../$(PINoC).mot
	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)
	
.s.o : $<
	$(CC) -c -v $< $(CFLAGS)
	
.S.o : $<
	$(CC) -c -v $< $(CFLAGS)
	
as : 
	$(CC) -S -v $(CFLAGS) main.c vectors.c
	
clean :
	rm -f *.o pinoc*
	