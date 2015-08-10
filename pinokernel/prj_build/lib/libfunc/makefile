#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/lib/libfunc/asciitobin.c \
$(BD)/src/lib/libfunc/decto10.c \

all : libfunc.a
	# libfunc.a finish!!

libfunc.a : $(OBJ)
	$(AR) rv $@  *.o
