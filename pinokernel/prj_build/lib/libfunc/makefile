#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/lib/libfunc/asciitobin.c \
$(BD)/src/lib/libfunc/decto10.c \

all : relfunc.rel
	# relfunc.rel finish!!

relfunc.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
