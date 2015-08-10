#CommonMakefile
include ../../config/CommonMakefile

SRC = \
$(BD)/src/protocol/XMODEM/XMODEM.c \

all : libprotocol.a
	# libprotocol.a finish!!

libprotocol.a : $(OBJ)
	$(AR) rv $@  *.o
