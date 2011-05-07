CC		= h8300-elf-gcc
C+		= h8300-elf-g++
AS		= h8300-elf-as
LD		= h8300-elf-ld
OBJCOPY= h8300-elf-objcopy
OBJDUMP= h8300-elf-objdump
STRIP	= h8300-elf-strip

PINoC	= pinoc_kernel

OBJS	= start.o pinoc.o dispatch.o

LDFLAGS	= -T kernel.ls -lc -l3069 -L ../../newlib/lib/h8300h -L ../lib
CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -O0 -Wall -I ../../newlib/include -I ../lib/include -I ../lib/3069

all : $(PINoC).mot
	# bootld finish!!

$(PINoC) : $(OBJS)
	$(LD) -o $(PINoC) $(OBJS) $(LDFLAGS) 
	cp $(PINoC) $(PINoC).elf
	$(STRIP) $(PINoC)
	
$(PINoC).mot : $(PINoC)
	$(OBJCOPY) -O srec $(PINoC) $(PINoC).mot
	
.c.o : $<
	$(CC) -c -v $< $(CFLAGS)
	
.s.o : $<
	$(CC) -c -v $< $(CFLAGS)
	
as : 
	$(CC) -S -v stack.s main.c vectors.c start.c
	
clean :
	rm -f *.o *.mot *.elf
	