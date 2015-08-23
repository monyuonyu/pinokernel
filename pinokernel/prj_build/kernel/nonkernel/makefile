#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/kernel/$(KERNEL)/nonkernel.c \

#all : relkernel.rel $(PINoC).mot
all : relkernel.rel
	# ALL SYSTEM BUILD finish!!

relkernel.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
