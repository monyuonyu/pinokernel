#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/kernel/$(KERNEL)/dispatch.S \
$(BD)/src/kernel/$(KERNEL)/pinoc.c \
$(BD)/src/kernel/$(KERNEL)/start.S \
# $(BD)/src/kernel/$(KERNEL)/cvector.c \

#all : relkernel.rel $(PINoC).mot
all : relkernel.rel
	# ALL SYSTEM BUILD finish!!

relkernel.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
