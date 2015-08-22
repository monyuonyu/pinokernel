#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/iodriver/h8300-hms/NIC/DP8390.c \
$(BD)/src/iodriver/h8300-hms/SCIF/3069f_sci.c \

all : reliod.rel
	# reliod.rel finish!!

reliod.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
