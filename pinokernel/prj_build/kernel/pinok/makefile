#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/kernel/pinok/dispatch.S \
$(BD)/src/kernel/pinok/pinoc.c \
$(BD)/src/kernel/pinok/start.S \
# $(BD)/src/kernel/pinok/cvector.c \

#all : relkernel.rel $(PINoC).mot
all : relkernel.rel
	# ALL SYSTEM BUILD finish!!

relkernel.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
