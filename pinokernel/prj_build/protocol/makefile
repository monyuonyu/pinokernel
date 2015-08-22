#CommonMakefile
include ../../config/CommonMakefile

SRC = \
$(BD)/src/protocol/XMODEM/XMODEM.c \

all : relprotocol.rel
	# relprotocol.rel finish!!

relprotocol.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
