#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/iodriver/h8300-hms/NIC/DP8390.c \
$(BD)/src/iodriver/h8300-hms/SCIF/3069f_sci.c \

all : libiod.a
	# libiod.a finish!!

libiod.a : $(OBJ)
	$(AR) rv $@  *.o
