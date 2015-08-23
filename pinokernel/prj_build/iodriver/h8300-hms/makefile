#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/iodriver/$(TARGET_WIDE)/NIC/DP8390.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/SCIF/3069f_sci.c \

all : reliod.rel
	# reliod.rel finish!!

reliod.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
