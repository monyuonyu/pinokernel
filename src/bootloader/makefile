CC		= h8300-elf-gcc
C+		= h8300-elf-g++
AS		= h8300-elf-as
LD		= h8300-elf-ld
OBJCOPY= h8300-elf-objcopy
OBJDUMP= h8300-elf-objdump
STRIP	= h8300-elf-strip

PINoC	= pinoc

OBJS	= vectors.o main.o start.o XMODEM.o load_mot.o

CFLAGS	= -mh -nostdinc -nostdlib -fno-builtin -O0 -Wall -I ../lib/include
LDFLAGS	= -T boot.ls -lc -l3069 -L ../lib

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
	rm -f *.o pinoc*
	