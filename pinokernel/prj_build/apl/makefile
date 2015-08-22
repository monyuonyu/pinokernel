#CommonMakefile
include ../../config/CommonMakefile

SRC = \
$(BD)/src/apl/FREE/free_main.c \

all : relapl.rel
	# relapl.rel finish!!

relapl.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
